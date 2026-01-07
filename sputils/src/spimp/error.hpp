#pragma once

#include "spimp/common.hpp"
#include <source_location>
#include <stdexcept>
#include <format>

namespace spade
{
    /**
     * The base error class
     */
    class SpadeError : public std::runtime_error {
      public:
        explicit SpadeError(const string &msg) : runtime_error(msg) {}
    };

    class CastError : public SpadeError {
        string from, to;

      public:
        CastError(const string &from, const string &to)
            : SpadeError(std::format("cannot cast type '{}' to type '{}'", from, to)), from(from), to(to) {}

        const string &get_from() const {
            return from;
        }

        const string &get_to() const {
            return to;
        }
    };

    class CorruptFileError : public SpadeError {
        string path;

      public:
        explicit CorruptFileError(const string &path) : SpadeError(std::format("'{}' is corrupted", path)), path(path) {}

        string getPath() const {
            return path;
        }
    };

    class Unreachable : public SpadeError {
      public:
        explicit Unreachable(const std::source_location location = std::source_location::current())
            : SpadeError(std::format("unreachable code reached at {}:{}:{}", location.file_name(), location.line(), location.column())) {}
    };

    class FileNotFoundError : public SpadeError {
        string path;

      public:
        explicit FileNotFoundError(const string &path) : SpadeError(std::format("file not found: '{}'", path)), path(path) {}

        const string &getPath() const {
            return path;
        }
    };

    class SignatureError : public SpadeError {
      public:
        SignatureError(const string &sign, const string &msg) : SpadeError(std::format("invalid signature: {}: '{}'", msg, sign)) {}

        SignatureError(const string &sign) : SpadeError(std::format("invalid signature: '{}'", sign)) {}
    };
}    // namespace spade
