#include "elpdef.hpp"
#include "../spimp/error.hpp"
#include "../spimp/utils.hpp"

namespace spade
{
    bool CpInfo::operator==(const CpInfo &rhs) const {
        if (tag != rhs.tag)
            return false;
        switch (tag) {
            case 0x03:
                return _char == rhs._char;
            case 0x04:
                return _int == rhs._int;
            case 0x05:
                return _float == rhs._float;
            case 0x06:
                return _string == rhs._string;
            case 0x07:
                return _array == rhs._array;
            default:
                throw Unreachable();
        }
    }

    bool CpInfo::operator!=(const CpInfo &rhs) const {
        return !(rhs == *this);
    }

    CpInfo CpInfo::fromChar(uint32_t c) {
        return CpInfo{.tag = 0x03, ._char = c};
    }

    CpInfo CpInfo::fromInt(int64_t i) {
        return CpInfo{.tag = 0x04, ._int = signed_to_unsigned(i)};
    }

    CpInfo CpInfo::fromFloat(double d) {
        return CpInfo{.tag = 0x05, ._float = double_to_raw(d)};
    }

    CpInfo CpInfo::fromString(string s) {
        _UTF8 str;
        str.len = s.size();
        str.bytes = new ui1[str.len];
        for (int i = 0; i < str.len; ++i) {
            str.bytes[i] = s[i];
        }
        return CpInfo{.tag = 0x06, ._string = str};
    }

    CpInfo CpInfo::fromArray(std::vector<CpInfo> v) {
        _Container arr;
        arr.len = v.size();
        arr.items = new CpInfo[arr.len];
        for (int i = 0; i < arr.len; ++i) {
            arr.items = &v[i];
        }
        return CpInfo{.tag = 0x07, ._array = arr};
    }

    bool _UTF8::operator==(const _UTF8 &rhs) const {
        return len == rhs.len && memcmp(bytes, rhs.bytes, len) == 0;
    }

    bool _UTF8::operator!=(const _UTF8 &rhs) const {
        return !(rhs == *this);
    }

    bool _Container::operator==(const _Container &rhs) const {
        if (len != rhs.len)
            return false;
        for (int i = 0; i < len; ++i)
            if (items[i] != rhs.items[i])
                return false;
        return true;
    }

    bool _Container::operator!=(const _Container &rhs) const {
        return !(rhs == *this);
    }

