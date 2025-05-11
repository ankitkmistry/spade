#include "writer.hpp"

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
        write(elp.compiled_from);
        write(elp.type);
        write(elp.this_module);
        write(elp.init);
        write(elp.entry);
        write(elp.imports);
        write(elp.constant_pool_count);
        for (int i = 0; i < elp.constant_pool_count; ++i) {
            write(elp.constant_pool[i]);
        }
        write(elp.globals_count);
        for (int i = 0; i < elp.globals_count; ++i) {
            write(elp.globals[i]);
        }
        write(elp.objects_count);
        for (int i = 0; i < elp.objects_count; ++i) {
            write(elp.objects[i]);
        }
        write(elp.meta);
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

    void ElpWriter::write(_UTF8 utf) {
        write(utf.len);
        for (int i = 0; i < utf.len; ++i) {
            write(utf.bytes[i]);
        }
    }

    void ElpWriter::write(_Container con) {
        write(con.len);
        for (int i = 0; i < con.len; ++i) {
            write(con.items[i]);
        }
    }

    void ElpWriter::write(const GlobalInfo &info) {
        write(info.flags);
        write(info.this_global);
        write(info.type);
        write(info.meta);
    }

    void ElpWriter::write(const ObjInfo &info) {
        write(info.type);
        switch (info.type) {
            case 0x01:
                write(info._method);
                break;
            case 0x02:
                write(info._class);
                break;
            default:
                throw Unreachable();
        }
    }

    void ElpWriter::write(const MethodInfo &info) {
        write(info.access_flags);
        write(info.type);
        write(info.this_method);
        write(info.type_param_count);
        for (int i = 0; i < info.type_param_count; ++i) {
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
        write(info.max_stack);
        write(info.code_count);
        for (ui4 i = 0; i < info.code_count; ++i) {
            write(info.code[i]);
        }
        write(info.exception_table_count);
        for (int i = 0; i < info.exception_table_count; ++i) {
            write(info.exception_table[i]);
        }
        write(info.line_info);
        write(info.lambda_count);
        for (int i = 0; i < info.lambda_count; ++i) {
            write(info.lambdas[i]);
        }
        write(info.match_count);
        for (int i = 0; i < info.match_count; ++i) {
            write(info.matches[i]);
        }
        write(info.meta);
    }

    void ElpWriter::write(MethodInfo::LineInfo line) {
        write(line.number_count);
        for (int i = 0; i < line.number_count; ++i) {
            auto info = line.numbers[i];
            write(info.times);
            write(info.lineno);
        }
    }

    void ElpWriter::write(const MethodInfo::ArgInfo &info) {
        write(info.this_arg);
        write(info.type);
        write(info.meta);
    }

    void ElpWriter::write(const MethodInfo::LocalInfo &info) {
        write(info.this_local);
        write(info.type);
        write(info.meta);
    }

    void ElpWriter::write(const MethodInfo::ExceptionTableInfo &info) {
        write(info.start_pc);
        write(info.end_pc);
        write(info.target_pc);
        write(info.exception);
        write(info.meta);
    }

    void ElpWriter::write(const MethodInfo::MatchInfo &info) {
        write(info.case_count);
        for (int i = 0; i < info.case_count; ++i) {
            write(info.cases[i]);
        }
        write(info.default_location);
        write(info.meta);
    }

    void ElpWriter::write(MethodInfo::MatchInfo::CaseInfo info) {
        write(info.value);
        write(info.location);
    }

    void ElpWriter::write(const ClassInfo &info) {
        write(info.type);
        write(info.access_flags);
        write(info.this_class);
        write(info.type_param_count);
        for (int i = 0; i < info.type_param_count; ++i) {
            write(info.type_params[i]);
        }
        write(info.supers);
        write(info.fields_count);
        for (int i = 0; i < info.fields_count; ++i) {
            write(info.fields[i]);
        }
        write(info.methods_count);
        for (int i = 0; i < info.methods_count; ++i) {
            write(info.methods[i]);
        }
        write(info.objects_count);
        for (int i = 0; i < info.objects_count; ++i) {
            write(info.objects[i]);
        }
        write(info.meta);
    }

    void ElpWriter::write(const FieldInfo &info) {
        write(info.flags);
        write(info.this_field);
        write(info.type);
        write(info.meta);
    }

    void ElpWriter::write(TypeParamInfo info) {
        write(info.name);
    }

    void ElpWriter::write(MetaInfo info) {
        write(info.len);
        for (int i = 0; i < info.len; ++i) {
            auto meta = info.table[i];
            write(meta.key);
            write(meta.value);
        }
    }
}    // namespace spade