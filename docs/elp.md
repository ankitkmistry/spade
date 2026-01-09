# The ELP Format Specification

The Executable Linkable Program, or ELP in short, is the file format used by the Spade compiler to store compiled bytecode. It is also used by the Spade assembler to store assembled bytecode. The Spade Swan Virtual Machine uses this bytecode format to load programs dynamically at runtime.

This format uses a tree structure to store the symbol tree. The format is described in a C-like format which is easy to understand.

### `cpidx`
```c
typedef uint16_t cpidx;
```
It is a type alias to `uint16_t`. It is the index of the an item in the latest constant pool. The item is specified by a type.
The possible types are:
- `any` - Any kind of value
- `sign` - A valid signature encoded in a UTF-8 string
- `bool` - Boolean value, i.e., `true` or `false`
- `char` - UTF-8 character
- `int` - 64-bit integer
- `float` - 64-bit floating-point value
- `string` - UTF-8 string
- `array` - Array of values of type `any`
- `array<T>` - Array of values of type `T`

### struct `ElpInfo`
```c
struct ElpInfo {
    uint32_t magic;
    uint16_t major_version;
    uint16_t minor_version;
    _UTF8 entry;
    uint16_t imports_count;
    _UTF8 imports[imports_count];
    uint16_t modules_count;
    ModuleInfo modules[modules_count];
    MetaInfo meta;
};
```
Represents an ELP file. An ELP file contains only one ElpInfo, i.e. `ElpInfo` is all the information encoded in an ELP file.
- `magic` - The magic number of the file. The possible magic numbers are as follows:
    - `0xC0FFEEDE` : Executable
    - `0xDEADCAFE` : Library
- `major_version` - The major version of the file
- `minor_version` - The minor version of the file
- `entry` - Signature of the entry function of the file
- `imports_count` - Count of import list
- `imports` - External imports required by the file
- `modules_count` - Count of nested modules
- `modules` - List of nested modules
- `meta` - Meta information of the file

### struct `ModuleInfo`
```c
struct ModuleInfo {
    uint8_t kind;
    cpidx compiled_from;
    cpidx name;
    cpidx init;
    uint16_t globals_count;
    GlobalInfo globals[globals_count];
    uint16_t methods_count;
    MethodInfo methods[methods_count];
    uint16_t classes_count;
    ClassInfo classes[classes_count];
    uint16_t constant_pool_count;
    CpInfo constant_pool[constant_pool_count];
    uint16_t modules_count;
    ModuleInfo modules[modules_count];
    MetaInfo meta;
};
```
Represents a module.
- `kind` - The kind of the module. The possible values of `kind` are as follows:
    - `0x00` - Executable
    - `0x01` - Library
- `compiled_from` : `string` - Path of the file from which the module was compiled from
- `name` : `string` - Name of the module
- `init` : `sign` - Signature of the initializing function of the module
- `globals_count` - Count of globals in the module
- `globals` - Array of globals of length `globals_count`
- `methods_count` - Count of methods in the module
- `methods` - Array of methods of length `methods_count`
- `classes_count` - Count of classes in the module
- `classes` - Array of classes of length `classes_count`
- `constant_pool_count` - Count of constant pool items in the module
- `constant_pool` - Array of constant pool items of length `constant_pool_count`
- `modules_count` - Count of nested modules
- `modules` - Array of nested modules of length `modules_count`
- `meta` - Meta information of the module

### struct `GlobalInfo`
```c
struct GlobalInfo {
    uint16_t kind;
    uint16_t access_flags;
    cpidx name;
    MetaInfo meta;
};
```
Represents a global variable of a module.
- `kind` - Kind of global. The possible values of `kind` are as follows:
    - `0x00` - variable
    - `0x01` - constant
- `access_flags` - Access flags of the global
- `name` : `string` - Index to the constant pool containing the name of the global
- `meta` - Meta information of the global

### struct `MethodInfo`
```c
struct MethodInfo {
    uint8_t kind;
    uint16_t access_flags;
    cpidx name;
    uint8_t args_count;
    ArgInfo args[args_count];
    uint16_t locals_count;
    LocalInfo locals[locals_count];
    uint32_t stack_max;
    uint32_t code_count;
    uint8_t code[code_count];
    uint16_t exception_table_count;
    ExceptionTableInfo exception_table[exception_table_count];
    LineInfo line_info;
    uint16_t match_count;
    MatchInfo matches[match_count];
    MetaInfo meta;
};
```
Represents a method.
- `kind` - The kind of the method. The possible values of `kind` are as follows:
    - `0x00` - Function
    - `0x01` - Method
    - `0x02` - Constructor
- `access_flags` - Access flags of the method
- `name` : `string` - Name of the method
- `args_count` - Count of args in the method
- `args` - Array of args of length `args_count`
- `locals_count` - Count of locals in the method
- `locals` - Array of locals of length `locals_count`
- `stack_max` - Maximum size of stack
- `code_count` - Count of code array
- `code` - Array of bytecode instructions of length `code_count`
- `exception_table_count` - Count of exception table items in the method
- `exception_table` - Exception table of the method
- `line_info` - Line number info of the method
- `match_count` - Count of match table items
- `matches` - Match table of the method
- `meta` - Meta information of the method

### struct `ArgInfo`
```c
struct ArgInfo {
    uint16_t kind;
    MetaInfo meta;
};
```
Represents a argument of a function.
- `kind` - Kind of argument. The possible values of kind are as follows:
    - `0x00` - variable
    - `0x01` - constant
- `meta` - Meta information of the argument

### struct `LocalInfo`
```c
struct LocalInfo {
    uint16_t kind;
    MetaInfo meta;
};
```
Represents a local of a function.
- `kind` - Kind of local. The possible values of kind are as follows:
    - `0x00` - variable
    - `0x01` - constant
