#pragma once

namespace spade
{
    class SpadeVM;

    class Debugger {
      public:
        virtual ~Debugger() = default;

        virtual void init(const SpadeVM *vm) = 0;
        virtual void update(const SpadeVM *vm) = 0;
        virtual void cleanup(const SpadeVM *vm) = 0;
    };
}    // namespace spade
