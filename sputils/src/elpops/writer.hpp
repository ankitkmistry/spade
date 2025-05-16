#pragma once

#include <fstream>
#include <filesystem>
#include <concepts>

#include "elpdef.hpp"

namespace spade
{
    class ElpWriter {
      private:
        std::filesystem::path path;
        std::ofstream file;

        void write8(uint8_t i) {
            file.put(*reinterpret_cast<char *>(&i));
        }

        void write16(uint16_t i) {
            write8(static_cast<uint8_t>(i >> 8));
            write8(static_cast<uint8_t>(i & 0xFF));
        }

        void write32(uint32_t i) {
            write16(static_cast<uint16_t>(i >> 16));
            write16(static_cast<uint16_t>(i & 0xFFFF));
        }

        void write64(uint64_t i) {
            write32(static_cast<uint32_t>(i >> 32));
            write32(static_cast<uint32_t>(i & 0xFFFFFFFF));
        }

        template<std::unsigned_integral T>
        void write(T value) {
            if constexpr (std::same_as<T, uint8_t>) {
                write8(static_cast<uint8_t>(value));
            } else if constexpr (std::same_as<T, uint16_t>) {
                write16(static_cast<uint16_t>(value));
            } else if constexpr (std::same_as<T, uint32_t>) {
                write32(static_cast<uint32_t>(value));
            } else if constexpr (std::same_as<T, uint64_t>) {
                write64(static_cast<uint64_t>(value));
            } else {
                static_assert(true, "T must be a valid unsigned integral type");
            }
        }

        void write(const ModuleInfo &info);
        void write(const ClassInfo &info);
        void write(const FieldInfo &info);
        void write(const MethodInfo &info);
        void write(const MatchInfo &info);
        void write(const LineInfo &info);
        void write(const ExceptionTableInfo &info);
        void write(const LocalInfo &info);
        void write(const ArgInfo &info);
        void write(const TypeParamInfo &info);
        void write(const GlobalInfo &info);
        void write(const MetaInfo &info);
        void write(const CpInfo &info);
        void write(const _Container &info);
        void write(const _UTF8 &info);

      public:
        explicit ElpWriter(const std::filesystem::path &file_path);

        /**
         * Writes the binary information given in the form of ElpInfo
         * in binary form which is readable by ElpReader to the file specified
         * during constructing the object
         * @param elp ELP information object
         */
        void write(const ElpInfo &elp);

        /**
         * Closes the file
         */
        void close();

        const std::filesystem::path &get_path() const {
            return path;
        }
    };
}    // namespace spade