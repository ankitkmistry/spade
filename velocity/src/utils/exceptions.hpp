#pragma once

#include <sputils.hpp>

namespace spade
{
    class RuntimeError : public SpadeError {
      public:
        explicit RuntimeError(const string &arg) : SpadeError(arg) {}
    };

    class Obj;

    class ThrowSignal : public RuntimeError {
      private:
        Obj *value;

      public:
        explicit ThrowSignal(Obj *value) : RuntimeError("value is thrown in the vm"), value(value) {}

        Obj *getValue() const {
            return value;
        }
    };

    class FatalError : public SpadeError {
      public:
        explicit FatalError(const string &message) : SpadeError(message) {}
    };

    class MemoryError : public FatalError {
      public:
        explicit MemoryError(size_t size) : FatalError(std::format("failed to allocate memory: {} bytes", size)) {}
    };

    class IllegalAccessError : public FatalError {
      public:
        explicit IllegalAccessError(const string &message) : FatalError(message) {}
    };

    class IndexError : public IllegalAccessError {
      public:
        explicit IndexError(size_t index) : IllegalAccessError(std::format("index out of bounds: {}", index)) {}

        explicit IndexError(const string &index_of, size_t index)
            : IllegalAccessError(std::format("index out of bounds: {} ({})", index, index_of)) {}
    };

    class IllegalTypeParamAccessError : public FatalError {
      public:
        explicit IllegalTypeParamAccessError(const string &sign)
            : FatalError(std::format("tried to access empty type parameter: '{}'", sign)) {}
    };

    class NativeLibraryError : public FatalError {
      public:
        NativeLibraryError(const string &library, const string &msg) : FatalError(std::format("in '{}': {}", library, msg)) {}

        NativeLibraryError(const string &library, const string &function, const string &msg)
            : FatalError(std::format("function {} in '{}': {}", function, library, msg)) {}
    };

    class StackOverflowError : public FatalError {
      public:
        explicit StackOverflowError() : FatalError("bad state: stack overflow") {}
    };

    class ArgumentError : public FatalError {
      public:
        ArgumentError(const string &sign, const string &msg) : FatalError(std::format("{}: {}", sign, msg)) {}
    };
}    // namespace spade
