#include "loader.hpp"
#include "ee/vm.hpp"
#include "objects/float.hpp"
#include "objects/int.hpp"
#include "objects/typeparam.hpp"
#include "verifier.hpp"

namespace spade
{
    Loader::Loader(SpadeVM *vm) : vm(vm), manager(vm->get_memory_manager()) {}

    ObjMethod *Loader::load(const string &path) {
        vector<ObjModule *> to_be_loaded;
        {    // Read

            struct ModInfo {
                vector<string> deps;
                uint32 i;
            };

            vector<ModInfo> module_stack;
            ObjModule *module = read_module(path);
            ModInfo mod_info = {.deps = module->get_dependencies(), .i = 0};
            module_stack.push_back(mod_info);
            to_be_loaded.push_back(module);
            while (!module_stack.empty()) {
                mod_info = module_stack.back();
                if (mod_info.i >= mod_info.deps.size()) {
                    module_stack.pop_back();
                }
                for (; mod_info.i < mod_info.deps.size(); ++mod_info.i) {
                    string dep_path = mod_info.deps[mod_info.i];
                    auto dep_module = read_module(dep_path);
                    if (dep_module->get_state() == ObjModule::State::NOT_READ) {
                        ModInfo dep_mod_info = {.deps = module->get_dependencies(), .i = 0};
                        module_stack.push_back(dep_mod_info);
                        to_be_loaded.push_back(dep_module);
                    }
                }
            }
        }
        {    // Load
            for (auto module: to_be_loaded) {
                current = module;
                load_module(module);
            }
            current = null;
            // Complain about the unresolved types
            for (const auto &[sign, reference]: reference_pool) {
                if (reference->get_kind() == Type::Kind::UNRESOLVED) {
                    throw IllegalAccessError(std::format("reference not found: '{}'", sign));
                    // TODO give a detailed error message for all the unresolved references
                }
            }
        }
        {    // Init
            for (auto module: to_be_loaded) {
                ObjMethod *init = module->get_init();
                if (init != null) {
                    init->invoke({});
                    module->set_state(ObjModule::State::INITIALIZED);
                }
            }
        }

        auto module = to_be_loaded[0];
        auto elp = module->get_elp();
        // If this is an executable one, get the entry point
        if (elp.type == 0x01) {
            // Load the constant pool
            vector<Obj *> const_pool = module->get_constant_pool();
            // get the param of the entry point
            auto entry_sign = const_pool[elp.entry]->to_string();
            // Return the entry point
            return cast<ObjMethod>(vm->get_symbol(entry_sign));
        }
        return null;
    }

    string Loader::resolve_path(const string &path_str) {
        fs::path path{path_str};
        fs::path result;

        if (path.is_absolute())
            result = path;
        else if (path.string()[0] == '.') {
            result = (get_load_path() / path);
        } else {
            for (const auto &dir: vm->get_settings().mod_path) {
                result = dir / path;
                if (exists(result))
                    return result.string();
            }
            result = get_load_path() / path;
            if (exists(result))
                return result.string();
            result = fs::current_path() / path;
            if (exists(result))
                return result.string();
        }
        if (!exists(result)) {
            throw IllegalAccessError(std::format("path not found: {}", path_str));
        }
        return result.string();
    }

    fs::path Loader::get_load_path() {
        if (get_current_module() != null) {
            return get_current_module()->get_path().parent_path();
        }
        return fs::current_path();
    }

    ObjModule *Loader::read_module(const string &path) {
        auto absolute_path = get_absolute_path(path);
        // If the module is already read, return it
        if (auto it = modules.find(absolute_path); it != modules.end()) {
            return it->second;
        }
        // Or else, start reading a new module
        ElpReader reader{resolve_path(path)};
        auto elp = reader.read();
        // Verify it...
        Verifier verifier{elp, path};
        verifier.verify();
        // Construct the module
        auto const_pool = read_const_pool(elp.constant_pool, elp.constant_pool_count);
        auto sign = Sign{const_pool[elp.this_module]->to_string()};
        auto dep_objs = cast<ObjArray>(const_pool[elp.imports]);
        vector<string> deps;
        deps.reserve(dep_objs->count());
        dep_objs->foreach ([&deps](auto dep_obj) { deps.push_back(dep_obj->to_string()); });
        vm->set_metadata(sign.to_string(), read_meta(elp.meta));

        auto module = halloc<ObjModule>(manager, sign, path, const_pool, deps, elp);
        module->set_state(ObjModule::State::READ);
        // Insert the module to the module table
        modules[absolute_path] = module;
        return module;
    }

