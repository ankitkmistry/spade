#pragma once

#include <asmjit/x86.h>
#include "asmjit/x86/x86compiler.h"
#include "asmjit/x86/x86operand.h"
#include "ee/vm.hpp"

namespace spade
{
    class JitCompiler {
        SpadeVM *vm;
        asmjit::JitRuntime runtime;

      public:
        JitCompiler(SpadeVM *vm) : vm(vm) {}

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
        void generate_body(asmjit::x86::Compiler &c, bool has_self, const asmjit::x86::Gp &thread, const asmjit::x86::Gp &self,
                           const asmjit::x86::Gp &ret, const vector<asmjit::x86::Gp> &args);
    };
}    // namespace spade
