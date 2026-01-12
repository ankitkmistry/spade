#pragma once

#include "common.hpp"
#include "spimp/error.hpp"

namespace spade
{
    class SWAN_EXPORT RuntimeError : public SpadeError {
      public:
        explicit RuntimeError(const string &arg) : SpadeError(arg) {}
    };

    class Obj;

    class ThrowSignal : public RuntimeError {
      private:
        Obj *value;

      public:
        explicit ThrowSignal(Obj *value) : RuntimeError("value is thrown in the vm"), value(value) {}

        Obj *get_value() const {
            return value;
        }
    };

    class SWAN_EXPORT FatalError : public SpadeError {
      public:
        explicit FatalError(const string &message) : SpadeError(message) {}
    };

    class SWAN_EXPORT MemoryError : public FatalError {
      public:
        explicit MemoryError(size_t size) : FatalError(std::format("failed to allocate memory: {} bytes", size)) {}
    };

    class SWAN_EXPORT IllegalAccessError : public FatalError {
      public:
        explicit IllegalAccessError(const string &message) : FatalError(message) {}
    };

    class SWAN_EXPORT IndexError : public IllegalAccessError {
      public:
        explicit IndexError(size_t index) : IllegalAccessError(std::format("index out of bounds: {}", index)) {}

        explicit IndexError(const string &index_of, size_t index)
            : IllegalAccessError(std::format("index out of bounds: {} ({})", index, index_of)) {}
    };

    class SWAN_EXPORT IllegalTypeParamAccessError : public FatalError {
      public:
        explicit IllegalTypeParamAccessError(const string &sign) : FatalError(std::format("tried to access empty type parameter: '{}'", sign)) {}
    };

    class SWAN_EXPORT NativeLibraryError : public FatalError {
      public:
        NativeLibraryError(const string &library, const string &msg) : FatalError(std::format("in '{}': {}", library, msg)) {}

        NativeLibraryError(const string &library, const string &function, const string &msg)
            : FatalError(std::format("function {} in '{}': {}", function, library, msg)) {}
    };

    class SWAN_EXPORT ForeignCallError : public FatalError {
      public:
        ForeignCallError(const string &sign, const string &msg) : FatalError(std::format("error calling foreign function: {}: {}", sign, msg)) {}
    };

    class SWAN_EXPORT StackOverflowError : public FatalError {
      public:
        explicit StackOverflowError() : FatalError("bad state: stack overflow") {}
    };

    class SWAN_EXPORT ArgumentError : public FatalError {
      public:
        ArgumentError(const string &sign, const string &msg) : FatalError(std::format("{}: {}", sign, msg)) {}
    };
}    // namespace spade
