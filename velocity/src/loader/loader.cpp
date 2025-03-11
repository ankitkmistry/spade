#include "loader.hpp"
#include "../ee/vm.hpp"
#include "../objects/float.hpp"
#include "../objects/int.hpp"
#include "verifier.hpp"

namespace spade
{
    Loader::Loader(SpadeVM *vm) : vm(vm), manager(vm->getMemoryManager()) {}

    ObjMethod *Loader::load(const string &path) {
        vector<ObjModule *> toBeLoaded;
        {    // Read

            struct ModInfo {
                vector<string> deps;
                uint32 i;
            };

            vector<ModInfo> moduleStack;
            ObjModule *module = readModule(path);
            ModInfo modInfo = {.deps = module->getDependencies(), .i = 0};
            moduleStack.push_back(modInfo);
            toBeLoaded.push_back(module);
            while (!moduleStack.empty()) {
                modInfo = moduleStack.back();
                if (modInfo.i >= modInfo.deps.size()) {
                    moduleStack.pop_back();
                }
                for (; modInfo.i < modInfo.deps.size(); ++modInfo.i) {
                    string depPath = modInfo.deps[modInfo.i];
                    auto depModule = readModule(depPath);
                    if (depModule->getState() == ObjModule::State::NOT_READ) {
                        ModInfo depModInfo = {.deps = module->getDependencies(), .i = 0};
                        moduleStack.push_back(depModInfo);
                        toBeLoaded.push_back(depModule);
                    }
                }
            }
        }
        {    // Load
            for (auto module: toBeLoaded) {
                current = module;
                loadModule(module);
            }
            current = null;
            // Complain about the unresolved types
            for (const auto &[sign, reference]: referencePool) {
                if (reference->getKind() == Type::Kind::UNRESOLVED) {
                    throw IllegalAccessError(std::format("reference not found: '{}'", sign));
                    // TODO give a detailed error message for all the unresolved references
                }
            }
        }
        {    // Init
            for (auto module: toBeLoaded) {
                ObjMethod *init = module->getInit();
                if (init != null) {
                    init->invoke({});
                    module->setState(ObjModule::State::INITIALIZED);
                }
            }
        }

        auto module = toBeLoaded[0];
        auto elp = module->getElp();
        // If this is an executable one, get the entry point
        if (elp.type == 0x01) {
            // Load the constant pool
            vector<Obj *> constPool = module->getConstantPool();
            // Get the param of the entry point
            auto entrySign = constPool[elp.entry]->toString();
            // Return the entry point
            return cast<ObjMethod>(vm->getSymbol(entrySign));
        }
        return null;
    }

    string Loader::resolvePath(const string &pathStr) {
        fs::path path{pathStr};
        fs::path result;

        if (path.is_absolute())
            result = path;
        else if (path.string()[0] == '.') {
            result = (getLoadPath() / path);
        } else {
            for (const auto &dir: vm->getSettings().modPath) {
                result = dir / path;
                if (exists(result))
                    return result.string();
            }
            result = getLoadPath() / path;
            if (exists(result))
                return result.string();
            result = fs::current_path() / path;
            if (exists(result))
                return result.string();
        }
        if (!exists(result)) {
            throw IllegalAccessError(std::format("path not found: {}", pathStr));
        }
        return result.string();
    }

    fs::path Loader::getLoadPath() {
        if (getCurrentModule() != null) {
            return getCurrentModule()->getPath().parent_path();
        }
        return fs::current_path();
    }

    ObjModule *Loader::readModule(const string &path) {
        auto absolutePath = get_absolute_path(path);
        // If the module is already read, return it
        if (auto it = modules.find(absolutePath); it != modules.end()) {
            return it->second;
        }
        // Or else, start reading a new module
        ElpReader reader{resolvePath(path)};
        auto elp = reader.read();
        // Verify it...
        Verifier verifier{elp, path};
        verifier.verify();
        // Construct the module
        auto constPool = readConstPool(elp.constantPool, elp.constantPoolCount);
        auto sign = Sign{constPool[elp.thisModule]->toString()};
        auto depObjs = cast<ObjArray>(constPool[elp.imports]);
        vector<string> deps;
        deps.reserve(depObjs->count());
        depObjs->foreach ([&deps](auto depObj) { deps.push_back(depObj->toString()); });
        vm->setMetadata(sign.toString(), readMeta(elp.meta));

        auto module = halloc<ObjModule>(manager, sign, path, constPool, deps, elp);
        module->setState(ObjModule::State::READ);
        // Insert the module to the module table
        modules[absolutePath] = module;
        return module;
    }

