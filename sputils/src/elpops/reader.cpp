#include "reader.hpp"

ElpInfo ElpReader::read() {
    ElpInfo elp;
    elp.magic = readInt();
    elp.minorVersion = readInt();
    elp.majorVersion = readInt();
    elp.compiledFrom = readShort();
    elp.type = readByte();
    elp.thisModule = readShort();
    elp.init = readShort();
    elp.entry = readShort();
    elp.imports = readShort();
    elp.constantPoolCount = readShort();
    elp.constantPool = new CpInfo[elp.constantPoolCount];
    for (int i = 0; i < elp.constantPoolCount; ++i) {
        elp.constantPool[i] = readCpInfo();
    }
    elp.globalsCount = readShort();
    elp.globals = new GlobalInfo[elp.globalsCount];
    for (int i = 0; i < elp.globalsCount; ++i) {
        elp.globals[i] = readGlobalInfo();
    }
    elp.objectsCount = readShort();
    elp.objects = new ObjInfo[elp.objectsCount];
    for (int i = 0; i < elp.objectsCount; ++i) {
        elp.objects[i] = readObjInfo();
    }
    elp.meta = readMetaInfo();
    // Reset the index to zero so that the file can be read again
    index = 0;
    return elp;
}

MetaInfo ElpReader::readMetaInfo() {
    MetaInfo meta;
    meta.len = readShort();
    meta.table = new MetaInfo::__meta[meta.len];
    for (int i = 0; i < meta.len; ++i) {
        MetaInfo::__meta entry;
        entry.key = readUTF8();
        entry.value = readUTF8();
        meta.table[i] = entry;
    }
    return meta;
}

ObjInfo ElpReader::readObjInfo() {
    ObjInfo obj;
    obj.type = readByte();
    switch (obj.type) {
        case 0x01:
            obj._method = readMethodInfo();
            break;
        case 0x02:
            obj._class = readClassInfo();
            break;
        default:
            corruptFileError();
    }
    return obj;
}

ClassInfo ElpReader::readClassInfo() {
    ClassInfo klass;
    klass.type = readByte();
    klass.accessFlags = readShort();
    klass.thisClass = readShort();
    klass.typeParamCount = readByte();
    klass.typeParams = new TypeParamInfo[klass.typeParamCount];
    for (int i = 0; i < klass.typeParamCount; ++i) {
        klass.typeParams[i] = readTypeParamInfo();
    }
    klass.supers = readShort();
    klass.fieldsCount = readShort();
    klass.fields = new FieldInfo[klass.fieldsCount];
    for (int i = 0; i < klass.fieldsCount; ++i) {
        klass.fields[i] = readFieldInfo();
    }
    klass.methodsCount = readShort();
    klass.methods = new MethodInfo[klass.methodsCount];
    for (int i = 0; i < klass.methodsCount; ++i) {
        klass.methods[i] = readMethodInfo();
    }
    klass.objectsCount = readShort();
    klass.objects = new ObjInfo[klass.objectsCount];
    for (int i = 0; i < klass.objectsCount; ++i) {
        klass.objects[i] = readObjInfo();
    }
    klass.meta = readMetaInfo();
    return klass;
}

FieldInfo ElpReader::readFieldInfo() {
    FieldInfo field;
    field.flags = readByte();
    field.thisField = readShort();
    field.type = readShort();
    field.meta = readMetaInfo();
    return field;
}

TypeParamInfo ElpReader::readTypeParamInfo() {
    TypeParamInfo typeParam;
    typeParam.name = readShort();
    return typeParam;
}

