#pragma once

#include "callable/frame.hpp"
#include "ee/state.hpp"
#include "utils/common.hpp"

namespace spade
{
    class DebugOp {
      private:
        static void print_call_stack(const VMState *state);

        static void print_frame(const Frame *frame);

        static void print_stack(Obj **const stack, uint32 count);

        static void print_exceptions(const ExceptionTable &exceptions);

        static void print_code(const uint8 *code, const uint8 *ip, const uint32 codeCount, const vector<Obj *> &pool, LineNumberTable lineInfos);

        static void print_locals(const LocalsTable &locals);

        static void print_args(const ArgsTable &args);

        static void print_const_pool(const vector<Obj *> &pool);

      public:
        static void print_vm_state(const VMState *state);
    };
}    // namespace spade
