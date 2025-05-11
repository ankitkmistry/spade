#include "verifier.hpp"

namespace spade
{
    void Verifier::verify() {
        auto magic = elp.magic;
        switch (elp.type) {
            case 0x01: {
                if (magic != 0xc0ffeede)
                    throw corrupt();
                break;
            }
            case 0x02: {
                if (magic != 0x6020cafe)
                    throw corrupt();
                break;
            }
            default:
                throw corrupt();
        }

        auto cp_count = elp.constant_pool_count;
        for (int i = 0; i < cp_count; ++i) {
            check_cp(elp.constant_pool[i]);
        }
        for (int i = 0; i < elp.globals_count; i++) {
            check_global(elp.globals[i], cp_count);
        }
        for (int i = 0; i < elp.objects_count; i++) {
            check_obj(elp.objects[i], cp_count);
        }
    }

    void Verifier::check_obj(ObjInfo object, uint16 cp_count) {
        switch (object.type) {
            case 0x01:
                check_method(object._method, cp_count);
                break;
            case 0x02:
                check_class(object._class, cp_count);
                break;
            default:
                throw corrupt();
        }
    }

    void Verifier::check_class(ClassInfo klass, uint16 cp_count) {
        if (klass.type < 0x01 || klass.type > 0x04)
            throw corrupt();
        check_range(klass.this_class, cp_count);
        check_range(klass.supers, cp_count);

        for (int i = 0; i < klass.fields_count; i++) {
            check_field(klass.fields[i], cp_count);
        }
        for (int i = 0; i < klass.methods_count; i++) {
            check_method(klass.methods[i], cp_count);
        }
        for (int i = 0; i < klass.objects_count; i++) {
            check_obj(klass.objects[i], cp_count);
        }
    }

    void Verifier::check_field(FieldInfo field, uint16 cp_count) {
        check_range(field.this_field, cp_count);
        check_range(field.type, cp_count);
    }

    void Verifier::check_method(MethodInfo method, uint16 cp_count) {
        if (method.type != 0x01 && method.type != 0x02) {
            throw corrupt();
        }
        for (int i = 0; i < method.type_param_count; ++i) {
            check_range(method.type_params[i].name, cp_count);
        }
        for (int i = 0; i < method.args_count; i++) {
            check_arg(method.args[i], cp_count);
        }
        for (int i = 0; i < method.locals_count; i++) {
            check_local(method.locals[i], cp_count);
        }
        for (int i = 0; i < method.exception_table_count; i++) {
            check_exception(method.exception_table[i], cp_count);
        }
        uint32 code_count = method.code_count;
        check_line(method.line_info, code_count);
        for (int i = 0; i < method.lambda_count; ++i) {
            check_method(method.lambdas[i], cp_count);
        }
        for (int i = 0; i < method.match_count; ++i) {
            check_match(method.matches[i], code_count, cp_count);
        }
    }

    void Verifier::check_match(MethodInfo::MatchInfo info, uint32 code_count, uint16 cp_count) {
        for (int i = 0; i < info.case_count; ++i) {
            auto kase = info.cases[i];
            check_range(kase.value, cp_count);
            check_range(kase.location, code_count);
        }
        check_range(info.default_location, code_count);
    }

    void Verifier::check_local(MethodInfo::LocalInfo local, uint16 cp_count) {
        check_range(local.this_local, cp_count);
        check_range(local.type, cp_count);
    }

    void Verifier::check_line(MethodInfo::LineInfo line, uint16 code_count) {
        uint32 total_byte_lines = 0;
        for (int i = 0; i < line.number_count; ++i) {
            total_byte_lines += line.numbers[i].times;
        }
        if (total_byte_lines > code_count)
            throw corrupt();
    }

    void Verifier::check_exception(MethodInfo::ExceptionTableInfo exception, uint16 cp_count) {
        check_range(exception.exception, cp_count);
    }

    void Verifier::check_arg(MethodInfo::ArgInfo arg, uint16 cp_count) {
        check_range(arg.this_arg, cp_count);
        check_range(arg.type, cp_count);
    }

    void Verifier::check_global(GlobalInfo global, uint16 cp_count) {
        if (global.flags != 0x01 && global.flags != 0x02)
            throw corrupt();
        check_range(global.this_global, cp_count);
        check_range(global.type, cp_count);
    }

    void Verifier::check_range(ui4 i, ui4 count) {
        if (i >= count)
            throw corrupt();
    }

    void Verifier::check_cp(CpInfo info) {
        if (info.tag > 0x07)
            throw corrupt();
        if (info.tag == 0x07) {
            auto array = info._array;
            for (int i = 0; i < array.len; ++i) {
                check_cp(array.items[i]);
            }
        }
    }
}    // namespace spade
