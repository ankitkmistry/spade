/**
 *
 * <strong>DEFINITIONS FOR THE ELP FILE FORMAT</strong>
 * <hr>
 * ELP stands for Executable or Linkable Program<br>
 * <br>
 * ELP is of two types :-
 * <oi>
 * <li>
 * .xp file - Executable Program<br>
 *      <em>Represents a program which as an entry point to start execution</em>
 * </li>
 * <li>
 * .sll file - Spade Linkable Library<br>
 *      <em>Represents a library which can be imported by other ELPs</em>
 * </li>
 * </ol>
 *
 */

#pragma once

#include "spimp/common.hpp"

namespace spade
{
    typedef uint8_t ui1;
    typedef uint16_t ui2;
    typedef uint32_t ui4;
    typedef uint64_t ui8;

    typedef ui2 cpidx;

    struct _UTF8 {
        ui2 len;
        ui1 *bytes;

        bool operator==(const _UTF8 &rhs) const;

        bool operator!=(const _UTF8 &rhs) const;
    };

    struct CpInfo;

    struct _Container {
        ui2 len;
        CpInfo *items;

        bool operator==(const _Container &rhs) const;

        bool operator!=(const _Container &rhs) const;
    };

    struct CpInfo {
        ui1 tag;

        union {
            ui4 _char;
            ui8 _int;
            ui8 _float;
            _UTF8 _string;
            _Container _array;
        };

        static CpInfo fromChar(uint32_t c);

        static CpInfo fromInt(int64_t i);

        static CpInfo fromFloat(double d);

        static CpInfo fromString(string s);

        static CpInfo fromArray(std::vector<CpInfo> v);

        bool operator==(const CpInfo &rhs) const;

        bool operator!=(const CpInfo &rhs) const;
    };

    struct MetaInfo {
        ui2 len;

        struct _meta {
            _UTF8 key;
            _UTF8 value;
        } *table;
    };

    struct GlobalInfo {
        ui1 flags;
        cpidx thisGlobal;
        cpidx type;
        MetaInfo meta;
    };

    struct FieldInfo {
        ui2 flags;
        cpidx thisField;
        cpidx type;
        MetaInfo meta;
    };

    struct TypeParamInfo {
        cpidx name;
    };

    struct MethodInfo {
        ui2 accessFlags;
        ui1 type;
        cpidx thisMethod;

        ui1 typeParamCount;
        TypeParamInfo *typeParams;

        ui1 argsCount;

        struct ArgInfo {
            cpidx thisArg;
            cpidx type;
            MetaInfo meta;
        } *args;

        ui2 localsCount;
        ui2 closureStart;

        struct LocalInfo {
            cpidx thisLocal;
            cpidx type;
            MetaInfo meta;
        } *locals;

        ui4 maxStack;
        ui4 codeCount;
        ui1 *code;

        ui2 exceptionTableCount;

        struct ExceptionTableInfo {
            ui4 startPc;
            ui4 endPc;
            ui4 targetPc;
            cpidx exception;
            MetaInfo meta;
        } *exceptionTable;

        struct LineInfo {
            ui2 numberCount;

            struct NumberInfo {
                ui1 times;
                ui4 lineno;
            } *numbers;
        } lineInfo;

        ui2 lambdaCount;
        MethodInfo *lambdas;

        ui2 matchCount;

        struct MatchInfo {
            ui2 caseCount;

            struct CaseInfo {
                cpidx value;
                ui4 location;
            } *cases;

            ui4 defaultLocation;
            MetaInfo meta;
        } *matches;

        MetaInfo meta;
    };

    struct ObjInfo;

    struct ClassInfo {
        ui1 type;
        ui2 accessFlags;
        cpidx thisClass;
        ui1 typeParamCount;
        TypeParamInfo *typeParams;
        cpidx supers;
        ui2 fieldsCount;
        FieldInfo *fields;
        ui2 methodsCount;
        MethodInfo *methods;
        ui2 objectsCount;
        ObjInfo *objects;
        MetaInfo meta;
    };

    struct ObjInfo {
        ui1 type;

        union {
            MethodInfo _method;
            ClassInfo _class;
        };
    };

    struct ElpInfo;

    void copyElp(ElpInfo &dest, const ElpInfo &from);
    void freeElp(ElpInfo &elp);

    struct ElpInfo {
        ui4 magic = 0;
        ui4 minorVersion = 0;
        ui4 majorVersion = 0;

        cpidx compiledFrom = 0;
        ui1 type = 0;
        cpidx thisModule = 0;

        cpidx init = 0;
        cpidx entry = 0;
        cpidx imports = 0;

        ui2 constantPoolCount = 0;
        CpInfo *constantPool = null;
        ui2 globalsCount = 0;
        GlobalInfo *globals = null;
        ui2 objectsCount = 0;
        ObjInfo *objects = null;
        MetaInfo meta;

        ElpInfo() {
            meta.len = 0;
            meta.table = null;
        }

        ElpInfo(const ElpInfo &other) {
            copyElp(*this, other);
        }

        ElpInfo(ElpInfo &&other) {
            copyElp(*this, std::move(other));
        }

        ElpInfo &operator=(const ElpInfo &other) {
            copyElp(*this, other);
            return *this;
        }

        ElpInfo &operator=(ElpInfo &&other) {
            copyElp(*this, std::move(other));
            return *this;
        }

        ~ElpInfo() {
            freeElp(*this);
        }
    };
}    // namespace spade