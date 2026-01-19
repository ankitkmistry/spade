#include "emitter.hpp"
#include "elpops/elpdef.hpp"

namespace spadec
{

    MethodEmitter::MethodEmitter(ModuleEmitter &module, const string &name, Kind kind, Flags modifiers, uint8_t args_count, uint16_t locals_count)
        : module(&module) {
        // info.kind
        switch (kind) {
        case Kind::FUNCTION:
            info.kind = 0;
            break;
        case Kind::METHOD:
            info.kind = 1;
            break;
        case Kind::CONSTRUCTOR:
            info.kind = 2;
            break;
        }
        // info.access_flags
        info.access_flags = modifiers.get_raw();
        // info.name
        info.name = module.get_constant(name);
        // info.args_count
        info.args_count = args_count;
        // info.locals_count
        info.locals_count = locals_count;
    }

    ClassEmitter::ClassEmitter(ModuleEmitter &module, const string &name, Kind kind, Flags modifiers, vector<string> supers) : module(&module) {
        // info.kind
        switch (kind) {
        case Kind::CLASS:
            info.kind = 0;
            break;
        case Kind::INTERFACE:
            info.kind = 1;
            break;
        case Kind::ANNOTATION:
            info.kind = 2;
            break;
        case Kind::ENUM:
            info.kind = 3;
            break;
        }
        // info.access_flags
        info.access_flags = modifiers.get_raw();
        // info.name
        info.name = module.get_constant(name);
        // info.supers
        vector<CpInfo> cp_supers;
        for (const auto &super: supers) cp_supers.push_back(CpInfo::from_string(super));
        info.supers = module.get_constant(cp_supers);
        // TODO: info.meta
    }

    void ClassEmitter::add_field(const string &name, bool is_const, Flags modifiers) {
        FieldInfo field;
        field.kind = is_const ? 1 : 0;
        field.access_flags = modifiers.get_raw();
        field.name = module->get_constant(name);
        // TODO: field.meta

        info.fields_count++;
        info.fields.push_back(field);
    }

    ModuleEmitter::ModuleEmitter(const string &name, bool is_executable, const fs::path &path) {
        info.kind = is_executable ? 0 : 1;
        info.compiled_from = get_constant(path.string());
        info.name = get_constant(name);
        // TODO: info.init
        // TODO: info.meta
    }

    void ModuleEmitter::add_global(const string &name, bool is_const, Flags modifiers) {
        GlobalInfo global;
        global.kind = is_const ? 1 : 0;
        global.access_flags = modifiers.get_raw();
        global.name = get_constant(name);
        // TODO: global.meta

        info.globals_count++;
        info.globals.push_back(global);
    }
}    // namespace spadec
