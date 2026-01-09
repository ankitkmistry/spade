#pragma once

#include <fstream>
#include <filesystem>

#include "spimp/error.hpp"
#include "elpdef.hpp"

namespace spade
{
    class ElpReader {
      private:
        uint32_t index = 0;
        std::ifstream file;
        string path;

        ModuleInfo read_module_info();
        ClassInfo read_class_info();
        FieldInfo read_field_info();
        MethodInfo read_method_info();
        MatchInfo read_match_info();
        LineInfo read_line_info();
        ExceptionTableInfo read_exception_info();
        LocalInfo read_local_info();
        ArgInfo read_arg_info();
        GlobalInfo read_global_info();
        MetaInfo read_meta_info();
        CpInfo read_cp_info();
        _Container read_container();
        _UTF8 read_utf8();

        uint8_t read_byte() {
            index++;
            return static_cast<uint8_t>(file.get());
        }

        uint16_t read_short() {
            uint16_t a = read_byte();
            uint8_t b = read_byte();
            return a << 8 | b;
        }

        uint32_t read_int() {
            uint32_t a = read_short();
            uint16_t b = read_short();
            return a << 16 | b;
        }

        uint64_t read_long() {
            uint64_t a = read_int();
            uint32_t b = read_int();
            return a << 32 | b;
        }

        [[noreturn]] void corrupt_file_error() const {
            throw CorruptFileError(path);
        }

      public:
        explicit ElpReader(const std::filesystem::path &path) : file(path, std::ios::in | std::ios::binary), path(path.generic_string()) {
            if (!file)
                throw FileNotFoundError(path.string());
        }

        ElpReader(const ElpReader &other) = delete;
        ElpReader(ElpReader &&other) noexcept = default;
        ElpReader &operator=(const ElpReader &other) = delete;
        ElpReader &operator=(ElpReader &&other) noexcept = default;
        ~ElpReader() = default;

        /**
         * This function parses the file associated with this reader
         * and returns the bytecode data
         * @return The bytecode data in the form of ElpInfo
         */
        ElpInfo read();

        const string &get_path() const {
            return path;
        }
    };
}    // namespace spade