    // Helper function to copy MethodInfo
    static void copyMethod(MethodInfo &dest, const MethodInfo &from) {
        dest.accessFlags = from.accessFlags;
        dest.type = from.type;
        dest.thisMethod = from.thisMethod;

        // Copy type parameters
        dest.typeParamCount = from.typeParamCount;
        dest.typeParams = new TypeParamInfo[from.typeParamCount];
        memcpy(dest.typeParams, from.typeParams, from.typeParamCount * sizeof(TypeParamInfo));

        // Copy arguments
        dest.argsCount = from.argsCount;
        dest.args = new MethodInfo::ArgInfo[from.argsCount];
        for (ui1 i = 0; i < from.argsCount; ++i) {
            dest.args[i] = from.args[i];
            // Deep copy meta info for each arg
            auto &meta = dest.args[i].meta;
            meta.table = new MetaInfo::_meta[meta.len];
            for (ui2 j = 0; j < meta.len; ++j) {
                auto &entry = meta.table[j];
                entry.key.bytes = new ui1[entry.key.len];
                entry.value.bytes = new ui1[entry.value.len];
                memcpy(entry.key.bytes, from.args[i].meta.table[j].key.bytes, entry.key.len);
                memcpy(entry.value.bytes, from.args[i].meta.table[j].value.bytes, entry.value.len);
            }
        }

        // Copy locals
        dest.localsCount = from.localsCount;
        dest.closureStart = from.closureStart;
        dest.locals = new MethodInfo::LocalInfo[from.localsCount];
        for (ui2 i = 0; i < from.localsCount; ++i) {
            dest.locals[i] = from.locals[i];
            // Deep copy meta info for each local
            auto &meta = dest.locals[i].meta;
            meta.table = new MetaInfo::_meta[meta.len];
            for (ui2 j = 0; j < meta.len; ++j) {
                auto &entry = meta.table[j];
                entry.key.bytes = new ui1[entry.key.len];
                entry.value.bytes = new ui1[entry.value.len];
                memcpy(entry.key.bytes, from.locals[i].meta.table[j].key.bytes, entry.key.len);
                memcpy(entry.value.bytes, from.locals[i].meta.table[j].value.bytes, entry.value.len);
            }
        }

        // Copy code
        dest.maxStack = from.maxStack;
        dest.codeCount = from.codeCount;
        dest.code = new ui1[from.codeCount];
        memcpy(dest.code, from.code, from.codeCount);

        // Copy exception table
        dest.exceptionTableCount = from.exceptionTableCount;
        dest.exceptionTable = new MethodInfo::ExceptionTableInfo[from.exceptionTableCount];
        for (ui2 i = 0; i < from.exceptionTableCount; ++i) {
            dest.exceptionTable[i] = from.exceptionTable[i];
            // Deep copy meta info
            auto &meta = dest.exceptionTable[i].meta;
            meta.table = new MetaInfo::_meta[meta.len];
            for (ui2 j = 0; j < meta.len; ++j) {
                auto &entry = meta.table[j];
                entry.key.bytes = new ui1[entry.key.len];
                entry.value.bytes = new ui1[entry.value.len];
                memcpy(entry.key.bytes, from.exceptionTable[i].meta.table[j].key.bytes, entry.key.len);
                memcpy(entry.value.bytes, from.exceptionTable[i].meta.table[j].value.bytes, entry.value.len);
            }
        }

        // Copy line info
        dest.lineInfo.numberCount = from.lineInfo.numberCount;
        dest.lineInfo.numbers = new MethodInfo::LineInfo::NumberInfo[from.lineInfo.numberCount];
        memcpy(dest.lineInfo.numbers, from.lineInfo.numbers,
               from.lineInfo.numberCount * sizeof(MethodInfo::LineInfo::NumberInfo));

        // Copy lambdas
        dest.lambdaCount = from.lambdaCount;
        dest.lambdas = new MethodInfo[from.lambdaCount];
        for (ui2 i = 0; i < from.lambdaCount; ++i) {
            copyMethod(dest.lambdas[i], from.lambdas[i]);
        }

        // Copy matches
        dest.matchCount = from.matchCount;
        dest.matches = new MethodInfo::MatchInfo[from.matchCount];
        for (ui2 i = 0; i < from.matchCount; ++i) {
            auto &match = dest.matches[i];
            match.caseCount = from.matches[i].caseCount;
            match.cases = new MethodInfo::MatchInfo::CaseInfo[match.caseCount];
            memcpy(match.cases, from.matches[i].cases, match.caseCount * sizeof(MethodInfo::MatchInfo::CaseInfo));
            match.defaultLocation = from.matches[i].defaultLocation;

            // Deep copy meta info
            match.meta.len = from.matches[i].meta.len;
            match.meta.table = new MetaInfo::_meta[match.meta.len];
            for (ui2 j = 0; j < match.meta.len; ++j) {
                auto &entry = match.meta.table[j];
                entry.key.bytes = new ui1[entry.key.len];
                entry.value.bytes = new ui1[entry.value.len];
                memcpy(entry.key.bytes, from.matches[i].meta.table[j].key.bytes, entry.key.len);
                memcpy(entry.value.bytes, from.matches[i].meta.table[j].value.bytes, entry.value.len);
            }
        }

        // Copy method meta info
        dest.meta.len = from.meta.len;
        dest.meta.table = new MetaInfo::_meta[from.meta.len];
        for (ui2 i = 0; i < from.meta.len; ++i) {
            auto &entry = dest.meta.table[i];
            entry.key.bytes = new ui1[entry.key.len];
            entry.value.bytes = new ui1[entry.value.len];
            memcpy(entry.key.bytes, from.meta.table[i].key.bytes, entry.key.len);
            memcpy(entry.value.bytes, from.meta.table[i].value.bytes, entry.value.len);
        }
    }