    void Loader::load_module(ObjModule *module) {
        auto elp = module->get_elp();
        // Load the objects
        for (int i = 0; i < elp.objects_count; ++i) {
            auto obj = read_obj(elp.objects[i]);
            module->set_member(obj->get_sign().get_name(), obj);
        }
        // Load the globals
        for (int i = 0; i < elp.globals_count; ++i) {
            auto global = read_global(elp.globals[i]);
            module->set_member(global->get_sign().get_name(), global);
        }
        // add the module to the globals
        vm->get_modules()[module->get_sign().get_name()] = module;
        // set the state to loaded
        module->set_state(ObjModule::State::LOADED);
        // Try to get init point
        auto init_sign = get_constant_pool()[elp.init]->to_string();
        if (!init_sign.empty()) {
            module->set_init(cast<ObjMethod>(vm->get_symbol(init_sign)));
        } else {
            module->set_init(null);
        }
    }

    Obj *Loader::read_global(GlobalInfo &global) {
        auto const_pool = get_constant_pool();
        auto sign = get_sign(global.this_global);
        auto type = find_type(const_pool[global.type]->to_string());

        auto meta = read_meta(global.meta);
        vm->set_metadata(sign.to_string(), meta);

        return make_obj(const_pool[global.type]->to_string(), sign, type);
    }

    Obj *Loader::read_obj(ObjInfo &obj) {
        auto const_pool = get_constant_pool();
        switch (obj.type) {
            case 0x01:
                return read_method(obj._method);
            case 0x02:
                return read_class(obj._class);
            default:
                throw Unreachable();
        }
    }

    Obj *Loader::read_class(ClassInfo &klass) {
        auto const_pool = get_constant_pool();
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
        auto sign = get_sign(klass.this_class);

        Table<NamedRef *> type_params;
        for (int i = 0; i < klass.type_param_count; ++i) {
            auto param_name = const_pool[klass.type_params[i].name]->to_string();
            auto type_param =
                    halloc<NamedRef>(manager, param_name, halloc<TypeParam>(manager, Sign{param_name}, get_current_module()), Table<string>{});
            type_params[param_name] = type_param;
            reference_pool[param_name] = cast<Type>(type_param->get_value());
        }

        Table<Type *> supers;
        cast<ObjArray>(const_pool[klass.supers])->foreach ([this, &supers](auto super) {
            auto str = super->to_string();
            Type *type = find_type(str);
            supers[type->get_sign().to_string()] = type;
        });

        Table<MemberSlot> members;
        for (int i = 0; i < klass.fields_count; ++i) {
            auto field = read_field(klass.fields[i]);
            members[field->get_sign().get_name()] = MemberSlot{field, Flags{klass.fields[i].flags}};
        }
        for (int i = 0; i < klass.methods_count; ++i) {
            auto method = read_method(sign.to_string(), klass.methods[i]);
            members[method->get_sign().get_name()] = MemberSlot{method};
        }
        for (int i = 0; i < klass.objects_count; ++i) {
            auto object = read_obj(klass.objects[i]);
            members[object->get_sign().get_name()] = MemberSlot{object};
        }

        auto meta = read_meta(klass.meta);
        vm->set_metadata(sign.to_string(), meta);

        // Resolve the type params
        for (const auto &[name, _]: type_params) {
            // Remove the unresolved types from ref pool
            reference_pool.erase(name);
        }
        // Resolve the type
        auto type = resolve_type(sign.to_string(), {sign, kind, type_params, supers, members, get_current_module()});
        vm->set_symbol(sign.to_string(), type);
        return type;
    }

    Obj *Loader::read_field(FieldInfo &field) {
        auto const_pool = get_constant_pool();
        auto sign = get_sign(field.this_field);
        auto type = find_type(const_pool[field.type]->to_string());

        auto meta = read_meta(field.meta);
        vm->set_metadata(sign.to_string(), meta);

        return make_obj(const_pool[field.type]->to_string(), sign, type);
    }

    Obj *Loader::read_method(const string &klass_sign, MethodInfo &method) {
        auto const_pool = get_constant_pool();
        auto type = find_type(klass_sign);
        auto met = read_method(method);
        met->set_type(type);
        return met;
    }

