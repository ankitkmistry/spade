#include "reader.hpp"

namespace spade {
    ElpInfo ElpReader::read() {
        ElpInfo elp;
        elp.magic = read_int();
        elp.minor_version = read_int();
        elp.major_version = read_int();
        elp.compiled_from = read_short();
        elp.type = read_byte();
        elp.this_module = read_short();
        elp.init = read_short();
        elp.entry = read_short();
        elp.imports = read_short();
        elp.constant_pool_count = read_short();
        elp.constant_pool = new CpInfo[elp.constant_pool_count];
        for (int i = 0; i < elp.constant_pool_count; ++i) {
            elp.constant_pool[i] = read_cp_info();
        }
        elp.globals_count = read_short();
        elp.globals = new GlobalInfo[elp.globals_count];
        for (int i = 0; i < elp.globals_count; ++i) {
            elp.globals[i] = read_global_info();
        }
        elp.objects_count = read_short();
        elp.objects = new ObjInfo[elp.objects_count];
        for (int i = 0; i < elp.objects_count; ++i) {
            elp.objects[i] = read_obj_info();
        }
        elp.meta = read_meta_info();
        // Reset the index to zero so that the file can be read again
        index = 0;
        return elp;
    }

    MetaInfo ElpReader::read_meta_info() {
        MetaInfo meta;
        meta.len = read_short();
        meta.table = new MetaInfo::_meta[meta.len];
        for (int i = 0; i < meta.len; ++i) {
            MetaInfo::_meta entry;
            entry.key = read_utf8();
            entry.value = read_utf8();
            meta.table[i] = entry;
        }
        return meta;
    }

    ObjInfo ElpReader::read_obj_info() {
        ObjInfo obj;
        obj.type = read_byte();
        switch (obj.type) {
            case 0x01:
                obj._method = read_method_info();
                break;
            case 0x02:
                obj._class = read_class_info();
                break;
            default:
                corrupt_file_error();
        }
        return obj;
    }

    ClassInfo ElpReader::read_class_info() {
        ClassInfo klass;
        klass.type = read_byte();
        klass.access_flags = read_short();
        klass.this_class = read_short();
        klass.type_param_count = read_byte();
        klass.type_params = new TypeParamInfo[klass.type_param_count];
        for (int i = 0; i < klass.type_param_count; ++i) {
            klass.type_params[i] = read_type_param_info();
        }
        klass.supers = read_short();
        klass.fields_count = read_short();
        klass.fields = new FieldInfo[klass.fields_count];
        for (int i = 0; i < klass.fields_count; ++i) {
            klass.fields[i] = read_field_info();
        }
        klass.methods_count = read_short();
        klass.methods = new MethodInfo[klass.methods_count];
        for (int i = 0; i < klass.methods_count; ++i) {
            klass.methods[i] = read_method_info();
        }
        klass.objects_count = read_short();
        klass.objects = new ObjInfo[klass.objects_count];
        for (int i = 0; i < klass.objects_count; ++i) {
            klass.objects[i] = read_obj_info();
        }
        klass.meta = read_meta_info();
        return klass;
    }

    FieldInfo ElpReader::read_field_info() {
        FieldInfo field;
        field.flags = read_byte();
        field.this_field = read_short();
        field.type = read_short();
        field.meta = read_meta_info();
        return field;
    }

    TypeParamInfo ElpReader::read_type_param_info() {
        TypeParamInfo typeParam;
        typeParam.name = read_short();
        return typeParam;
    }

