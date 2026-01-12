#pragma once

#include "utils/common.hpp"

namespace spade
{
    class SpadeVM;

    class SWAN_EXPORT Debugger {
      public:
        virtual ~Debugger() = default;

        virtual void init(const SpadeVM *vm) = 0;
        virtual void update(const SpadeVM *vm) = 0;
        virtual void cleanup(const SpadeVM *vm) = 0;
    };
}    // namespace spade