- `meta` - Meta information of the local

### struct `ExceptionTableInfo`
```c
struct ExceptionTableInfo {
    uint32_t start_pc;
    uint32_t end_pc;
    uint32_t target_pc;
    cpidx exception;
    MetaInfo meta;
};
```
Represents a single element of the exception table of a function.
- `start_pc` - Starting region of the exception catching mechanism
- `end_pc` - Ending region of the exception catching mechanism
- `target_pc` - The location to follow if exception is caught
- `exception` : `sign` - Index to the constant pool containing the signature of the exception
- `meta` - Meta information of the exception table element

### struct `NumberInfo` and struct `LineInfo`
```c
struct NumberInfo {
    uint8_t times;
    uint32_t lineno;
};

struct LineInfo {
    uint16_t number_count;
    NumberInfo numbers[numbers_count];
};
```
Line information is saved using run-length encoding. The `NumberInfo` struct represents a single 
element of the encoding and the `LineInfo` represents a list of `NumberInfo`s. It is important to
note that the sum of `times` of all `numbers` is always equal to `code_count`, which means that
source line information is present for every byte in the bytecode.
- struct `NumberInfo`
    - `times` - Number of times to repeat this number
    - `lineno` - Source line number to repeat
- struct `LineInfo`
    - `number_count` - Count of number list
    - `numbers` - Array of numbers of length `number_count`

### struct `MatchInfo`
```c
struct MatchInfo {
    uint16_t case_count;
    CaseInfo cases[case_count];
    uint32_t default_location;
    MetaInfo meta;
};
```
Represents a match statement.
- `case_count` - Count of cases
- `cases` - Array of cases of length `case_count`
- `default_location` - The absolute location in code where the vm will jump if none of 
                       the switch cases succeed
- `meta` - Meta information of the match statement

### struct `CaseInfo`
```c
struct CaseInfo {
    cpidx value;
    uint32_t location;
};
```
Represents a single switch case.
- `value` : `any` - The switch case will look for this constant for matching
- `location` - The absolute location in code where the vm will jump to if 
               this switch case succeeds

### struct `ClassInfo`
```c
struct ClassInfo {
    uint8_t kind;
    uint16_t access_flags;
    cpidx name;
    cpidx supers;
    uint16_t fields_count;
    FieldInfo fields[fields_count];
    uint16_t methods_count;
    MethodInfo methods[methods_count];
    MetaInfo meta;
};
```
Represents a class.
- `kind` - The kind of the class. The possible values of `kind` are as follows:
    - `0x00` - Class
    - `0x01` - Interface
    - `0x02` - Annotation
    - `0x03` - Enum
- `access_flags` - Access flags of the class
- `name` : `string` - Name of the class
- `supers` : `array<sign>` - List of the signatures of super classes
- `fields_count` - Count of fields in the class
- `fields` - Array of fields of length `fields_count`
- `methods_count` - Count of methods in the class
- `methods` - Array of methods of length `methods_count`
- `meta` - Meta information of the class

### struct `FieldInfo`
```c
struct FieldInfo {
    uint8_t kind;
    uint16_t access_flags;
    cpidx name;
    MetaInfo meta;
};
```
Represents a class field.
- `kind` - The kind of the field. The possible values of `kind` are as follows:
    - `0x00` - variable
    - `0x01` - constant
- `access_flags` - Access flags of the field
- `name` : `string` -  Name of the field
- `meta` - Meta information of the field

### struct `CpInfo`
```c
struct CpInfo {
    uint8_t tag;
    union {
         uint32_t _char;
         uint64_t _int;
         uint64_t _float;
         _UTF8 _string;
         _Container _array;
    };
};
```
Represents an constant value.
- `tag` - Tag value of the constant which specifies its kind. The possible values of tag are:
    - `0x00` - Specifies a `null` value and no value from the `union` is selected
    - `0x01` - Specifies a `true` value and no value from the `union` is selected
    - `0x02` - Specifies a `false` value and no value from the `union` is selected
    - `0x03` - Specifies a character and `_char` member from the `union` is selected
    - `0x04` - Specifies a integer and `_int` member from the `union` is selected
    - `0x05` - Specifies a float and `_float` member from the `union` is selected
    - `0x06` - Specifies a string and `_string` member from the `union` is selected
    - `0x07` - Specifies a array of constants and `_array` member from the `union` is selected
- `union` - An union of various values, thus there is only one valid value among the following
    - `_char` - a character
    - `_int` - an integer
    - `_float` - a floating point
    - `_string` - a UTF-8 string (or sequence of byes)
    - `_array` - a array of constants

### struct `_UTF8`
```c
struct _UTF8 {
    uint16_t len;
    uint8_t bytes[len];
};
```
Represents an UTF-8 byte sequence.
- `len` - Length of the byte sequence
- `bytes` - Array of bytes of length `len`

### struct `_Container`
```c
struct _Container {
    uint16_t len;
    CpInfo items[len];
};
```
Represents an array of constants.
- `len` - Length of the array
- `bytes` - Array of constants of length `len`

### struct `MetaInfo`
```c
struct MetaInfo {
    struct _Meta {
        _UTF8 key;
        _UTF8 value;
    };
    uint16_t len;
    _Meta table[len];
};
```
Represents meta information about different objects, stored in a key-value pair, where both the key and pair are UTF-8 strings.

- `struct _Meta` - a structure describing a UTF-8 key and value pair
    - `key` - Key as UTF-8 string
    - `value` - Value as UTF-8 string
- `len` - Number of key-value pairs
- `table` - Array of key-value pairs of length `len`