    Obj *Loader::read_method(MethodInfo &method) {
        auto const_pool = get_constant_pool();
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
        auto sign = get_sign(method.this_method);
        Table<NamedRef *> type_params;
        for (int i = 0; i < method.type_param_count; ++i) {
            auto param_name = const_pool[method.type_params[i].name]->to_string();
            auto type_param =
                    halloc<NamedRef>(manager, param_name, halloc<TypeParam>(manager, Sign{param_name}, get_current_module()), Table<string>{});
            type_params[param_name] = type_param;
            reference_pool[param_name] = cast<Type>(type_param->get_value());
        }
        ArgsTable args{};
        for (int i = 0; i < method.args_count; ++i) {
            args.add_arg(read_arg(method.args[i]));
        }
        LocalsTable locals{method.closure_start};
        for (int i = 0; i < method.locals_count; ++i) {
            locals.add_local(read_local(method.locals[i]));
        }
        ExceptionTable exceptions{};
        for (int i = 0; i < method.exception_table_count; ++i) {
            exceptions.add_exception(read_exception(method.exception_table[i]));
        }
        LineNumberTable lines{};
        for (int i = 0; i < method.line_info.number_count; ++i) {
            auto number_info = method.line_info.numbers[i];
            lines.add_line(number_info.times, number_info.lineno);
        }
        vector<ObjMethod *> lambdas;
        lambdas.reserve(method.lambda_count);
        for (int i = 0; i < method.lambda_count; ++i) {
            lambdas.push_back(cast<ObjMethod>(read_method(method.lambdas[i])));
        }
        vector<MatchTable> matches;
        matches.reserve(method.match_count);
        for (int i = 0; i < method.match_count; ++i) {
            matches.push_back(read_match(method.matches[i]));
        }

        auto meta = read_meta(method.meta);
        vm->set_metadata(sign.to_string(), meta);

        // Resolve the type params
        for (const auto &[name, _]: type_params) {
            // Remove the unresolved types from ref pool
            reference_pool.erase(name);
        }
        // Create the frame template
        auto frame_template = new FrameTemplate{method.code_count, method.code, method.max_stack, args, locals, exceptions, lines, lambdas, matches};
        auto method_obj = halloc<ObjMethod>(manager, sign, kind, frame_template, null, type_params, get_current_module());
        return method_obj;
    }

    MatchTable Loader::read_match(MethodInfo::MatchInfo match) {
        auto const_pool = get_constant_pool();
        vector<Case> cases;
        cases.reserve(match.case_count);
        for (int i = 0; i < match.case_count; ++i) {
            auto kase = match.cases[i];
            cases.emplace_back(Obj::create_copy(const_pool[kase.value]), kase.location);
        }
        return {cases, match.default_location};
    }

    Exception Loader::read_exception(MethodInfo::ExceptionTableInfo &exception) {
        auto const_pool = get_constant_pool();
        auto type = find_type(const_pool[exception.exception]->to_string());
        return {exception.start_pc, exception.end_pc, exception.target_pc, type, read_meta(exception.meta)};
    }

    NamedRef *Loader::read_local(MethodInfo::LocalInfo &local) {
        auto const_pool = get_constant_pool();
        auto name = const_pool[local.this_local]->to_string();
        auto type = find_type(const_pool[local.type]->to_string());
        auto meta = read_meta(local.meta);
        auto obj = make_obj(const_pool[local.type]->to_string(), type);
        return halloc<NamedRef>(manager, name, obj, meta);
    }

    NamedRef *Loader::read_arg(MethodInfo::ArgInfo &arg) {
        auto const_pool = get_constant_pool();
        auto name = const_pool[arg.this_arg]->to_string();
        auto type = find_type(const_pool[arg.type]->to_string());
        auto meta = read_meta(arg.meta);
        auto obj = make_obj(const_pool[arg.type]->to_string(), type);
        return halloc<NamedRef>(manager, name, obj, meta);
    }

    vector<Obj *> Loader::read_const_pool(const CpInfo *constant_pool, uint16 count) {
        vector<Obj *> list;
        list.reserve(count);
        for (int i = 0; i < count; ++i) {
            list.push_back(read_cp(constant_pool[i]));
        }
        return list;
    }

