#pragma once

#include "spimp/common.hpp"

namespace spade
{
    typedef uint16_t cpidx;

    struct _UTF8 {
        // Length of bytes
        uint16_t len;
        // Sequence of bytes
        vector<uint8_t> bytes;

        bool operator==(const _UTF8 &rhs) const;
        bool operator!=(const _UTF8 &rhs) const;
    };

    struct CpInfo;

    struct _Container {
        // Count of items
        uint16_t len;
        // List of items (constants)
        vector<CpInfo> items;

        bool operator==(const _Container &rhs) const;
        bool operator!=(const _Container &rhs) const;
    };

    struct CpInfo {
        // Type of the constant pool
        // 0x00 : null
        // 0x01 : true
        // 0x02 : false
        // 0x03 : char
        // 0x04 : int
        // 0x05 : float
        // 0x06 : string
        // 0x07 : array
        uint8_t tag;

        // Any one of the members is initialized according to tag
        // union {
        uint32_t _char;
        uint64_t _int;
        uint64_t _float;
        _UTF8 _string;
        _Container _array;
        // };

        static CpInfo from_char(uint32_t c);
        static CpInfo from_int(int64_t i);
        static CpInfo from_float(double d);
        static CpInfo from_string(const string &str);
        static CpInfo from_array(const vector<CpInfo> &v);

        bool operator==(const CpInfo &rhs) const;
        bool operator!=(const CpInfo &rhs) const;
    };

    struct MetaInfo {
        // Size of the table
        uint16_t len;

        struct _Meta {
            // Key  of the table item
            _UTF8 key;
            // Value of the table item
            _UTF8 value;
        };

        // Meta information table
        vector<_Meta> table;
    };

    struct GlobalInfo {
        // The kind of the global
        uint16_t kind;
        // Access flags for the global
        uint16_t access_flags;
        // [string] Name of the global
        cpidx name;
        // [sign] Type signature of the global
        cpidx type;
        // Meta information of the global
        MetaInfo meta;
    };

    struct TypeParamInfo {
        // [string] Name of the type param
        cpidx name;
    };

    struct ArgInfo {
        // The kind of the arg
        // 0x00 : VAR
        // 0x01 : CONST
        uint16_t kind;
        // [string] Name of the arg
        cpidx name;
        // [sign] Type signature of the arg
        cpidx type;
        // Meta information of the arg
        MetaInfo meta;
    };

    struct LocalInfo {
        // The kind of the local
        // 0x00 : VAR
        // 0x01 : CONST
        uint16_t kind;
        // [string] Name of the local
        cpidx name;
        // [sign] Type signature of the local
        cpidx type;
        // Meta information of the local
        MetaInfo meta;
    };

    struct ExceptionTableInfo {
        // Starting region of the exception catching mechanism
        uint32_t start_pc;
        // Ending region of the exception catching mechanism
        uint32_t end_pc;
        // The location to follow if exception is caught
        uint32_t target_pc;
        // [sign] The type of the exception
        cpidx exception;
        // Meta information of the exception table item
        MetaInfo meta;
    };

    struct NumberInfo {
        // The times to repeat this number
        uint8_t times;
        // The number to repeat
        uint32_t lineno;
    };

    struct LineInfo {
        // Count of numbers
        uint16_t number_count;
        // List of numbers
        vector<NumberInfo> numbers;
    };

    struct CaseInfo {
        // [any] Value of the case
        cpidx value;
        // The location to follow if this cases succeeds
        uint32_t location;
    };

    struct MatchInfo {
        // Count of cases
        uint16_t case_count;
        // List of cases
        vector<CaseInfo> cases;
        // Default location to follow if matching fails
        uint32_t default_location;
        // Meta information of the match
        MetaInfo meta;
    };

    struct MethodInfo {
        // The kind of the method
        // 0x00 : INIT
        // 0x01 : METHOD
        uint8_t kind;
        // Access flags for the method
        uint16_t access_flags;
        // [string] Name of the method
        cpidx name;

