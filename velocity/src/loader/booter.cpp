#include <algorithm>

#include "booter.hpp"
#include "callable/frame_template.hpp"
#include "callable/method.hpp"
#include "callable/table.hpp"
#include "memory/manager.hpp"
#include "memory/memory.hpp"
#include "objects/float.hpp"
#include "objects/inbuilt_types.hpp"
#include "ee/vm.hpp"
#include "objects/int.hpp"
#include "objects/module.hpp"
#include "objects/obj.hpp"
#include "objects/type.hpp"
#include "objects/typeparam.hpp"
#include "verifier.hpp"

#define load_sign(index) (current_sign() | conpool[index]->to_string())

namespace spade
{
    ObjMethod *Booter::load(const fs::path &path) {
        const auto &[ctx, modules] = _load(path);
        // Execute the post processing callbacks
        for (const auto &callback: post_callbacks) callback();
        // Complain about the unresolved types
        for (const auto &[sign, reference]: unresolved) {
            // TODO: give a detailed error message for all the unresolved references
            throw IllegalAccessError(std::format("reference not found: '{}'", sign.to_string()));
        }
        // Initialize all modules
        for (const auto module: modules) {
            if (ObjMethod *init = module->get_init())
                init->invoke({});
        }
        if (!ctx.entry.empty())
            // Return the entry point
            return cast<ObjMethod>(vm->get_symbol(ctx.entry));
        return null;
    }

    std::pair<Booter::ElpContext, vector<ObjModule *>> Booter::_load(const fs::path &path) {
        auto file_path = fs::canonical(resolve_path("", path));
        if (const auto it = loaded_mods.find(file_path); it != loaded_mods.end())
            return {{}, it->second};
        if (const auto it = std::find(path_stack.begin(), path_stack.end(), file_path); it != path_stack.end())
            return {{}, {}};

        path_stack.push_back(file_path);
        ElpReader reader(file_path);
        const auto elp = reader.read();
        Verifier verifier(elp, file_path.string());
        verifier.verify();
        const auto ctx = load_elp(elp);

        vector<ObjModule *> modules;
        for (const auto import: ctx.imports) {
            auto imported_mods = _load(resolve_path(path, import)).second;
            modules.insert(modules.end(), imported_mods.begin(), imported_mods.end());
            extend_vec(modules, imported_mods);
        }
        for (const auto &module_info: elp.modules) {
            modules.push_back(load_module(module_info));
        }

        path_stack.pop_back();
        return {ctx, modules};
    }

    Booter::ElpContext Booter::load_elp(const ElpInfo &elp) {
        ElpContext ctx;
        // Load the constant pool
        const auto conpool = read_const_pool(elp.constant_pool);
        // Fetch the entry point signature
        if (elp.magic == 0xC0FFEEDE)
            ctx.entry = conpool[elp.entry]->to_string();
        // Fetch the import paths
        const auto imports_obj = cast<ObjArray>(conpool[elp.imports]);
        ctx.imports.resize(imports_obj->count());
        for (uint16 i = 0; i < imports_obj->count(); i++) {
            ctx.imports[i] = imports_obj->get(i)->to_string();
        }
        // Free the constants as they are no longer needed
        for (const auto constant: conpool) hfree(constant);
        return ctx;
    }

    ObjModule *Booter::load_module(const ModuleInfo &info) {
        const auto mgr = vm->get_memory_manager();

        const auto conpool = read_const_pool(info.constant_pool);
        const fs::path compiled_from = conpool[info.compiled_from]->to_string();
        const string name = conpool[info.name]->to_string();
        const Sign init = conpool[info.init]->to_string();

        const auto obj = halloc_mgr<ObjModule>(mgr, current_sign(), current_path(), conpool, Table<MemberSlot>{});

        const auto old_cur_mod = cur_mod;
        cur_mod = obj;

        Table<MemberSlot> member_slots;
        begin_scope(name);
        for (const auto &global: info.globals) {
            MemberSlot slot(load_global(global, conpool), Flags(global.access_flags));
            member_slots[slot.get_value()->get_sign().get_name()] = slot;
        }
        for (const auto &method: info.methods) {
            MemberSlot slot(load_method(method, conpool), Flags(method.access_flags));
            member_slots[slot.get_value()->get_sign().get_name()] = slot;
        }
        for (const auto &klass: info.classes) {
            MemberSlot slot(load_class(klass, conpool), Flags(klass.access_flags));
            member_slots[slot.get_value()->get_sign().get_name()] = slot;
        }
        cur_mod = old_cur_mod;
        obj->get_member_slots() = member_slots;
        vm->set_metadata(current_sign().to_string(), read_meta(info.meta));

        // Add the module to the vm
        vm->get_modules()[end_scope().get_name()] = obj;
        // Try to get init point
        if (const auto init_sign = conpool[info.init]->to_string(); !init_sign.empty())
            obj->set_init(cast<ObjMethod>(vm->get_symbol(init_sign)));
        return obj;
    }