    void Loader::loadModule(ObjModule *module) {
        auto elp = module->getElp();
        // Load the objects
        for (int i = 0; i < elp.objectsCount; ++i) {
            auto obj = readObj(elp.objects[i]);
            module->setMember(obj->getSign().getName(), obj);
        }
        // Load the globals
        for (int i = 0; i < elp.globalsCount; ++i) {
            auto global = readGlobal(elp.globals[i]);
            module->setMember(global->getSign().getName(), global);
        }
        // Add the module to the globals
        vm->getModules()[module->getSign().getName()] = module;
        // Set the state to loaded
        module->setState(ObjModule::State::LOADED);
        // Try to get init point
        auto initSign = getConstantPool()[elp.init]->toString();
        if (!initSign.empty()) {
            module->setInit(cast<ObjMethod>(vm->getSymbol(initSign)));
        } else {
            module->setInit(null);
        }
    }

    Obj *Loader::readGlobal(GlobalInfo &global) {
        auto constPool = getConstantPool();
        auto sign = getSign(global.thisGlobal);
        auto type = findType(constPool[global.type]->toString());

        auto meta = readMeta(global.meta);
        vm->setMetadata(sign.toString(), meta);

        return makeObj(constPool[global.type]->toString(), sign, type);
    }

    Obj *Loader::readObj(ObjInfo &obj) {
        auto constPool = getConstantPool();
        switch (obj.type) {
            case 0x01:
                return readMethod(obj._method);
            case 0x02:
                return readClass(obj._class);
            default:
                throw Unreachable();
        }
    }

    Obj *Loader::readClass(ClassInfo &klass) {
        auto constPool = getConstantPool();
        Type::Kind kind;
        switch (klass.type) {
            case 0x01:
                kind = Type::Kind::CLASS;
                break;
            case 0x02:
                kind = Type::Kind::INTERFACE;
                break;
            case 0x03:
                kind = Type::Kind::ENUM;
                break;
            case 0x04:
                kind = Type::Kind::ANNOTATION;
                break;
            default:
                throw Unreachable();
        }
        auto sign = getSign(klass.thisClass);

        Table<NamedRef *> typeParams;
        for (int i = 0; i < klass.typeParamCount; ++i) {
            auto paramName = constPool[klass.typeParams[i].name]->toString();
            auto typeParam = halloc<NamedRef>(manager, paramName,
                                              halloc<TypeParam>(manager, Sign{paramName}, getCurrentModule()), Table<string>{});
            typeParams[paramName] = typeParam;
            referencePool[paramName] = cast<Type>(typeParam->getValue());
        }

        Table<Type *> supers;
        cast<ObjArray>(constPool[klass.supers])->foreach ([this, &supers](auto super) {
            auto str = super->toString();
            Type *type = findType(str);
            supers[type->getSign().toString()] = type;
        });

        Table<MemberSlot> members;
        for (int i = 0; i < klass.fieldsCount; ++i) {
            auto field = readField(klass.fields[i]);
            members[field->getSign().getName()] = MemberSlot{field, Flags{klass.fields[i].flags}};
        }
        for (int i = 0; i < klass.methodsCount; ++i) {
            auto method = readMethod(sign.toString(), klass.methods[i]);
            members[method->getSign().getName()] = MemberSlot{method};
        }
        for (int i = 0; i < klass.objectsCount; ++i) {
            auto object = readObj(klass.objects[i]);
            members[object->getSign().getName()] = MemberSlot{object};
        }

        auto meta = readMeta(klass.meta);
        vm->setMetadata(sign.toString(), meta);

        // Resolve the type params
        for (const auto &[name, _]: typeParams) {
            // Remove the unresolved types from ref pool
            referencePool.erase(name);
        }
        // Resolve the type
        auto type = resolveType(sign.toString(), {sign, kind, typeParams, supers, members, getCurrentModule()});
        vm->setSymbol(sign.toString(), type);
        return type;
    }

    Obj *Loader::readField(FieldInfo &field) {
        auto constPool = getConstantPool();
        auto sign = getSign(field.thisField);
        auto type = findType(constPool[field.type]->toString());

        auto meta = readMeta(field.meta);
        vm->setMetadata(sign.toString(), meta);

        return makeObj(constPool[field.type]->toString(), sign, type);
    }

    Obj *Loader::readMethod(const string &klassSign, MethodInfo &method) {
        auto constPool = getConstantPool();
        auto type = findType(klassSign);
        auto met = readMethod(method);
        met->setType(type);
        return met;
    }