        // Count of type params in the method
        uint8_t type_params_count;
        // List of type params
        vector<TypeParamInfo> type_params;

        // Count of args in the method
        uint8_t args_count;
        // List of args
        vector<ArgInfo> args;

        // Count of locals in the method
        uint16_t locals_count;
        // Starting index for closures in locals list
        uint16_t closure_start;
        // List of locals
        vector<LocalInfo> locals;

        // Maximum size for stack
        uint32_t stack_max;
        // Count for code array
        uint32_t code_count;
        // List of bytecode instructions
        vector<uint8_t> code;

        // Count of exception table items in the method
        uint16_t exception_table_count;
        // Exception table for the method
        vector<ExceptionTableInfo> exception_table;

        // Line number info for the method
        LineInfo line_info;

        // Count of match table items
        uint16_t match_count;
        // Match table for the method
        vector<MatchInfo> matches;

        // Meta information for the method
        MetaInfo meta;
    };

    struct FieldInfo {
        // The kind of the field
        // 0x00 : VAR
        // 0x01 : CONST
        uint8_t kind;
        // Access flags for the field
        uint16_t access_flags;
        // [string] Name of the field
        cpidx name;
        // [sign] Type signature of the field
        cpidx type;
        // Meta information of the field
        MetaInfo meta;
    };

    struct ClassInfo {
        // The kind of the class:
        // 0x00 : CLASS
        // 0x01 : INTERFACE
        // 0x02 : ANNOTATION
        // 0x03 : ENUM
        uint8_t kind;
        // Access flags for the class
        uint16_t access_flags;
        // [string] Name of the class
        cpidx name;
        // [array<sign>] List of the signatures of super classes
        cpidx supers;

        // Count of type params in the class
        uint8_t type_params_count;
        // List of type params
        vector<TypeParamInfo> type_params;

        // Count of fields in the class
        uint16_t fields_count;
        // List of fields
        vector<FieldInfo> fields;

        // Count of methods in the class
        uint16_t methods_count;
        // List of methods
        vector<MethodInfo> methods;

        // Meta information for the class
        MetaInfo meta;
    };

    struct ModuleInfo {
        // The kind of the module
        // 0x00 : EXECUTABLE
        // 0x01 : LIBRARY
        uint8_t kind = 0;
        // [string] Path of the file from which the module was compiled from
        cpidx compiled_from = 0;
        // [string] Name of the module
        cpidx name = 0;
        // [array<string>] Imports of the module
        // cpidx imports = 0;
        // [sign] Signature of the initializing function of the module
        cpidx init = 0;

        // Count of globals in the module
        uint16_t globals_count = 0;
        // List of globals
        vector<GlobalInfo> globals;

        // Count of methods in the module
        uint16_t methods_count = 0;
        // List of methods
        vector<MethodInfo> methods;

        // Count of classes in the module
        uint16_t classes_count = 0;
        // List of classes
        vector<ClassInfo> classes;

        // Count of constant pool items in the module
        uint16_t constant_pool_count = 0;
        // List of constant pool items
        vector<CpInfo> constant_pool;

        // Count of nested modules
        uint16_t modules_count = 0;
        // List of nested modules
        vector<ModuleInfo> modules;

        // Meta information for the module
        MetaInfo meta;
    };

    struct ElpInfo {
        // The magic number of the file
        uint32_t magic = 0;
        // The minor version of the file
        uint32_t minor_version = 0;
        // The major version of the file
        uint32_t major_version = 0;

        // [sign] Signature of the entry function of the file
        cpidx entry = 0;
        // [array<string>] External imports required by the file
        cpidx imports = 0;

        // Count of constant pool items in the (top level) file
        // required by `entry` and `imports`
        uint16_t constant_pool_count = 0;
        // List of constant pool items
        vector<CpInfo> constant_pool;

        // Count of nested modules
        uint16_t modules_count = 0;
        // List of nested modules
        vector<ModuleInfo> modules;

        // Meta information for the file
        MetaInfo meta;
    };
}    // namespace spade