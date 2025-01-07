#ifndef SOURCE_UTILS_EXCEPTIONS_HPP_
#define SOURCE_UTILS_EXCEPTIONS_HPP_

#include "common.hpp"

namespace spade {
    class FatalError : public std::runtime_error {
    public:
        explicit FatalError(const string&message)
                : std::runtime_error(message) {}
    };

    class RuntimeError : public std::runtime_error {
    public:
        explicit RuntimeError(const string&arg) : std::runtime_error(arg) {}
    };

    class Obj;

    class ThrowSignal : public RuntimeError {
    private:
        Obj *value;
    public:
        explicit ThrowSignal(Obj *value) : RuntimeError("value is thrown in the vm"), value(value) {}

        Obj *getValue() const { return value; }
    };


    class CorruptFileError : public FatalError {
    private:
        string path;

    public:
        explicit CorruptFileError(const string&path)
                : FatalError(std::format("'{}' is corrupted", path)), path(path) {}

        const string& getPath() const { return path; }
    };

    class CastError : public FatalError {
    private:
        string from, to;

    public:
        CastError(const string&from, const string&to)
                : FatalError(std::format("cannot cast type '{}' to type '{}'", from, to)),
                  from(from), to(to) {}

        const string&getFrom() const { return from; }

        const string&getTo() const { return to; }
    };

    class MemoryError : public FatalError {
    public:
        explicit MemoryError(size_t size)
                : FatalError(std::format("failed to allocate memory: {} bytes", size)) {}
    };

    class Unreachable : public FatalError {
    public:
        Unreachable() : FatalError("unreachable code reached") {}
    };

    class IllegalAccessError : public FatalError {
    public:
        explicit IllegalAccessError(const string&message)
                : FatalError(message) {}
    };

    class IndexError : public IllegalAccessError {
    public:
        explicit IndexError(size_t index)
                : IllegalAccessError(std::format("index out of bounds: {}", index)) {}

        explicit IndexError(const string& index_of, size_t index)
                : IllegalAccessError(std::format("index out of bounds: {} ({})", index, index_of)) {}
    };

    class IllegalTypeParamAccessError : public FatalError {
    public:
        explicit IllegalTypeParamAccessError(const string&sign)
                : FatalError(std::format("tried to access empty type parameter: '{}'", sign)) {}
    };

    class NativeLibraryError : public FatalError {
    public:
        NativeLibraryError(const string& library, const string& msg)
                : FatalError(std::format("in '{}': {}", library, msg)) {}

        NativeLibraryError(const string& library, const string& function, const string& msg)
                : FatalError(std::format("function {} in '{}': {}", function, library, msg)) {}
    };

    class StackOverflowError : public FatalError {
    public:
        explicit StackOverflowError()
                : FatalError("bad state: stack overflow") {}
    };

    class ArgumentError : public FatalError {
    public:
        ArgumentError(const string& sign, const string& msg)
                : FatalError(std::format("{}: {}", sign, msg)) {}
    };
}

#endif /* SOURCE_UTILS_EXCEPTIONS_HPP_ */