    Obj *Loader::readMethod(MethodInfo &method) {
        auto constPool = getConstantPool();
        ObjMethod::Kind kind;
        switch (method.type) {
            case 0x01:
                kind = ObjMethod::Kind::FUNCTION;
                break;
            case 0x02:
                kind = ObjMethod::Kind::METHOD;
                break;
            case 0x03:
                kind = ObjMethod::Kind::CONSTRUCTOR;
                break;
            default:
                throw Unreachable();
        }
        auto sign = getSign(method.thisMethod);
        Table<NamedRef *> typeParams;
        for (int i = 0; i < method.typeParamCount; ++i) {
            auto paramName = constPool[method.typeParams[i].name]->toString();
            auto typeParam = halloc<NamedRef>(manager, paramName,
                                              halloc<TypeParam>(manager, Sign{paramName}, getCurrentModule()), Table<string>{});
            typeParams[paramName] = typeParam;
            referencePool[paramName] = cast<Type>(typeParam->getValue());
        }
        ArgsTable args{};
        for (int i = 0; i < method.argsCount; ++i) {
            args.addArg(readArg(method.args[i]));
        }
        LocalsTable locals{method.closureStart};
        for (int i = 0; i < method.localsCount; ++i) {
            locals.addLocal(readLocal(method.locals[i]));
        }
        ExceptionTable exceptions{};
        for (int i = 0; i < method.exceptionTableCount; ++i) {
            exceptions.addException(readException(method.exceptionTable[i]));
        }
        LineNumberTable lines{};
        for (int i = 0; i < method.lineInfo.numberCount; ++i) {
            auto numberInfo = method.lineInfo.numbers[i];
            lines.addLine(numberInfo.times, numberInfo.lineno);
        }
        vector<ObjMethod *> lambdas;
        lambdas.reserve(method.lambdaCount);
        for (int i = 0; i < method.lambdaCount; ++i) {
            lambdas.push_back(cast<ObjMethod>(readMethod(method.lambdas[i])));
        }
        vector<MatchTable> matches;
        matches.reserve(method.matchCount);
        for (int i = 0; i < method.matchCount; ++i) {
            matches.push_back(readMatch(method.matches[i]));
        }

        auto meta = readMeta(method.meta);
        vm->setMetadata(sign.toString(), meta);

        // Resolve the type params
        for (const auto &[name, _]: typeParams) {
            // Remove the unresolved types from ref pool
            referencePool.erase(name);
        }
        // Create the frame template
        auto frameTemplate = new FrameTemplate{method.codeCount, method.code, method.maxStack, args,   locals,
                                               exceptions,       lines,       lambdas,         matches};
        auto methodObj = halloc<ObjMethod>(manager, sign, kind, frameTemplate, null, typeParams, getCurrentModule());
        return methodObj;
    }

    MatchTable Loader::readMatch(MethodInfo::MatchInfo match) {
        auto constPool = getConstantPool();
        vector<Case> cases;
        cases.reserve(match.caseCount);
        for (int i = 0; i < match.caseCount; ++i) {
            auto kase = match.cases[i];
            cases.emplace_back(Obj::createCopy(constPool[kase.value]), kase.location);
        }
        return {cases, match.defaultLocation};
    }

    Exception Loader::readException(MethodInfo::ExceptionTableInfo &exception) {
        auto constPool = getConstantPool();
        auto type = findType(constPool[exception.exception]->toString());
        return {exception.startPc, exception.endPc, exception.targetPc, type, readMeta(exception.meta)};
    }

    NamedRef *Loader::readLocal(MethodInfo::LocalInfo &local) {
        auto constPool = getConstantPool();
        auto name = constPool[local.thisLocal]->toString();
        auto type = findType(constPool[local.type]->toString());
        auto meta = readMeta(local.meta);
        auto obj = makeObj(constPool[local.type]->toString(), type);
        return halloc<NamedRef>(manager, name, obj, meta);
    }

    NamedRef *Loader::readArg(MethodInfo::ArgInfo &arg) {
        auto constPool = getConstantPool();
        auto name = constPool[arg.thisArg]->toString();
        auto type = findType(constPool[arg.type]->toString());
        auto meta = readMeta(arg.meta);
        auto obj = makeObj(constPool[arg.type]->toString(), type);
        return halloc<NamedRef>(manager, name, obj, meta);
    }

    vector<Obj *> Loader::readConstPool(const CpInfo *constantPool, uint16 count) {
        vector<Obj *> list;
        list.reserve(count);
        for (int i = 0; i < count; ++i) {
            list.push_back(readCp(constantPool[i]));
        }
        return list;
    }

