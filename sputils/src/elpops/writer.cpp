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
        write(elp.minorVersion);
        write(elp.majorVersion);
        write(elp.compiledFrom);
        write(elp.type);
        write(elp.thisModule);
        write(elp.init);
        write(elp.entry);
        write(elp.imports);
        write(elp.constantPoolCount);
        for (int i = 0; i < elp.constantPoolCount; ++i) {
            write(elp.constantPool[i]);
        }
        write(elp.globalsCount);
        for (int i = 0; i < elp.globalsCount; ++i) {
            write(elp.globals[i]);
        }
        write(elp.objectsCount);
        for (int i = 0; i < elp.objectsCount; ++i) {
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
        write(info.thisGlobal);
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
        write(info.accessFlags);
        write(info.type);
        write(info.thisMethod);
        write(info.typeParamCount);
        for (int i = 0; i < info.typeParamCount; ++i) {
            write(info.typeParams[i]);
        }
        write(info.argsCount);
        for (int i = 0; i < info.argsCount; ++i) {
            write(info.args[i]);
        }
        write(info.localsCount);
        write(info.closureStart);
        for (int i = 0; i < info.localsCount; ++i) {
            write(info.locals[i]);
        }
        write(info.maxStack);
        write(info.codeCount);
        for (ui4 i = 0; i < info.codeCount; ++i) {
            write(info.code[i]);
        }
        write(info.exceptionTableCount);
        for (int i = 0; i < info.exceptionTableCount; ++i) {
            write(info.exceptionTable[i]);
        }
        write(info.lineInfo);
        write(info.lambdaCount);
        for (int i = 0; i < info.lambdaCount; ++i) {
            write(info.lambdas[i]);
        }
        write(info.matchCount);
        for (int i = 0; i < info.matchCount; ++i) {
            write(info.matches[i]);
        }
        write(info.meta);
    }

    void ElpWriter::write(MethodInfo::LineInfo line) {
        write(line.numberCount);
        for (int i = 0; i < line.numberCount; ++i) {
            auto info = line.numbers[i];
            write(info.times);
            write(info.lineno);
        }
    }

    void ElpWriter::write(const MethodInfo::ArgInfo &info) {
        write(info.thisArg);
        write(info.type);
        write(info.meta);
    }

    void ElpWriter::write(const MethodInfo::LocalInfo &info) {
        write(info.thisLocal);
        write(info.type);
        write(info.meta);
    }

    void ElpWriter::write(const MethodInfo::ExceptionTableInfo &info) {
        write(info.startPc);
        write(info.endPc);
        write(info.targetPc);
        write(info.exception);
        write(info.meta);
    }

    void ElpWriter::write(const MethodInfo::MatchInfo &info) {
        write(info.caseCount);
        for (int i = 0; i < info.caseCount; ++i) {
            write(info.cases[i]);
        }
        write(info.defaultLocation);
        write(info.meta);
    }

    void ElpWriter::write(MethodInfo::MatchInfo::CaseInfo info) {
        write(info.value);
        write(info.location);
    }

    void ElpWriter::write(const ClassInfo &info) {
        write(info.type);
        write(info.accessFlags);
        write(info.thisClass);
        write(info.typeParamCount);
        for (int i = 0; i < info.typeParamCount; ++i) {
            write(info.typeParams[i]);
        }
        write(info.supers);
        write(info.fieldsCount);
        for (int i = 0; i < info.fieldsCount; ++i) {
            write(info.fields[i]);
        }
        write(info.methodsCount);
        for (int i = 0; i < info.methodsCount; ++i) {
            write(info.methods[i]);
        }
        write(info.objectsCount);
        for (int i = 0; i < info.objectsCount; ++i) {
            write(info.objects[i]);
        }
        write(info.meta);
    }

    void ElpWriter::write(const FieldInfo &info) {
        write(info.flags);
        write(info.thisField);
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