    Obj *Booter::load_global(const GlobalInfo &info, const vector<Obj *> &conpool) {
        const Sign sign = load_sign(info.name);
        const Sign type_sign = conpool[info.type]->to_string();
        const auto type = find_type(type_sign);

        vm->set_metadata(sign.to_string(), read_meta(info.meta));
        return make_obj(type_sign, sign, type);
    }

    Obj *Booter::load_method(const MethodInfo &info, const vector<Obj *> &conpool) {
        const auto mgr = vm->get_memory_manager();

        ObjMethod::Kind kind;
        switch (info.kind) {
            case 0x00:
                kind = ObjMethod::Kind::FUNCTION;
                break;
            case 0x01:
                kind = ObjMethod::Kind::METHOD;
                break;
            case 0x02:
                kind = ObjMethod::Kind::CONSTRUCTOR;
                break;
            default:
                throw Unreachable();
        }
        const Sign sign = load_sign(info.name);
        Table<TypeParam *> type_params;
        for (int i = 0; i < info.type_params_count; ++i) {
            const auto name = "[" + conpool[info.type_params[i].name]->to_string() + "]";
            const auto type_param = halloc_mgr<TypeParam>(mgr, name, get_current_module());
            type_params[name] = type_param;
            reference_pool[name] = type_param;
        }

        begin_scope(sign.get_name());
        ArgsTable args;
        LocalsTable locals{info.closure_start};
        ExceptionTable exceptions;
        LineNumberTable lines;
        vector<MatchTable> matches;
        matches.reserve(info.match_count);
        for (const auto &arg: info.args) args.add_arg(load_arg(arg, conpool));
        for (const auto &local: info.locals) locals.add_local(load_local(local, conpool));
        for (const auto &exception: info.exception_table) exceptions.add_exception(load_exception(exception, conpool));
        for (const auto &number: info.line_info.numbers) lines.add_line(number.times, number.lineno);
        for (const auto &match: info.matches) matches.push_back(load_match(match, conpool));
        end_scope();
        vm->set_metadata(sign.to_string(), read_meta(info.meta));

        // Resolve the type params
        for (const auto &[name, _]: type_params)
            // Remove the unresolved types from ref pool
            reference_pool.erase(name);

        const FrameTemplate frame_template(info.code, info.stack_max, args, locals, exceptions, lines, matches);
        return halloc_mgr<ObjMethod>(mgr, sign, kind, frame_template, type_params, get_current_module());
    }

    NamedRef Booter::load_arg(const ArgInfo &arg, const vector<Obj *> &conpool) {
        const auto sign = load_sign(arg.name);
        const Sign type_sign = conpool[arg.type]->to_string();
        const auto type = find_type(type_sign);
        const auto meta = read_meta(arg.meta);
        const auto obj = make_obj(type_sign, sign, type);
        return NamedRef(sign.get_name(), obj, meta);
    }

    NamedRef Booter::load_local(const LocalInfo &local, const vector<Obj *> &conpool) {
        const auto sign = load_sign(local.name);
        const Sign type_sign = conpool[local.type]->to_string();
        const auto type = find_type(type_sign);
        const auto meta = read_meta(local.meta);
        const auto obj = make_obj(type_sign, sign, type);
        return NamedRef(sign.get_name(), obj, meta);
    }

    Exception Booter::load_exception(const ExceptionTableInfo &exception, const vector<Obj *> &conpool) {
        const auto type = find_type(conpool[exception.exception]->to_string());
        return Exception(exception.start_pc, exception.end_pc, exception.target_pc, type, read_meta(exception.meta));
    }

