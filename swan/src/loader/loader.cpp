#include "loader.hpp"
#include "ee/vm.hpp"
#include "callable/table.hpp"
#include "ee/obj.hpp"
#include "elpops/elpdef.hpp"
#include "elpops/reader.hpp"
#include "memory/memory.hpp"
#include "spimp/utils.hpp"
#include "verifier.hpp"
#include <cstddef>
#include <spdlog/spdlog.h>

namespace spade
{
    Loader::Loader(SpadeVM *vm) : vm(vm), obj_null(halloc_mgr<ObjNull>(vm->get_memory_manager())) {}

    LoadResult Loader::load(const fs::path &path) {
        // Read the file
        ElpReader reader(resolve_path("", path));
        ElpInfo elp_info = reader.read();
        spdlog::info("Loader: Read file '{}'", reader.get_path());
        // Verify the file
        Verifier verifier(elp_info, path.generic_string());
        verifier.verify();
        spdlog::info("Loader: Verified file '{}'", reader.get_path());
        // Load the file
        std::vector<fs::path> imports;
        string entry = load_elp(elp_info, path, imports);
        // Load the imports
        for (size_t i = 0; i < imports.size(); i++) {
            ElpReader reader(imports[i]);
            ElpInfo elp_info = reader.read();
            spdlog::info("Loader: Read import file '{}'", reader.get_path());
            Verifier verifier(elp_info, path.generic_string());
            verifier.verify();
            spdlog::info("Loader: Verified file '{}'", reader.get_path());
            load_elp(elp_info, path, imports);
        }
        // Find the module inits
        vector<ObjMethod *> inits;
        for (const auto &sign: module_init_signs) {
            inits.push_back(cast<ObjMethod>(vm->get_symbol(sign.to_string())));
        }
        module_init_signs.clear();
        // Return the result
        return LoadResult{
                .entry = entry.empty() ? null : cast<ObjMethod>(vm->get_symbol(entry)),
                .inits = inits,
        };
    }

    void Loader::start_scope(Obj *scope) {
        scope_stack.push_back(scope);
    }

    Obj *Loader::get_scope() const {
        return scope_stack.empty() ? null : scope_stack.back();
    }

    void Loader::end_scope() {
        scope_stack.pop_back();
    }

    void Loader::start_sign_scope(const string &name) {
        if (sign_stack.empty()) {
            sign_stack.push_back(Sign(name));
        } else {
            sign_stack.push_back(sign_stack.back() | name);
        }
    }

    const Sign &Loader::get_sign() const {
        return sign_stack.back();
    }

    void Loader::end_sign_scope() {
        if (!sign_stack.empty())
            sign_stack.pop_back();
    }

    void Loader::start_conpool_scope(const vector<Obj *> &conpool) {
        conpool_stack.push_back(conpool);
    }

    const vector<Obj *> &Loader::get_conpool() const {
        return conpool_stack.back();
    }

    void Loader::end_conpool_scope() {
        conpool_stack.pop_back();
    }