    Obj *Loader::read_cp(const CpInfo &cp_info) {
        switch (cp_info.tag) {
            case 0x00:
                return halloc<ObjNull>(manager, get_current_module());
            case 0x01:
                return halloc<ObjBool>(manager, true, get_current_module());
            case 0x02:
                return halloc<ObjBool>(manager, false, get_current_module());
            case 0x03:
                return halloc<ObjChar>(manager, static_cast<char>(cp_info._char), get_current_module());
            case 0x04:
                return halloc<ObjInt>(manager, unsigned_to_signed(cp_info._int), get_current_module());
            case 0x05:
                return halloc<ObjFloat>(manager, raw_to_double(cp_info._float), get_current_module());
            case 0x06:
                return halloc<ObjString>(manager, cp_info._string.bytes, cp_info._string.len, get_current_module());
            case 0x07: {
                auto con = cp_info._array;
                auto array = halloc<ObjArray>(manager, con.len, get_current_module());
                for (int i = 0; i < con.len; ++i) {
                    array->set(i, read_cp(con.items[i]));
                }
                return array;
            }
            default:
                throw Unreachable();
        }
    }

    string Loader::read_utf8(const _UTF8 &value) {
        return string(reinterpret_cast<const char *>(value.bytes), value.len);
    }

    Table<string> Loader::read_meta(const MetaInfo &meta) {
        Table<string> table;
        for (int i = 0; i < meta.len; ++i) {
            auto entry = meta.table[i];
            table[read_utf8(entry.key)] = read_utf8(entry.value);
        }
        return table;
    }

    Type *Loader::find_type(const string &sign) {
        if (vm->get_settings().inbuilt_types.contains(sign))
            return null;

        Type *type;

        if (auto find1 = vm->get_symbol(sign); is<Type>(find1)) {    // Try to find in vm globals
            // get it
            type = cast<Type>(find1);
        } else if (auto find2 = reference_pool.find(sign);
                   find2 != reference_pool.end()) {    // Try to find the type if it is already present in the ref pool
            type = find2->second;
        } else {    // Build an unresolved type in the ref pool and return it
            // get a sentinel with the name attached to it
            type = Type::SENTINEL_(sign, manager);
            // Put the type
            reference_pool[sign] = type;
        }
        return type;
    }

    Type *Loader::resolve_type(const string &sign, const Type &type) {
        // get the object
        auto find = reference_pool.find(sign);
        // If it was referred earlier, then resolve it
        if (find != reference_pool.end()) {
            // get the sentinel
            auto unresolved = find->second;
            // Change it
            *unresolved = type;
            return unresolved;
        }
        return halloc<Type>(manager, type);
    }

    Obj *Loader::make_obj(const string &type_sign, const Sign &obj_sign, Type *type) const {
        static std::unordered_map<string, std::function<Obj *()>> obj_map = {
                {"array",  [&] { return halloc<ObjArray>(manager, 0, get_current_module()); }   },
                {"bool",   [&] { return halloc<ObjBool>(manager, false, get_current_module()); }},
                {"char",   [&] { return halloc<ObjChar>(manager, '\0', get_current_module()); } },
                {"float",  [&] { return halloc<ObjFloat>(manager, 0, get_current_module()); }   },
                {"int",    [&] { return halloc<ObjInt>(manager, 0, get_current_module()); }     },
                {"string", [&] { return halloc<ObjString>(manager, "", get_current_module()); } }
        };
        try {
            return obj_map.at(type_sign)();
        } catch (const std::out_of_range &) {
            return halloc<Obj>(manager, obj_sign, type, get_current_module());
        }
    }

    Obj *Loader::make_obj(const string &type_sign, Type *type) const {
        static std::unordered_map<string, std::function<Obj *()>> obj_map = {
                {".array",  [&] { return halloc<ObjArray>(manager, 0, get_current_module()); }   },
                {".bool",   [&] { return halloc<ObjBool>(manager, false, get_current_module()); }},
                {".char",   [&] { return halloc<ObjChar>(manager, '\0', get_current_module()); } },
                {".float",  [&] { return halloc<ObjFloat>(manager, 0, get_current_module()); }   },
                {".int",    [&] { return halloc<ObjInt>(manager, 0, get_current_module()); }     },
                {".string", [&] { return halloc<ObjString>(manager, "", get_current_module()); } }
        };
        try {
            return obj_map.at(type_sign)();
        } catch (const std::out_of_range &) {
            return halloc<Obj>(manager, Sign(""), type, get_current_module());
        }
    }
}    // namespace spade