    MatchTable Booter::load_match(const MatchInfo match, const vector<Obj *> &conpool) {
        vector<Case> cases(match.case_count);
        for (int i = 0; i < match.case_count; ++i) {
            const auto kase = match.cases[i];
            cases[i] = Case(conpool[kase.value], kase.location);
        }
        return MatchTable(cases, match.default_location);
    }

    Obj *Booter::load_class(const ClassInfo &info, const vector<Obj *> &conpool) {
        const auto mgr = vm->get_memory_manager();

        Type::Kind kind;
        switch (info.kind) {
            case 0x00:
                kind = Type::Kind::CLASS;
                break;
            case 0x01:
                kind = Type::Kind::INTERFACE;
                break;
            case 0x02:
                kind = Type::Kind::ANNOTATION;
                break;
            case 0x03:
                kind = Type::Kind::ENUM;
                break;
            default:
                throw Unreachable();
        }
        const Sign sign = load_sign(info.name);
        Table<TypeParam *> type_params;
        for (int i = 0; i < info.type_params_count; ++i) {
            const auto name = "[" + conpool[info.type_params[i].name]->to_string() + "]";
            const auto type_param = halloc_mgr<TypeParam>(mgr, name, get_current_module());
            type_params[name] = type_param;
            reference_pool[name] = type_param;
        }
        Table<Type *> supers;
        cast<ObjArray>(conpool[info.supers])->foreach ([this, &supers](auto super) {
            Type *type = find_type(super->to_string());
            supers[type->get_sign().to_string()] = type;
        });

        begin_scope(sign.get_name());
        Table<MemberSlot> member_slots;
        for (const auto &field: info.fields) {
            MemberSlot slot(load_field(field, conpool), Flags(field.access_flags));
            member_slots[slot.get_value()->get_sign().get_name()] = slot;
        }
        for (const auto &method: info.methods) {
            MemberSlot slot(load_method(method, conpool), Flags(method.access_flags));
            member_slots[slot.get_value()->get_sign().get_name()] = slot;
        }
        end_scope();
        vm->set_metadata(sign.to_string(), read_meta(info.meta));

        // Resolve the type params
        for (const auto &[name, _]: type_params) {
            // Remove the unresolved types from ref pool
            reference_pool.erase(name);
        }
        // Resolve the type if it was unresolved
        if (const auto obj = resolve_type(sign)) {
            obj->set_kind(kind);
            obj->get_type_params() = type_params;
            obj->get_supers() = supers;
            obj->get_member_slots() = member_slots;
            return obj;
        } else {
            return halloc_mgr<Type>(mgr, sign, kind, type_params, supers, member_slots, get_current_module());
        }
    }

    Obj *Booter::load_field(const FieldInfo &info, const vector<Obj *> &conpool) {
        const Sign sign = load_sign(info.name);
        const auto type = find_type(conpool[info.type]->to_string());

        vm->set_metadata(sign.to_string(), read_meta(info.meta));
        return make_obj(conpool[info.type]->to_string(), sign, type);
    }

    vector<Obj *> Booter::read_const_pool(const vector<CpInfo> &constants) {
        vector<Obj *> list;
        list.reserve(constants.size());
        for (const auto &constant: constants) {
            list.push_back(read_cp(constant));
        }
        return list;
    }

    Obj *Booter::read_cp(const CpInfo &cp) {
        const auto mgr = vm->get_memory_manager();

        switch (cp.tag) {
            case 0x00:
                return halloc_mgr<ObjNull>(mgr);
            case 0x01:
                return halloc_mgr<ObjBool>(mgr, true);
            case 0x02:
                return halloc_mgr<ObjBool>(mgr, false);
            case 0x03:
                return halloc_mgr<ObjChar>(mgr, static_cast<char>(std::get<uint32_t>(cp.value)));
            case 0x04:
                return halloc_mgr<ObjInt>(mgr, unsigned_to_signed(std::get<uint64_t>(cp.value)));
            case 0x05:
                return halloc_mgr<ObjFloat>(mgr, raw_to_double(std::get<uint64_t>(cp.value)));
            case 0x06:
                return halloc_mgr<ObjString>(mgr, std::get<_UTF8>(cp.value).bytes.data(), std::get<_UTF8>(cp.value).len);
            case 0x07: {
                const auto con = std::get<_Container>(cp.value);
                const auto array = halloc_mgr<ObjArray>(mgr, con.len);
                for (int i = 0; i < con.len; ++i) {
                    array->set(i, read_cp(con.items[i]));
                }
                return array;
            }
            default:
                throw Unreachable();
        }
    }

