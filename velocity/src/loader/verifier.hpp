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

        void check_obj(ObjInfo object, uint16 cp_count);

        void check_class(ClassInfo klass, uint16 cp_count);

        void check_field(FieldInfo field, uint16 cp_count);

        void check_method(MethodInfo method, uint16 cp_count);

        void check_arg(MethodInfo::ArgInfo arg, uint16 count);

        void check_local(MethodInfo::LocalInfo local, uint16 count);

        void check_exception(MethodInfo::ExceptionTableInfo exception, uint16 cp_count);

        void check_line(MethodInfo::LineInfo line, uint16 codeCount);

        void check_match(MethodInfo::MatchInfo info, uint32 codeCount, uint16 cp_count);

        void check_global(GlobalInfo global, uint16 count);

        void check_range(ui4 i, ui4 count);

        void check_cp(CpInfo info);

        CorruptFileError corrupt() const {
            return CorruptFileError(path);
        }

      public:
        Verifier(const ElpInfo elp, const string path) : elp(elp), path(path) {}

        /**
         * This function verifies the bytecode for basic standards.
         * This function does not check syntax or semantics of the bytecode.
         * This function only verifies if the bytecode has maintained basic standards
         * for various values.
         */
        void verify();
    };
}    // namespace spade