MethodInfo ElpReader::readMethodInfo() {
    MethodInfo method;
    method.accessFlags = readShort();
    method.type = readByte();
    method.thisMethod = readShort();
    method.typeParamCount = readByte();
    method.typeParams = new TypeParamInfo[method.typeParamCount];
    for (int i = 0; i < method.typeParamCount; ++i) {
        method.typeParams[i] = readTypeParamInfo();
    }
    method.argsCount = readByte();
    method.args = new MethodInfo::ArgInfo[method.argsCount];
    for (int i = 0; i < method.argsCount; i++) {
        method.args[i] = readArgInfo();
    }
    method.localsCount = readShort();
    method.closureStart = readShort();
    method.locals = new MethodInfo::LocalInfo[method.localsCount];
    for (int i = 0; i < method.localsCount; i++) {
        method.locals[i] = readLocalInfo();
    }
    method.maxStack = readInt();
    method.codeCount = readInt();
    method.code = new uint8_t[method.codeCount];
    for (ui4 i = 0; i < method.codeCount; i++) {
        method.code[i] = readByte();
    }
    method.exceptionTableCount = readShort();
    method.exceptionTable =
            new MethodInfo::ExceptionTableInfo[method.exceptionTableCount];
    for (int i = 0; i < method.exceptionTableCount; i++) {
        method.exceptionTable[i] = readExceptionInfo();
    }
    method.lineInfo = readLineInfo();
    method.lambdaCount = readShort();
    method.lambdas = new MethodInfo[method.lambdaCount];
    for (int i = 0; i < method.lambdaCount; i++) {
        method.lambdas[i] = readMethodInfo();
    }
    method.matchCount = readShort();
    method.matches = new MethodInfo::MatchInfo[method.matchCount];
    for (int i = 0; i < method.matchCount; i++) {
        method.matches[i] = readMatchInfo();
    }
    method.meta = readMetaInfo();
    return method;
}

MethodInfo::MatchInfo ElpReader::readMatchInfo() {
    MethodInfo::MatchInfo match;
    match.caseCount = readShort();
    match.cases = new MethodInfo::MatchInfo::CaseInfo[match.caseCount];
    for (int i = 0; i < match.caseCount; i++) {
        match.cases[i] = readCaseInfo();
    }
    match.defaultLocation = readInt();
    match.meta = readMetaInfo();
    return match;
}

MethodInfo::MatchInfo::CaseInfo ElpReader::readCaseInfo() {
    MethodInfo::MatchInfo::CaseInfo kase;
    kase.value = readShort();
    kase.location = readInt();
    return kase;
}

MethodInfo::LineInfo ElpReader::readLineInfo() {
    MethodInfo::LineInfo line;
    line.numberCount = readShort();
    line.numbers = new MethodInfo::LineInfo::NumberInfo[line.numberCount];
    for (int i = 0; i < line.numberCount; ++i) {
        MethodInfo::LineInfo::NumberInfo number;
        number.times = readByte();
        number.lineno = readInt();
        line.numbers[i] = number;
    }
    return line;
}

MethodInfo::ExceptionTableInfo ElpReader::readExceptionInfo() {
    MethodInfo::ExceptionTableInfo exception;
    exception.startPc = readInt();
    exception.endPc = readInt();
    exception.targetPc = readInt();
    exception.exception = readShort();
    exception.meta = readMetaInfo();
    return exception;
}

MethodInfo::LocalInfo ElpReader::readLocalInfo() {
    MethodInfo::LocalInfo local;
    local.thisLocal = readShort();
    local.type = readShort();
    local.meta = readMetaInfo();
    return local;
}

MethodInfo::ArgInfo ElpReader::readArgInfo() {
    MethodInfo::ArgInfo arg;
    arg.thisArg = readShort();
    arg.type = readShort();
    arg.meta = readMetaInfo();
    return arg;
}

GlobalInfo ElpReader::readGlobalInfo() {
    GlobalInfo global;
    global.flags = readByte();
    global.thisGlobal = readShort();
    global.type = readShort();
    global.meta = readMetaInfo();
    return global;
}

CpInfo ElpReader::readCpInfo() {
    CpInfo cp;
    cp.tag = readByte();
    switch (cp.tag) {
        case 0x03:
            cp._char = readInt();
            break;
        case 0x04:
            cp._int = readLong();
            break;
        case 0x05:
            cp._float = readLong();
            break;
        case 0x06:
            cp._string = readUTF8();
            break;
        case 0x07:
            cp._array = readContainer();
            break;
        default:
            corruptFileError();
    }
    return cp;
}

__Container ElpReader::readContainer() {
    __Container container;
    container.len = readShort();
    container.items = new CpInfo[container.len];
    for (int i = 0; i < container.len; ++i) {
        container.items[i] = readCpInfo();
    }
    return container;
}

__UTF8 ElpReader::readUTF8() {
    __UTF8 utf8;
    utf8.len = readShort();
    utf8.bytes = new uint8_t[utf8.len];
    for (int i = 0; i < utf8.len; ++i) {
        utf8.bytes[i] = readByte();
    }
    return utf8;
}