    fs::path Loader::resolve_path(const fs::path &from_path, const fs::path &path) {
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

    string Loader::load_elp(const ElpInfo &info, const fs::path &path, std::vector<fs::path> &imports) {
        // TODO: version checking will be enabled later
        string entry = load_utf8(info.entry);
        // Find imports
        for (const auto &import: info.imports) {
            imports.push_back(resolve_path(path, load_utf8(import)));
        }
        // Load modules
        for (const auto &module: info.modules) {
            load_module(module);
        }
        return entry;
    }

    void Loader::load_module(const ModuleInfo &info) {
        start_conpool_scope(load_const_pool(info.constant_pool));

        // INFO: check info.kind
        fs::path compiled_from = get_conpool()[info.compiled_from]->to_string();
        string name = get_conpool()[info.name]->to_string();
        // Set init
        string init = get_conpool()[info.init]->to_string();
        if (!init.empty())
            module_init_signs.push_back(init);

        start_sign_scope(name);

        auto module = halloc_mgr<ObjModule>(vm->get_memory_manager(), get_sign());
        module->set_path(compiled_from);
        module->set_sign(get_sign());
        module->set_constant_pool(get_conpool());

        start_scope(module);

        for (const auto &info: info.globals) {
            // INFO: check info.kind
            // INFO: check info.access_flags
            string name = get_conpool()[info.name]->to_string();
            // Set metadata
            vm->set_metadata((get_sign() | name).to_string(), load_meta(info.meta));
            // Set the global in the scope
            assert(get_scope()->get_tag() == ObjTag::MODULE);
            get_scope()->set_member(name, obj_null);
        }

        for (const auto &method: info.methods) {
            load_method(method);
        }

        for (const auto &klass: info.classes) {
            load_class(klass);
        }

        for (const auto &module: info.modules) {
            load_module(module);
        }

        end_scope();
        end_sign_scope();
        end_conpool_scope();

        if (const auto obj = get_scope()) {
            obj->set_member(name, module);
        } else {
            vm->set_symbol(name, module);
        }

        spdlog::info("Loader: Loaded module: {}", module->get_sign().to_string());
    }

    void Loader::load_method(const MethodInfo &info) {
        // Set kind
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
        // INFO: check info.access_flags
        string name = get_conpool()[info.name]->to_string();
        Sign sign = get_sign() | name;

        // TODO: check info.type_params
        Table<Type *> type_params;
        // Set args
        VariableTable args(info.args_count);
        for (size_t i = 0; const auto &arg: info.args) {
            args.set(i, obj_null);
            args.set_meta(i, load_meta(info.meta));
            i++;
        }
        // Set locals
        VariableTable locals(info.locals_count);
        for (size_t i = 0; const auto &local: info.locals) {
            locals.set(i, obj_null);
            locals.set_meta(i, load_meta(info.meta));
            i++;
        }
        // Set exception table
        ExceptionTable exceptions;
        for (const auto &ex: info.exception_table) {
            // TODO: perform type resolution
            Exception exception(ex.start_pc, ex.end_pc, ex.target_pc, null, load_meta(ex.meta));
            exceptions.add_exception(exception);
        }
        // Set line number info
        LineNumberTable lines;
        for (const auto &number: info.line_info.numbers) lines.add_line(number.times, number.lineno);
        // Set matches
        vector<MatchTable> matches;
        for (const auto &info: info.matches) {
            vector<Case> cases;
            for (const auto &info: info.cases) {
                cases.emplace_back(get_conpool()[info.value], info.location);
            }
            matches.emplace_back(cases, info.default_location);
        }
        // Set metadata
        vm->set_metadata(sign.to_string(), load_meta(info.meta));
        // Set frame template
        FrameTemplate frame(info.code, info.stack_max, args, locals, exceptions, lines, matches);
        ObjMethod *method = halloc_mgr<ObjMethod>(vm->get_memory_manager(), kind, sign, frame, type_params);
        // Set the method in the scope
        assert(get_scope()->get_tag() == ObjTag::MODULE || get_scope()->get_tag() == ObjTag::TYPE);
        get_scope()->set_member(name, method);

        spdlog::info("Loader: Loaded method: {}", method->get_sign().to_string());
    }

    void Loader::load_class(const ClassInfo &info) {
        // Set kind
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
        // INFO: check info.access_flags
        string name = get_conpool()[info.name]->to_string();
        start_sign_scope(name);

        // Set supers
        vector<Sign> supers;
        cast<ObjArray>(get_conpool()[info.supers])->for_each([&](Obj *super) {    //
            supers.emplace_back(super->to_string());                              //
        });
        // TODO: check info.type_params
        Table<Type *> type_params;

        auto type = halloc_mgr<Type>(vm->get_memory_manager(), kind, get_sign(), type_params, supers);
        start_scope(type);

        // Set fields
        for (const auto &info: info.fields) {
            // INFO: check info.kind
            // INFO: check info.access_flags
            string name = get_conpool()[info.name]->to_string();
            // Set metadata
            vm->set_metadata((get_sign() | name).to_string(), load_meta(info.meta));
            // Set the global in the scope
            assert(get_scope()->get_tag() == ObjTag::TYPE);
            get_scope()->set_member(name, obj_null);
        }
        // Set methods
        for (const auto &method: info.methods) {
            load_method(method);
        }
        // Set metadata
        vm->set_metadata(get_sign().to_string(), load_meta(info.meta));

        end_scope();
        end_sign_scope();

        assert(get_scope()->get_tag() == ObjTag::MODULE || get_scope()->get_tag() == ObjTag::TYPE);
        get_scope()->set_member(name, type);

        spdlog::info("Loader: Loaded type: {}", type->get_sign().to_string());
    }

    vector<Obj *> Loader::load_const_pool(const vector<CpInfo> &cps) {
        vector<Obj *> pool;
        for (const auto &cp: cps) {
            pool.push_back(load_cp(cp));
        }
        spdlog::info("Loader: Loaded constant pool");
        return pool;
    }

    Table<string> Loader::load_meta(const MetaInfo &meta) {
        Table<string> table;
        for (int i = 0; i < meta.len; ++i) {
            auto entry = meta.table[i];
            table[load_utf8(entry.key)] = load_utf8(entry.value);
        }
        return table;
    }

    Obj *Loader::load_cp(const CpInfo &cp) {
        const auto mgr = vm->get_memory_manager();
        switch (cp.tag) {
        case 0x00:
            return obj_null;
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
            for (size_t i = 0; i < con.len; ++i) {
                array->set(i, load_cp(con.items[i]));
            }
            return array;
        }
        default:
            throw Unreachable();
        }
        return null;
    }

    string Loader::load_utf8(const _UTF8 &info) {
        return string(info.bytes.begin(), info.bytes.end());
    }
}    // namespace spade
