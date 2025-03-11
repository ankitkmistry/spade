#include "reader.hpp"

namespace spade {
    ElpInfo ElpReader::read() {
        ElpInfo elp;
        elp.magic = read_int();
        elp.minorVersion = read_int();
        elp.majorVersion = read_int();
        elp.compiledFrom = read_short();
        elp.type = read_byte();
        elp.thisModule = read_short();
        elp.init = read_short();
        elp.entry = read_short();
        elp.imports = read_short();
        elp.constantPoolCount = read_short();
        elp.constantPool = new CpInfo[elp.constantPoolCount];
        for (int i = 0; i < elp.constantPoolCount; ++i) {
            elp.constantPool[i] = read_cp_info();
        }
        elp.globalsCount = read_short();
        elp.globals = new GlobalInfo[elp.globalsCount];
        for (int i = 0; i < elp.globalsCount; ++i) {
            elp.globals[i] = read_global_info();
        }
        elp.objectsCount = read_short();
        elp.objects = new ObjInfo[elp.objectsCount];
        for (int i = 0; i < elp.objectsCount; ++i) {
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
        klass.accessFlags = read_short();
        klass.thisClass = read_short();
        klass.typeParamCount = read_byte();
        klass.typeParams = new TypeParamInfo[klass.typeParamCount];
        for (int i = 0; i < klass.typeParamCount; ++i) {
            klass.typeParams[i] = read_type_param_info();
        }
        klass.supers = read_short();
        klass.fieldsCount = read_short();
        klass.fields = new FieldInfo[klass.fieldsCount];
        for (int i = 0; i < klass.fieldsCount; ++i) {
            klass.fields[i] = read_field_info();
        }
        klass.methodsCount = read_short();
        klass.methods = new MethodInfo[klass.methodsCount];
        for (int i = 0; i < klass.methodsCount; ++i) {
            klass.methods[i] = read_method_info();
        }
        klass.objectsCount = read_short();
        klass.objects = new ObjInfo[klass.objectsCount];
        for (int i = 0; i < klass.objectsCount; ++i) {
            klass.objects[i] = read_obj_info();
        }
        klass.meta = read_meta_info();
        return klass;
    }

    FieldInfo ElpReader::read_field_info() {
        FieldInfo field;
        field.flags = read_byte();
        field.thisField = read_short();
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
        method.accessFlags = read_short();
        method.type = read_byte();
        method.thisMethod = read_short();
        method.typeParamCount = read_byte();
        method.typeParams = new TypeParamInfo[method.typeParamCount];
        for (int i = 0; i < method.typeParamCount; ++i) {
            method.typeParams[i] = read_type_param_info();
        }
        method.argsCount = read_byte();
        method.args = new MethodInfo::ArgInfo[method.argsCount];
        for (int i = 0; i < method.argsCount; i++) {
            method.args[i] = read_arg_info();
        }
        method.localsCount = read_short();
        method.closureStart = read_short();
        method.locals = new MethodInfo::LocalInfo[method.localsCount];
        for (int i = 0; i < method.localsCount; i++) {
            method.locals[i] = read_local_info();
        }
        method.maxStack = read_int();
        method.codeCount = read_int();
        method.code = new uint8_t[method.codeCount];
        for (ui4 i = 0; i < method.codeCount; i++) {
            method.code[i] = read_byte();
        }
        method.exceptionTableCount = read_short();
        method.exceptionTable = new MethodInfo::ExceptionTableInfo[method.exceptionTableCount];
        for (int i = 0; i < method.exceptionTableCount; i++) {
            method.exceptionTable[i] = read_exception_info();
        }
        method.lineInfo = read_line_info();
        method.lambdaCount = read_short();
        method.lambdas = new MethodInfo[method.lambdaCount];
        for (int i = 0; i < method.lambdaCount; i++) {
            method.lambdas[i] = read_method_info();
        }
        method.matchCount = read_short();
        method.matches = new MethodInfo::MatchInfo[method.matchCount];
        for (int i = 0; i < method.matchCount; i++) {
            method.matches[i] = read_match_info();
        }
        method.meta = read_meta_info();
        return method;
    }

    MethodInfo::MatchInfo ElpReader::read_match_info() {
        MethodInfo::MatchInfo match;
        match.caseCount = read_short();
        match.cases = new MethodInfo::MatchInfo::CaseInfo[match.caseCount];
        for (int i = 0; i < match.caseCount; i++) {
            match.cases[i] = read_case_info();
        }
        match.defaultLocation = read_int();
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
        line.numberCount = read_short();
        line.numbers = new MethodInfo::LineInfo::NumberInfo[line.numberCount];
        for (int i = 0; i < line.numberCount; ++i) {
            MethodInfo::LineInfo::NumberInfo number;
            number.times = read_byte();
            number.lineno = read_int();
            line.numbers[i] = number;
        }
        return line;
    }

    MethodInfo::ExceptionTableInfo ElpReader::read_exception_info() {
        MethodInfo::ExceptionTableInfo exception;
        exception.startPc = read_int();
        exception.endPc = read_int();
        exception.targetPc = read_int();
        exception.exception = read_short();
        exception.meta = read_meta_info();
        return exception;
    }

    MethodInfo::LocalInfo ElpReader::read_local_info() {
        MethodInfo::LocalInfo local;
        local.thisLocal = read_short();
        local.type = read_short();
        local.meta = read_meta_info();
        return local;
    }

    MethodInfo::ArgInfo ElpReader::read_arg_info() {
        MethodInfo::ArgInfo arg;
        arg.thisArg = read_short();
        arg.type = read_short();
        arg.meta = read_meta_info();
        return arg;
    }

    GlobalInfo ElpReader::read_global_info() {
        GlobalInfo global;
        global.flags = read_byte();
        global.thisGlobal = read_short();
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