    MethodInfo ElpReader::read_method_info() {
        MethodInfo method;
        method.access_flags = read_short();
        method.type = read_byte();
        method.this_method = read_short();
        method.type_param_count = read_byte();
        method.type_params = new TypeParamInfo[method.type_param_count];
        for (int i = 0; i < method.type_param_count; ++i) {
            method.type_params[i] = read_type_param_info();
        }
        method.args_count = read_byte();
        method.args = new MethodInfo::ArgInfo[method.args_count];
        for (int i = 0; i < method.args_count; i++) {
            method.args[i] = read_arg_info();
        }
        method.locals_count = read_short();
        method.closure_start = read_short();
        method.locals = new MethodInfo::LocalInfo[method.locals_count];
        for (int i = 0; i < method.locals_count; i++) {
            method.locals[i] = read_local_info();
        }
        method.max_stack = read_int();
        method.code_count = read_int();
        method.code = new uint8_t[method.code_count];
        for (ui4 i = 0; i < method.code_count; i++) {
            method.code[i] = read_byte();
        }
        method.exception_table_count = read_short();
        method.exception_table = new MethodInfo::ExceptionTableInfo[method.exception_table_count];
        for (int i = 0; i < method.exception_table_count; i++) {
            method.exception_table[i] = read_exception_info();
        }
        method.line_info = read_line_info();
        method.lambda_count = read_short();
        method.lambdas = new MethodInfo[method.lambda_count];
        for (int i = 0; i < method.lambda_count; i++) {
            method.lambdas[i] = read_method_info();
        }
        method.match_count = read_short();
        method.matches = new MethodInfo::MatchInfo[method.match_count];
        for (int i = 0; i < method.match_count; i++) {
            method.matches[i] = read_match_info();
        }
        method.meta = read_meta_info();
        return method;
    }

    MethodInfo::MatchInfo ElpReader::read_match_info() {
        MethodInfo::MatchInfo match;
        match.case_count = read_short();
        match.cases = new MethodInfo::MatchInfo::CaseInfo[match.case_count];
        for (int i = 0; i < match.case_count; i++) {
            match.cases[i] = read_case_info();
        }
        match.default_location = read_int();
        match.meta = read_meta_info();
        return match;
    }

    MethodInfo::MatchInfo::CaseInfo ElpReader::read_case_info() {
        MethodInfo::MatchInfo::CaseInfo kase;
        kase.value = read_short();
        kase.location = read_int();
        return kase;
    }

    MethodInfo::LineInfo ElpReader::read_line_info() {
        MethodInfo::LineInfo line;
        line.number_count = read_short();
        line.numbers = new MethodInfo::LineInfo::NumberInfo[line.number_count];
        for (int i = 0; i < line.number_count; ++i) {
            MethodInfo::LineInfo::NumberInfo number;
            number.times = read_byte();
            number.lineno = read_int();
            line.numbers[i] = number;
        }
        return line;
    }

    MethodInfo::ExceptionTableInfo ElpReader::read_exception_info() {
        MethodInfo::ExceptionTableInfo exception;
        exception.start_pc = read_int();
        exception.end_pc = read_int();
        exception.target_pc = read_int();
        exception.exception = read_short();
        exception.meta = read_meta_info();
        return exception;
    }

    MethodInfo::LocalInfo ElpReader::read_local_info() {
        MethodInfo::LocalInfo local;
        local.this_local = read_short();
        local.type = read_short();
        local.meta = read_meta_info();
        return local;
    }

    MethodInfo::ArgInfo ElpReader::read_arg_info() {
        MethodInfo::ArgInfo arg;
        arg.this_arg = read_short();
        arg.type = read_short();
        arg.meta = read_meta_info();
        return arg;
    }

    GlobalInfo ElpReader::read_global_info() {
        GlobalInfo global;
        global.flags = read_byte();
        global.this_global = read_short();
        global.type = read_short();
        global.meta = read_meta_info();
        return global;
    }

    CpInfo ElpReader::read_cp_info() {
        CpInfo cp;
        cp.tag = read_byte();
        switch (cp.tag) {
            case 0x03:
                cp._char = read_int();
                break;
            case 0x04:
                cp._int = read_long();
                break;
            case 0x05:
                cp._float = read_long();
                break;
            case 0x06:
                cp._string = read_utf8();
                break;
            case 0x07:
                cp._array = read_container();
                break;
            default:
                corrupt_file_error();
        }
        return cp;
    }

    _Container ElpReader::read_container() {
        _Container container;
        container.len = read_short();
        container.items = new CpInfo[container.len];
        for (int i = 0; i < container.len; ++i) {
            container.items[i] = read_cp_info();
        }
        return container;
    }

    _UTF8 ElpReader::read_utf8() {
        _UTF8 utf8;
        utf8.len = read_short();
        utf8.bytes = new uint8_t[utf8.len];
        for (int i = 0; i < utf8.len; ++i) {
            utf8.bytes[i] = read_byte();
        }
        return utf8;
    }
}
