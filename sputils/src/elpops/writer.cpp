#include "writer.hpp"
#include "elpdef.hpp"
#include "spimp/error.hpp"

namespace spade
{
    ElpWriter::ElpWriter(const string &filename) : path(filename), file(filename, std::ios::out | std::ios::binary) {
        if (!file.is_open())
            throw FileNotFoundError(filename);
    }

    void ElpWriter::close() {
        file.close();
    }

    void ElpWriter::write(const ElpInfo &elp) {
        write(elp.magic);
        write(elp.minor_version);
        write(elp.major_version);

        write(elp.entry);
        write(elp.imports);

        write(elp.constant_pool_count);
        for (int i = 0; i < elp.constant_pool_count; ++i) {
            write(elp.constant_pool[i]);
        }

        write(elp.modules_count);
        for (int i = 0; i < elp.modules_count; ++i) {
            write(elp.modules[i]);
        }

        write(elp.meta);
    }

    void ElpWriter::write(const ModuleInfo &elp) {
        write(elp.kind);
        write(elp.compiled_from);
        write(elp.name);
        write(elp.init);

        write(elp.globals_count);
        for (int i = 0; i < elp.globals_count; ++i) {
            write(elp.globals[i]);
        }

        write(elp.methods_count);
        for (int i = 0; i < elp.methods_count; ++i) {
            write(elp.methods[i]);
        }

        write(elp.classes_count);
        for (int i = 0; i < elp.classes_count; ++i) {
            write(elp.classes[i]);
        }

        write(elp.constant_pool_count);
        for (int i = 0; i < elp.constant_pool_count; ++i) {
            write(elp.constant_pool[i]);
        }

        write(elp.modules_count);
        for (int i = 0; i < elp.modules_count; ++i) {
            write(elp.modules[i]);
        }

        write(elp.meta);
    }

    void ElpWriter::write(const ClassInfo &info) {
        write(info.kind);
        write(info.access_flags);
        write(info.name);
        write(info.supers);

        write(info.type_params_count);
        for (int i = 0; i < info.type_params_count; ++i) {
            write(info.type_params[i]);
        }

        write(info.fields_count);
        for (int i = 0; i < info.fields_count; ++i) {
            write(info.fields[i]);
        }

        write(info.methods_count);
        for (int i = 0; i < info.methods_count; ++i) {
            write(info.methods[i]);
        }

        write(info.meta);
    }

    void ElpWriter::write(const FieldInfo &info) {
        write(info.kind);
        write(info.access_flags);
        write(info.name);
        write(info.type);
        write(info.meta);
    }

    void ElpWriter::write(const MethodInfo &info) {
        write(info.kind);
        write(info.access_flags);
        write(info.name);

        write(info.type_params_count);
        for (int i = 0; i < info.type_params_count; ++i) {
            write(info.type_params[i]);
        }

        write(info.args_count);
        for (int i = 0; i < info.args_count; ++i) {
            write(info.args[i]);
        }

        write(info.locals_count);
        write(info.closure_start);
        for (int i = 0; i < info.locals_count; ++i) {
            write(info.locals[i]);
        }

        write(info.stack_max);
        write(info.code_count);
        for (uint32_t i = 0; i < info.code_count; ++i) {
            write(info.code[i]);
        }

        write(info.exception_table_count);
        for (int i = 0; i < info.exception_table_count; ++i) {
            write(info.exception_table[i]);
        }

        write(info.line_info);

        write(info.match_count);
        for (int i = 0; i < info.match_count; ++i) {
            write(info.matches[i]);
        }

        write(info.meta);
    }

    void ElpWriter::write(const MatchInfo &info) {
        write(info.case_count);
        for (int i = 0; i < info.case_count; ++i) {
            write(info.cases[i].value);
            write(info.cases[i].location);
        }
        write(info.default_location);
        write(info.meta);
    }

    void ElpWriter::write(const LineInfo &line) {
        write(line.number_count);
        for (int i = 0; i < line.number_count; ++i) {
            auto info = line.numbers[i];
            write(info.times);
            write(info.lineno);
        }
    }

    void ElpWriter::write(const ExceptionTableInfo &info) {
        write(info.start_pc);
        write(info.end_pc);
        write(info.target_pc);
        write(info.exception);
        write(info.meta);
    }

    void ElpWriter::write(const LocalInfo &info) {
        write(info.kind);
        write(info.name);
        write(info.type);
        write(info.meta);
    }

    void ElpWriter::write(const ArgInfo &info) {
        write(info.kind);
        write(info.name);
        write(info.type);
        write(info.meta);
    }

    void ElpWriter::write(const TypeParamInfo &info) {
        write(info.name);
    }

    void ElpWriter::write(const GlobalInfo &info) {
        write(info.kind);
        write(info.access_flags);
        write(info.name);
        write(info.type);
        write(info.meta);
    }

    void ElpWriter::write(const MetaInfo &info) {
        write(info.len);
        for (int i = 0; i < info.len; ++i) {
            auto meta = info.table[i];
            write(meta.key);
            write(meta.value);
        }
    }

    void ElpWriter::write(const CpInfo &info) {
        write(info.tag);
        switch (info.tag) {
            case 0x03:
                write(info._char);
                break;
            case 0x04:
                write(info._int);
                break;
            case 0x05:
                write(info._float);
                break;
            case 0x06:
                write(info._string);
                break;
            case 0x07:
                write(info._array);
                break;
            default:
                throw Unreachable();
        }
    }

    void ElpWriter::write(const _Container &info) {
        write(info.len);
        for (int i = 0; i < info.len; ++i) {
            write(info.items[i]);
        }
    }

    void ElpWriter::write(const _UTF8 &info) {
        write(info.len);
        for (int i = 0; i < info.len; ++i) {
            write(info.bytes[i]);
        }
    }
}    // namespace spade