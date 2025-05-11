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
        dest.access_flags = from.access_flags;
        dest.type = from.type;
        dest.this_method = from.this_method;

        // Copy type parameters
        dest.type_param_count = from.type_param_count;
        dest.type_params = new TypeParamInfo[from.type_param_count];
        memcpy(dest.type_params, from.type_params, from.type_param_count * sizeof(TypeParamInfo));

        // Copy arguments
        dest.args_count = from.args_count;
        dest.args = new MethodInfo::ArgInfo[from.args_count];
        for (ui1 i = 0; i < from.args_count; ++i) {
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
        dest.locals_count = from.locals_count;
        dest.closure_start = from.closure_start;
        dest.locals = new MethodInfo::LocalInfo[from.locals_count];
        for (ui2 i = 0; i < from.locals_count; ++i) {
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
        dest.max_stack = from.max_stack;
        dest.code_count = from.code_count;
        dest.code = new ui1[from.code_count];
        memcpy(dest.code, from.code, from.code_count);

        // Copy exception table
        dest.exception_table_count = from.exception_table_count;
        dest.exception_table = new MethodInfo::ExceptionTableInfo[from.exception_table_count];
        for (ui2 i = 0; i < from.exception_table_count; ++i) {
            dest.exception_table[i] = from.exception_table[i];
            // Deep copy meta info
            auto &meta = dest.exception_table[i].meta;
            meta.table = new MetaInfo::_meta[meta.len];
            for (ui2 j = 0; j < meta.len; ++j) {
                auto &entry = meta.table[j];
                entry.key.bytes = new ui1[entry.key.len];
                entry.value.bytes = new ui1[entry.value.len];
                memcpy(entry.key.bytes, from.exception_table[i].meta.table[j].key.bytes, entry.key.len);
                memcpy(entry.value.bytes, from.exception_table[i].meta.table[j].value.bytes, entry.value.len);
            }
        }

        // Copy line info
        dest.line_info.number_count = from.line_info.number_count;
        dest.line_info.numbers = new MethodInfo::LineInfo::NumberInfo[from.line_info.number_count];
        memcpy(dest.line_info.numbers, from.line_info.numbers, from.line_info.number_count * sizeof(MethodInfo::LineInfo::NumberInfo));

        // Copy lambdas
        dest.lambda_count = from.lambda_count;
        dest.lambdas = new MethodInfo[from.lambda_count];
        for (ui2 i = 0; i < from.lambda_count; ++i) {
            copyMethod(dest.lambdas[i], from.lambdas[i]);
        }

        // Copy matches
        dest.match_count = from.match_count;
        dest.matches = new MethodInfo::MatchInfo[from.match_count];
        for (ui2 i = 0; i < from.match_count; ++i) {
            auto &match = dest.matches[i];
            match.case_count = from.matches[i].case_count;
            match.cases = new MethodInfo::MatchInfo::CaseInfo[match.case_count];
            memcpy(match.cases, from.matches[i].cases, match.case_count * sizeof(MethodInfo::MatchInfo::CaseInfo));
            match.default_location = from.matches[i].default_location;

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
        dest.access_flags = from.access_flags;
        dest.this_class = from.this_class;

        // Copy type parameters
        dest.type_param_count = from.type_param_count;
        dest.type_params = new TypeParamInfo[from.type_param_count];
        memcpy(dest.type_params, from.type_params, from.type_param_count * sizeof(TypeParamInfo));

        dest.supers = from.supers;

        // Copy fields
        dest.fields_count = from.fields_count;
        dest.fields = new FieldInfo[from.fields_count];
        for (ui2 i = 0; i < from.fields_count; ++i) {
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
        dest.methods_count = from.methods_count;
        dest.methods = new MethodInfo[from.methods_count];
        for (ui2 i = 0; i < from.methods_count; ++i) {
            copyMethod(dest.methods[i], from.methods[i]);
        }

        // Copy objects
        dest.objects_count = from.objects_count;
        dest.objects = new ObjInfo[from.objects_count];
        for (ui2 i = 0; i < from.objects_count; ++i) {
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
        dest.minor_version = from.minor_version;
        dest.major_version = from.major_version;
        dest.compiled_from = from.compiled_from;
        dest.type = from.type;
        dest.this_module = from.this_module;
        dest.init = from.init;
        dest.entry = from.entry;
        dest.imports = from.imports;

        // Copy constant pool
        dest.constant_pool_count = from.constant_pool_count;
        dest.constant_pool = new CpInfo[from.constant_pool_count];
        for (ui2 i = 0; i < from.constant_pool_count; ++i) {
            dest.constant_pool[i] = from.constant_pool[i];
            // For string and array types, need to deep copy their contents
            if (from.constant_pool[i].tag == 0x06) {
                // String type
                auto &str = dest.constant_pool[i]._string;
                str.bytes = new ui1[str.len];
                memcpy(str.bytes, from.constant_pool[i]._string.bytes, str.len);
            } else if (from.constant_pool[i].tag == 0x07) {
                // Array type
                auto &arr = dest.constant_pool[i]._array;
                arr.items = new CpInfo[arr.len];
                for (ui2 j = 0; j < arr.len; ++j) {
                    arr.items[j] = from.constant_pool[i]._array.items[j];
                }
            }
        }

        // Copy globals
        dest.globals_count = from.globals_count;
        dest.globals = new GlobalInfo[from.globals_count];
        for (ui2 i = 0; i < from.globals_count; ++i) {
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
        dest.objects_count = from.objects_count;
        dest.objects = new ObjInfo[from.objects_count];
        for (ui2 i = 0; i < from.objects_count; ++i) {
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
        delete[] method.type_params;

        // Free args
        for (ui1 i = 0; i < method.args_count; ++i) {
            freeMetaInfo(method.args[i].meta);
        }
        delete[] method.args;

        // Free locals
        for (ui2 i = 0; i < method.locals_count; ++i) {
            freeMetaInfo(method.locals[i].meta);
        }
        delete[] method.locals;

        delete[] method.code;

        // Free exception table
        for (ui2 i = 0; i < method.exception_table_count; ++i) {
            freeMetaInfo(method.exception_table[i].meta);
        }
        delete[] method.exception_table;

        // Free line info
        delete[] method.line_info.numbers;

        // Free lambdas recursively
        for (ui2 i = 0; i < method.lambda_count; ++i) {
            freeMethod(method.lambdas[i]);
        }
        delete[] method.lambdas;

        // Free matches
        for (ui2 i = 0; i < method.match_count; ++i) {
            delete[] method.matches[i].cases;
            freeMetaInfo(method.matches[i].meta);
        }
        delete[] method.matches;

        freeMetaInfo(method.meta);
    }

    // Helper function to free ClassInfo structure
    static void freeClass(ClassInfo &cls) {
        delete[] cls.type_params;

        // Free fields
        for (ui2 i = 0; i < cls.fields_count; ++i) {
            freeMetaInfo(cls.fields[i].meta);
        }
        delete[] cls.fields;

        // Free methods
        for (ui2 i = 0; i < cls.methods_count; ++i) {
            freeMethod(cls.methods[i]);
        }
        delete[] cls.methods;

        // Free nested objects
        for (ui2 i = 0; i < cls.objects_count; ++i) {
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
        for (ui2 i = 0; i < elp.constant_pool_count; ++i) {
            if (elp.constant_pool[i].tag == 0x06) {    // String
                delete[] elp.constant_pool[i]._string.bytes;
            } else if (elp.constant_pool[i].tag == 0x07) {    // Array
                delete[] elp.constant_pool[i]._array.items;
            }
        }
        delete[] elp.constant_pool;

        // Free globals
        for (ui2 i = 0; i < elp.globals_count; ++i) {
            freeMetaInfo(elp.globals[i].meta);
        }
        delete[] elp.globals;

        // Free objects
        for (ui2 i = 0; i < elp.objects_count; ++i) {
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