    // Helper function to copy ClassInfo
    static void copyClass(ClassInfo &dest, const ClassInfo &from) {
        dest.type = from.type;
        dest.accessFlags = from.accessFlags;
        dest.thisClass = from.thisClass;

        // Copy type parameters
        dest.typeParamCount = from.typeParamCount;
        dest.typeParams = new TypeParamInfo[from.typeParamCount];
        memcpy(dest.typeParams, from.typeParams, from.typeParamCount * sizeof(TypeParamInfo));

        dest.supers = from.supers;

        // Copy fields
        dest.fieldsCount = from.fieldsCount;
        dest.fields = new FieldInfo[from.fieldsCount];
        for (ui2 i = 0; i < from.fieldsCount; ++i) {
            dest.fields[i] = from.fields[i];
            // Deep copy meta info
            auto &meta = dest.fields[i].meta;
            meta.table = new MetaInfo::_meta[meta.len];
            for (ui2 j = 0; j < meta.len; ++j) {
                auto &entry = meta.table[j];
                entry.key.bytes = new ui1[entry.key.len];
                entry.value.bytes = new ui1[entry.value.len];
                memcpy(entry.key.bytes, from.fields[i].meta.table[j].key.bytes, entry.key.len);
                memcpy(entry.value.bytes, from.fields[i].meta.table[j].value.bytes, entry.value.len);
            }
        }

        // Copy methods
        dest.methodsCount = from.methodsCount;
        dest.methods = new MethodInfo[from.methodsCount];
        for (ui2 i = 0; i < from.methodsCount; ++i) {
            copyMethod(dest.methods[i], from.methods[i]);
        }

        // Copy objects
        dest.objectsCount = from.objectsCount;
        dest.objects = new ObjInfo[from.objectsCount];
        for (ui2 i = 0; i < from.objectsCount; ++i) {
            dest.objects[i].type = from.objects[i].type;
            if (from.objects[i].type == 0) {
                copyMethod(dest.objects[i]._method, from.objects[i]._method);
            } else {
                copyClass(dest.objects[i]._class, from.objects[i]._class);
            }
        }

        // Copy class meta info
        dest.meta.len = from.meta.len;
        dest.meta.table = new MetaInfo::_meta[from.meta.len];
        for (ui2 i = 0; i < from.meta.len; ++i) {
            auto &entry = dest.meta.table[i];
            entry.key.bytes = new ui1[entry.key.len];
            entry.value.bytes = new ui1[entry.value.len];
            memcpy(entry.key.bytes, from.meta.table[i].key.bytes, entry.key.len);
            memcpy(entry.value.bytes, from.meta.table[i].value.bytes, entry.value.len);
        }
    }

    void copyElp(ElpInfo &dest, const ElpInfo &from) {
        // Copy basic fields
        dest.magic = from.magic;
        dest.minorVersion = from.minorVersion;
        dest.majorVersion = from.majorVersion;
        dest.compiledFrom = from.compiledFrom;
        dest.type = from.type;
        dest.thisModule = from.thisModule;
        dest.init = from.init;
        dest.entry = from.entry;
        dest.imports = from.imports;

        // Copy constant pool
        dest.constantPoolCount = from.constantPoolCount;
        dest.constantPool = new CpInfo[from.constantPoolCount];
        for (ui2 i = 0; i < from.constantPoolCount; ++i) {
            dest.constantPool[i] = from.constantPool[i];
            // For string and array types, need to deep copy their contents
            if (from.constantPool[i].tag == 0x06) {
                // String type
                auto &str = dest.constantPool[i]._string;
                str.bytes = new ui1[str.len];
                memcpy(str.bytes, from.constantPool[i]._string.bytes, str.len);
            } else if (from.constantPool[i].tag == 0x07) {
                // Array type
                auto &arr = dest.constantPool[i]._array;
                arr.items = new CpInfo[arr.len];
                for (ui2 j = 0; j < arr.len; ++j) {
                    arr.items[j] = from.constantPool[i]._array.items[j];
                }
            }
        }

        // Copy globals
        dest.globalsCount = from.globalsCount;
        dest.globals = new GlobalInfo[from.globalsCount];
        for (ui2 i = 0; i < from.globalsCount; ++i) {
            dest.globals[i] = from.globals[i];
            // Deep copy meta info
            auto &meta = dest.globals[i].meta;
            meta.table = new MetaInfo::_meta[meta.len];
            for (ui2 j = 0; j < meta.len; ++j) {
                auto &entry = meta.table[j];
                entry.key.bytes = new ui1[entry.key.len];
                entry.value.bytes = new ui1[entry.value.len];
                memcpy(entry.key.bytes, from.globals[i].meta.table[j].key.bytes, entry.key.len);
                memcpy(entry.value.bytes, from.globals[i].meta.table[j].value.bytes, entry.value.len);
            }
        }

        // Copy objects
        dest.objectsCount = from.objectsCount;
        dest.objects = new ObjInfo[from.objectsCount];
        for (ui2 i = 0; i < from.objectsCount; ++i) {
            dest.objects[i].type = from.objects[i].type;
            if (from.objects[i].type == 0) {    // Method
                copyMethod(dest.objects[i]._method, from.objects[i]._method);
            } else {    // Class
                copyClass(dest.objects[i]._class, from.objects[i]._class);
            }
        }

        // Copy meta info
        dest.meta.len = from.meta.len;
        dest.meta.table = new MetaInfo::_meta[from.meta.len];
        for (ui2 i = 0; i < from.meta.len; ++i) {
            auto &entry = dest.meta.table[i];
            entry.key.len = from.meta.table[i].key.len;
            entry.value.len = from.meta.table[i].value.len;
            entry.key.bytes = new ui1[entry.key.len];
            entry.value.bytes = new ui1[entry.value.len];
            memcpy(entry.key.bytes, from.meta.table[i].key.bytes, entry.key.len);
            memcpy(entry.value.bytes, from.meta.table[i].value.bytes, entry.value.len);
        }
    }

