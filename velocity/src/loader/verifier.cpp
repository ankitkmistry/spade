#include "verifier.hpp"

namespace spade {
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

        auto cpCount = elp.constantPoolCount;
        for (int i = 0; i < cpCount; ++i) {
            checkCp(elp.constantPool[i]);
        }
        for (int i = 0; i < elp.globalsCount; i++) {
            checkGlobal(elp.globals[i], cpCount);
        }
        for (int i = 0; i < elp.objectsCount; i++) {
            checkObj(elp.objects[i], cpCount);
        }
    }

    void Verifier::checkObj(ObjInfo object, uint16 cpCount) {
        switch (object.type) {
            case 0x01:
                checkMethod(object._method, cpCount);
                break;
            case 0x02:
                checkClass(object._class, cpCount);
                break;
            default:
                throw corrupt();
        }
    }

    void Verifier::checkClass(ClassInfo klass, uint16 cpCount) {
        if (klass.type < 0x01 || klass.type > 0x04)
            throw corrupt();
        checkRange(klass.thisClass, cpCount);
        checkRange(klass.supers, cpCount);

        for (int i = 0; i < klass.fieldsCount; i++) {
            checkField(klass.fields[i], cpCount);
        }
        for (int i = 0; i < klass.methodsCount; i++) {
            checkMethod(klass.methods[i], cpCount);
        }
        for (int i = 0; i < klass.objectsCount; i++) {
            checkObj(klass.objects[i], cpCount);
        }
    }

    void Verifier::checkField(FieldInfo field, uint16 cpCount) {
        checkRange(field.thisField, cpCount);
        checkRange(field.type, cpCount);
    }

    void Verifier::checkMethod(MethodInfo method, uint16 cpCount) {
        if (method.type != 0x01 && method.type != 0x02) {
            throw corrupt();
        }
        for (int i = 0; i < method.typeParamCount; ++i) {
            checkRange(method.typeParams[i].name, cpCount);
        }
        for (int i = 0; i < method.argsCount; i++) {
            checkArg(method.args[i], cpCount);
        }
        for (int i = 0; i < method.localsCount; i++) {
            checkLocal(method.locals[i], cpCount);
        }
        for (int i = 0; i < method.exceptionTableCount; i++) {
            checkException(method.exceptionTable[i], cpCount);
        }
        uint32 codeCount = method.codeCount;
        checkLine(method.lineInfo, codeCount);
        for (int i = 0; i < method.lambdaCount; ++i) {
            checkMethod(method.lambdas[i], cpCount);
        }
        for (int i = 0; i < method.matchCount; ++i) {
            checkMatch(method.matches[i], codeCount, cpCount);
        }
    }

    void Verifier::checkMatch(MethodInfo::MatchInfo info, uint32 codeCount, uint16 cpCount) {
        for (int i = 0; i < info.caseCount; ++i) {
            auto kase = info.cases[i];
            checkRange(kase.value, cpCount);
            checkRange(kase.location, codeCount);
        }
        checkRange(info.defaultLocation, codeCount);
    }

    void Verifier::checkLocal(MethodInfo::LocalInfo local, uint16 cpCount) {
        checkRange(local.thisLocal, cpCount);
        checkRange(local.type, cpCount);
    }

    void Verifier::checkLine(MethodInfo::LineInfo line, uint16 codeCount) {
        uint32 totalByteLines = 0;
        for (int i = 0; i < line.numberCount; ++i) {
            totalByteLines += line.numbers[i].times;
        }
        if (totalByteLines > codeCount) throw corrupt();
    }

    void Verifier::checkException(MethodInfo::ExceptionTableInfo exception,
                                  uint16 cpCount) {
        checkRange(exception.exception, cpCount);
    }

    void Verifier::checkArg(MethodInfo::ArgInfo arg, uint16 cpCount) {
        checkRange(arg.thisArg, cpCount);
        checkRange(arg.type, cpCount);
    }

    void Verifier::checkGlobal(GlobalInfo global, uint16 cpCount) {
        if (global.flags != 0x01 && global.flags != 0x02)
            throw corrupt();
        checkRange(global.thisGlobal, cpCount);
        checkRange(global.type, cpCount);
    }

    void Verifier::checkRange(ui4 i, ui4 count) {
        if (i >= count) throw corrupt();
    }

    void Verifier::checkCp(CpInfo info) {
        if (info.tag > 0x07)
            throw corrupt();
        if (info.tag == 0x07) {
            auto array = info._array;
            for (int i = 0; i < array.len; ++i) {
                checkCp(array.items[i]);
            }
        }
    }
}// namespace spade