    Obj *Loader::readCp(const CpInfo &cpInfo) {
        switch (cpInfo.tag) {
            case 0x00:
                return halloc<ObjNull>(manager, getCurrentModule());
            case 0x01:
                return halloc<ObjBool>(manager, true, getCurrentModule());
            case 0x02:
                return halloc<ObjBool>(manager, false, getCurrentModule());
            case 0x03:
                return halloc<ObjChar>(manager, static_cast<char>(cpInfo._char), getCurrentModule());
            case 0x04:
                return halloc<ObjInt>(manager, unsigned_to_signed(cpInfo._int), getCurrentModule());
            case 0x05:
                return halloc<ObjFloat>(manager, raw_to_double(cpInfo._float), getCurrentModule());
            case 0x06:
                return halloc<ObjString>(manager, cpInfo._string.bytes, cpInfo._string.len, getCurrentModule());
            case 0x07: {
                auto con = cpInfo._array;
                auto array = halloc<ObjArray>(manager, con.len, getCurrentModule());
                for (int i = 0; i < con.len; ++i) {
                    array->set(i, readCp(con.items[i]));
                }
                return array;
            }
            default:
                throw Unreachable();
        }
    }

    string Loader::readUTF8(const _UTF8 &value) {
        return string(reinterpret_cast<const char *>(value.bytes), value.len);
    }

    Table<string> Loader::readMeta(const MetaInfo &meta) {
        Table<string> table;
        for (int i = 0; i < meta.len; ++i) {
            auto entry = meta.table[i];
            table[readUTF8(entry.key)] = readUTF8(entry.value);
        }
        return table;
    }

    Type *Loader::findType(const string &sign) {
        if (vm->getSettings().inbuiltTypes.contains(sign))
            return null;

        Type *type;

        if (auto find1 = vm->getSymbol(sign); is<Type>(find1)) {    // Try to find in vm globals
            // Get it
            type = cast<Type>(find1);
        } else if (auto find2 = referencePool.find(sign);
                   find2 != referencePool.end()) {    // Try to find the type if it is already present in the ref pool
            type = find2->second;
        } else {    // Build an unresolved type in the ref pool and return it
            // Get a sentinel with the name attached to it
            type = Type::SENTINEL_(sign, manager);
            // Put the type
            referencePool[sign] = type;
        }
        return type;
    }

    Type *Loader::resolveType(const string &sign, const Type &type) {
        // Get the object
        auto find = referencePool.find(sign);
        // If it was referred earlier, then resolve it
        if (find != referencePool.end()) {
            // Get the sentinel
            auto unresolved = find->second;
            // Change it
            *unresolved = type;
            return unresolved;
        }
        return halloc<Type>(manager, type);
    }

    Obj *Loader::makeObj(const string &typeSign, const Sign &objSign, Type *type) const {
        static std::unordered_map<string, function<Obj *()>> objMap = {
                {"array",  [&] { return halloc<ObjArray>(manager, 0, getCurrentModule()); }   },
                {"bool",   [&] { return halloc<ObjBool>(manager, false, getCurrentModule()); }},
                {"char",   [&] { return halloc<ObjChar>(manager, '\0', getCurrentModule()); } },
                {"float",  [&] { return halloc<ObjFloat>(manager, 0, getCurrentModule()); }   },
                {"int",    [&] { return halloc<ObjInt>(manager, 0, getCurrentModule()); }     },
                {"string", [&] { return halloc<ObjString>(manager, "", getCurrentModule()); } }
        };
        try {
            return objMap.at(typeSign)();
        } catch (const std::out_of_range &) {
            return halloc<Obj>(manager, objSign, type, getCurrentModule());
        }
    }

    Obj *Loader::makeObj(const string &typeSign, Type *type) const {
        static map<string, function<Obj *()>> objMap = {
                {".array",  [&] { return halloc<ObjArray>(manager, 0, getCurrentModule()); }   },
                {".bool",   [&] { return halloc<ObjBool>(manager, false, getCurrentModule()); }},
                {".char",   [&] { return halloc<ObjChar>(manager, '\0', getCurrentModule()); } },
                {".float",  [&] { return halloc<ObjFloat>(manager, 0, getCurrentModule()); }   },
                {".int",    [&] { return halloc<ObjInt>(manager, 0, getCurrentModule()); }     },
                {".string", [&] { return halloc<ObjString>(manager, "", getCurrentModule()); } }
        };
        try {
            return objMap.at(typeSign)();
        } catch (const std::out_of_range &) {
            return halloc<Obj>(manager, Sign(""), type, getCurrentModule());
        }
    }
}    // namespace spade