    // Helper function to free MetaInfo structure
    static void freeMetaInfo(MetaInfo &meta) {
        for (ui2 i = 0; i < meta.len; ++i) {
            delete[] meta.table[i].key.bytes;
            delete[] meta.table[i].value.bytes;
        }
        delete[] meta.table;
    }

    // Helper function to free MethodInfo structure
    static void freeMethod(MethodInfo &method) {
        delete[] method.typeParams;

        // Free args
        for (ui1 i = 0; i < method.argsCount; ++i) {
            freeMetaInfo(method.args[i].meta);
        }
        delete[] method.args;

        // Free locals
        for (ui2 i = 0; i < method.localsCount; ++i) {
            freeMetaInfo(method.locals[i].meta);
        }
        delete[] method.locals;

        delete[] method.code;

        // Free exception table
        for (ui2 i = 0; i < method.exceptionTableCount; ++i) {
            freeMetaInfo(method.exceptionTable[i].meta);
        }
        delete[] method.exceptionTable;

        // Free line info
        delete[] method.lineInfo.numbers;

        // Free lambdas recursively
        for (ui2 i = 0; i < method.lambdaCount; ++i) {
            freeMethod(method.lambdas[i]);
        }
        delete[] method.lambdas;

        // Free matches
        for (ui2 i = 0; i < method.matchCount; ++i) {
            delete[] method.matches[i].cases;
            freeMetaInfo(method.matches[i].meta);
        }
        delete[] method.matches;

        freeMetaInfo(method.meta);
    }

    // Helper function to free ClassInfo structure
    static void freeClass(ClassInfo &cls) {
        delete[] cls.typeParams;

        // Free fields
        for (ui2 i = 0; i < cls.fieldsCount; ++i) {
            freeMetaInfo(cls.fields[i].meta);
        }
        delete[] cls.fields;

        // Free methods
        for (ui2 i = 0; i < cls.methodsCount; ++i) {
            freeMethod(cls.methods[i]);
        }
        delete[] cls.methods;

        // Free nested objects
        for (ui2 i = 0; i < cls.objectsCount; ++i) {
            if (cls.objects[i].type == 0) {    // Method
                freeMethod(cls.objects[i]._method);
            } else {    // Class
                freeClass(cls.objects[i]._class);
            }
        }
        delete[] cls.objects;

        freeMetaInfo(cls.meta);
    }

    void freeElp(ElpInfo &elp) {
        // Free constant pool
        for (ui2 i = 0; i < elp.constantPoolCount; ++i) {
            if (elp.constantPool[i].tag == 0x06) {    // String
                delete[] elp.constantPool[i]._string.bytes;
            } else if (elp.constantPool[i].tag == 0x07) {    // Array
                delete[] elp.constantPool[i]._array.items;
            }
        }
        delete[] elp.constantPool;

        // Free globals
        for (ui2 i = 0; i < elp.globalsCount; ++i) {
            freeMetaInfo(elp.globals[i].meta);
        }
        delete[] elp.globals;

        // Free objects
        for (ui2 i = 0; i < elp.objectsCount; ++i) {
            if (elp.objects[i].type == 0) {    // Method
                freeMethod(elp.objects[i]._method);
            } else {    // Class
                freeClass(elp.objects[i]._class);
            }
        }
        delete[] elp.objects;

        // Free top-level meta info
        freeMetaInfo(elp.meta);
    }
}    // namespace spade