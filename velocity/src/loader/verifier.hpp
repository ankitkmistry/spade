#pragma once

#include "../utils/common.hpp"

namespace spade
{
    /**
     * Represents the bytecode verifier
     */
    class Verifier {
      private:
        ElpInfo elp;
        string path;

        void check_module(const ModuleInfo &module);

        void check_class(const ClassInfo &klass, const uint16 cp_count);

        void check_field(const FieldInfo &field, const uint16 cp_count);

        void check_method(const MethodInfo &method, const uint16 cp_count);

        void check_arg(const ArgInfo &arg, const uint16 count);

        void check_local(const LocalInfo &local, const uint16 count);

        void check_exception(const ExceptionTableInfo &exception, const uint16 cp_count);

        void check_line(const LineInfo &line, const uint32 codeCount);

        void check_match(const MatchInfo &info, const uint32 codeCount, const uint16 cp_count);

        void check_global(const GlobalInfo &global, const uint16 count);

        void check_range(const uint32 i, const uint32 count);

        void check_cp(const CpInfo &info);

        CorruptFileError corrupt() const {
            return CorruptFileError(path);
        }

      public:
        Verifier(const ElpInfo &elp, const string path) : elp(elp), path(path) {}

        /**
         * This function verifies the bytecode for basic standards.
         * This function does not check syntax or semantics of the bytecode.
         * This function only verifies if the bytecode has maintained basic standards
         * for various values.
         */
        void verify();
    };
}    // namespace spade