    string Booter::read_utf8(const _UTF8 &value) {
        return string(value.bytes.begin(), value.bytes.end());
    }

    Table<string> Booter::read_meta(const MetaInfo &meta) {
        Table<string> table;
        for (int i = 0; i < meta.len; ++i) {
            auto entry = meta.table[i];
            table[read_utf8(entry.key)] = read_utf8(entry.value);
        }
        return table;
    }

    Type *Booter::find_type(const Sign &sign) {
        const auto mgr = vm->get_memory_manager();
        const auto sign_str = sign.to_string();

        if (vm->get_settings().inbuilt_types.contains(sign_str))
            return null;

        if (const auto symbol = vm->get_symbol(sign_str, false); symbol && is<Type>(symbol)) {
            // Try to find in vm globals
            return cast<Type>(symbol);
        } else if (const auto it = reference_pool.find(sign); it != reference_pool.end()) {
            // Try to find the type if it is already present in the ref pool
            return it->second;
        } else if (const auto it = unresolved.find(sign); it != unresolved.end()) {
            // Try to find the type if it is already present in the unresolved types
            return it->second;
        } else {
            // Build an unresolved type in the unresolved and return it
            const auto type = Type::UNRESOLVED(sign, get_current_module(), mgr);
            unresolved[sign] = type;
            return type;
        }
    }

    Type *Booter::resolve_type(const Sign &sign) {
        if (const auto it = unresolved.find(sign); it != unresolved.end()) {
            const auto type = it->second;
            unresolved.erase(it);
            return type;
        }
        return null;
    }

    Obj *Booter::make_obj(const Sign &type_sign, const Sign &obj_sign, Type *type) {
        const auto mgr = vm->get_memory_manager();
        static std::unordered_map<Sign, std::function<Obj *(MemoryManager *const)>> obj_map = {
                {Sign("basic.array"),  [this](MemoryManager *const mgr) { return halloc_mgr<ObjArray>(mgr, 0, get_current_module()); }   },
                {Sign("basic.bool"),   [this](MemoryManager *const mgr) { return halloc_mgr<ObjBool>(mgr, false, get_current_module()); }},
                {Sign("basic.char"),   [this](MemoryManager *const mgr) { return halloc_mgr<ObjChar>(mgr, '\0', get_current_module()); } },
                {Sign("basic.float"),  [this](MemoryManager *const mgr) { return halloc_mgr<ObjFloat>(mgr, 0, get_current_module()); }   },
                {Sign("basic.int"),    [this](MemoryManager *const mgr) { return halloc_mgr<ObjInt>(mgr, 0, get_current_module()); }     },
                {Sign("basic.string"), [this](MemoryManager *const mgr) { return halloc_mgr<ObjString>(mgr, "", get_current_module()); } }
        };
        if (const auto it = obj_map.find(type_sign); it != obj_map.end())
            return it->second(mgr);
        else {
            if (const auto tp = dynamic_cast<TypeParam *>(type))
                return halloc_mgr<Obj>(mgr, obj_sign, tp, get_current_module());
            if (type && type->get_kind() == Type::Kind::UNRESOLVED) {
                const auto obj = halloc_mgr<Obj>(mgr, obj_sign, type, get_current_module());
                post_callbacks.push_back([obj, type] { obj->set_type(type); });
                return obj;
            }
            return halloc_mgr<Obj>(mgr, obj_sign, type, get_current_module());
        }
    }

    fs::path Booter::resolve_path(const fs::path &from_path, const fs::path &path) {
        if (path.is_absolute())
            return path;
        fs::path result;
        if (path.string()[0] == '.') {
            result = from_path.empty() ? fs::current_path() / path    //
                                       : from_path / path;
        } else {
            result = from_path / path;
            if (exists(result))
                return result.string();
            result = fs::current_path() / path;
            if (exists(result))
                return result.string();
            for (const auto &dir: vm->get_settings().mod_path) {
                result = dir / path;
                if (exists(result))
                    return result.string();
            }
        }
        return fs::exists(result) ? result : "";
    }
}    // namespace spade