#include "emitter.hpp"
#include "elpops/elpdef.hpp"
#include "spinfo/opcode.hpp"
#include <cassert>
#include <cstdint>

namespace spadec
{
    bool CodeEmitter::append(const CodeEmitter &other) {
        if (module != other.module)
            return false;

        for (const auto &[label, patch_list]: other.patches) {
            for (const auto patch: patch_list) {
                // Relocate the new label positions
                // Because the code is going to be appended
                // at the end of the current code buffer
                patches[label].push_back(patch + code.size());
            }
        }

        code.insert(code.end(), other.code.begin(), other.code.end());
        lines.insert(lines.end(), other.lines.begin(), other.lines.end());
        return true;
    }

    static LineInfo encode_lines(const vector<uint32_t> &lines) {
        LineInfo info;
        for (const auto line: lines) {
            if (info.numbers.empty()) {
                NumberInfo number;
                number.times = 1;
                number.lineno = line;
                info.numbers.push_back(number);
            } else {
                if (auto &last_number = info.numbers.back(); last_number.lineno == line)
                    last_number.times++;
                else {
                    NumberInfo number;
                    number.times = 1;
                    number.lineno = line;
                    info.numbers.push_back(number);
                }
            }
        }
        info.number_count = info.numbers.size();
        return info;
    }

    void CodeEmitter::emit(MethodInfo &info) {
        patch_labels();
        // TODO: compute stack_max
        info.stack_max = 16;
        info.code_count = code.size();
        info.code = code;
        info.line_info = encode_lines(lines);
    }

    // TODO: Do bounds checking for uint16_max
    void CodeEmitter::emit_const(const CpInfo &cp, uint32_t line) {
        switch (cp.tag) {
        case 0:
            emit_opcode(Opcode::CONST_NULL, line);
            break;
        case 1:
            emit_opcode(Opcode::CONST_TRUE, line);
            break;
        case 2:
            emit_opcode(Opcode::CONST_FALSE, line);
            break;
        default: {
            cpidx index = module->get_constant(cp);
            if (index <= uint8_max) {
                emit_opcode(Opcode::CONST, line);
                emit_byte(index & 0xFF, line);
            } else {
                emit_opcode(Opcode::CONSTL, line);
                emit_short(index, line);
            }
            break;
        }
        }
    }

    void CodeEmitter::emit_inst(Opcode opcode, string param, uint32_t line) {
        assert(OpcodeInfo::params_count(opcode) == 2);
        cpidx index = module->get_constant(param);
        if (index <= uint8_max) {
            emit_opcode(OpcodeInfo::alternate(opcode), line);
            emit_byte(index & 0xFF, line);
        } else {
            emit_opcode(opcode, line);
            emit_short(index, line);
        }
    }

    void CodeEmitter::emit_inst(Opcode opcode, uint16_t param, uint32_t line) {
        assert(OpcodeInfo::params_count(opcode) == 2);
        if (param <= uint8_max) {
            emit_opcode(OpcodeInfo::alternate(opcode), line);
            emit_byte(param & 0xFF, line);
        } else {
            emit_opcode(opcode, line);
            emit_short(param, line);
        }
    }

    void CodeEmitter::emit_label(const std::shared_ptr<Label> &label, uint32_t line) {
        size_t patch_location = code.size();
        emit_short(0, line);
        patches[label].push_back(patch_location);
        // offset will be patched at locations
        // ====================================================
        // code[patch_location] code[patch_location + 1]
        // +-- 1 high byte ---+ +----- 1 low byte -----+
        // offset is in big-endian format
        //
        // from_pos = patch_location + 2
        // dest_pos = label.location
        // offset = dest_pos - from_pos
    }

    void CodeEmitter::emit_opcode(Opcode opcode, uint32_t line) {
        emit_byte(static_cast<uint8_t>(opcode), line);
    }

    void CodeEmitter::emit_byte(uint8_t u8, uint32_t line) {
        code.push_back(u8);
        lines.push_back(line);
    }

    void CodeEmitter::emit_short(uint16_t u16, uint32_t line) {
        emit_byte((u16 >> 8) & 0xFF, line);
        emit_byte(u16 & 0xFF, line);
    }

    void CodeEmitter::patch_labels() {
        for (const auto &[label, patch_locs]: patches) {
            for (const auto patch_loc: patch_locs) {
                // TODO: is there any undefined behaviour in these arithmetic
                // what if the offset is unable to describe the jump
                const int32_t from_pos = patch_loc + 2;
                const int32_t dest_pos = label->get_pos();
                const int16_t offset = dest_pos - from_pos;

                code[patch_loc] = (offset >> 8) & 0xFF;
                code[patch_loc + 1] = offset & 0xFF;
            }
        }
        patches.clear();
    }

    MethodEmitter::MethodEmitter(ModuleEmitter &module, const string &name, Kind kind, Flags modifiers, uint8_t args_count, uint16_t locals_count)
        : code_emitter(module) {
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
        info.access_flags = modifiers.get_raw();
        info.name = module.get_constant(name);
        info.args_count = args_count;
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
