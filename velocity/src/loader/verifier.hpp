#ifndef SOURCE_LOADER_VERIFIER_HPP_
#define SOURCE_LOADER_VERIFIER_HPP_

#include "../utils/common.hpp"
#include "../utils/exceptions.hpp"

namespace spade {
/**
 * Represents the bytecode verifier
 */
    class Verifier {
    private:
        const ElpInfo elp;
        const string path;

        void checkObj(ObjInfo object, uint16 cpCount);

        void checkClass(ClassInfo klass, uint16 cpCount);

        void checkField(FieldInfo field, uint16 cpCount);

        void checkMethod(MethodInfo method, uint16 cpCount);

        void checkArg(MethodInfo::ArgInfo arg, uint16 count);

        void checkLocal(MethodInfo::LocalInfo local, uint16 count);

        void checkException(MethodInfo::ExceptionTableInfo exception, uint16 cpCount);

        void checkLine(MethodInfo::LineInfo line, uint16 codeCount);

        void checkMatch(MethodInfo::MatchInfo info, uint32 codeCount, uint16 cpCount);

        void checkGlobal(GlobalInfo global, uint16 count);

        void checkRange(ui4 i, ui4 count);

        void checkCp(CpInfo info);

        CorruptFileError corrupt() const { return CorruptFileError(path); }

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
}

#endif /* SOURCE_LOADER_VERIFIER_HPP_ */
