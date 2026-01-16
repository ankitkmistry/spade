#pragma once

#include "ee/vm.hpp"
#include "utils/common.hpp"
#include <asmjit/x86.h>

namespace spade
{
    class SWAN_EXPORT JitCompiler {
        SpadeVM *vm;
        asmjit::JitRuntime runtime;
        asmjit::FileLogger logger;

      public:
        JitCompiler(SpadeVM *vm) : vm(vm), logger(stdout) {
            logger.set_flags(asmjit::FormatFlags::kMachineCode | asmjit::FormatFlags::kShowAliases | asmjit::FormatFlags::kExplainImms);
            logger.set_indentation(asmjit::FormatIndentationGroup::kCode, 4);
        }

        JitCompiler() = delete;
        JitCompiler(const JitCompiler &) = delete;
        JitCompiler(JitCompiler &&) = delete;
        JitCompiler &operator=(const JitCompiler &) = delete;
        JitCompiler &operator=(JitCompiler &&) = delete;
        ~JitCompiler() = default;

        void compile_symbol(const ObjMethod *method);
        void compile_symbol(Obj *obj, const ObjMethod *method);

      private:
        void *assemble_symbol(const ObjMethod *method, bool has_self);
        void generate_body(asmjit::x86::Assembler &a, bool has_self, const asmjit::Label &exit);
    };
}    // namespace spade
