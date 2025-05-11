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
        cpidx this_global;
        cpidx type;
        MetaInfo meta;
    };

    struct FieldInfo {
        ui2 flags;
        cpidx this_field;
        cpidx type;
        MetaInfo meta;
    };

    struct TypeParamInfo {
        cpidx name;
    };

    struct MethodInfo {
        ui2 access_flags;
        ui1 type;
        cpidx this_method;

        ui1 type_param_count;
        TypeParamInfo *type_params;

        ui1 args_count;

        struct ArgInfo {
            cpidx this_arg;
            cpidx type;
            MetaInfo meta;
        } *args;

        ui2 locals_count;
        ui2 closure_start;

        struct LocalInfo {
            cpidx this_local;
            cpidx type;
            MetaInfo meta;
        } *locals;

        ui4 max_stack;
        ui4 code_count;
        ui1 *code;

        ui2 exception_table_count;

        struct ExceptionTableInfo {
            ui4 start_pc;
            ui4 end_pc;
            ui4 target_pc;
            cpidx exception;
            MetaInfo meta;
        } *exception_table;

        struct LineInfo {
            ui2 number_count;

            struct NumberInfo {
                ui1 times;
                ui4 lineno;
            } *numbers;
        } line_info;

        ui2 lambda_count;
        MethodInfo *lambdas;

        ui2 match_count;

        struct MatchInfo {
            ui2 case_count;

            struct CaseInfo {
                cpidx value;
                ui4 location;
            } *cases;

            ui4 default_location;
            MetaInfo meta;
        } *matches;

        MetaInfo meta;
    };

    struct ObjInfo;

    struct ClassInfo {
        ui1 type;
        ui2 access_flags;
        cpidx this_class;
        ui1 type_param_count;
        TypeParamInfo *type_params;
        cpidx supers;
        ui2 fields_count;
        FieldInfo *fields;
        ui2 methods_count;
        MethodInfo *methods;
        ui2 objects_count;
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
        ui4 minor_version = 0;
        ui4 major_version = 0;

        cpidx compiled_from = 0;
        ui1 type = 0;
        cpidx this_module = 0;

        cpidx init = 0;
        cpidx entry = 0;
        cpidx imports = 0;

        ui2 constant_pool_count = 0;
        CpInfo *constant_pool = null;
        ui2 globals_count = 0;
        GlobalInfo *globals = null;
        ui2 objects_count = 0;
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