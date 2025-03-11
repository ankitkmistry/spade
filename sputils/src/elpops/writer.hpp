#pragma once

#include "reader.hpp"

namespace spade
{
    class ElpWriter {
      private:
        string path;
        std::ofstream file;

        void write(uint8_t i) {
            file.put(*reinterpret_cast<char *>(&i));
        }

        void write(uint16_t i) {
            write(static_cast<uint8_t>(i >> 8));
            write(static_cast<uint8_t>(i & 0xFF));
        }

        void write(uint32_t i) {
            write(static_cast<uint16_t>(i >> 16));
            write(static_cast<uint16_t>(i & 0xFFFF));
        }

        void write(uint64_t i) {
            write(static_cast<uint32_t>(i >> 32));
            write(static_cast<uint32_t>(i & 0xFFFFFFFF));
        }

        void write(const CpInfo &info);

        void write(_UTF8 utf);

        void write(_Container con);

        void write(const GlobalInfo &info);

        void write(const ObjInfo &info);

        void write(const MethodInfo &info);

        void write(MethodInfo::LineInfo line);

        void write(const MethodInfo::ArgInfo &info);

        void write(const MethodInfo::LocalInfo &info);

        void write(const MethodInfo::ExceptionTableInfo &info);

        void write(const MethodInfo::MatchInfo &info);

        void write(MethodInfo::MatchInfo::CaseInfo info);

        void write(const ClassInfo &info);

        void write(const FieldInfo &info);

        void write(TypeParamInfo info);

        void write(MetaInfo info);

      public:
        explicit ElpWriter(const string &filename);

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

        const string &get_path() const {
            return path;
        }
    };
}    // namespace spade