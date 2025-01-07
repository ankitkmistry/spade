#pragma once

#include "common.hpp"

namespace err
{
    class CorruptFileError : public std::runtime_error {
      private:
        string path;

      public:
        explicit CorruptFileError(const string &path)
            : std::runtime_error(std::format("'{}' is corrupted", path)), path(path) {}

        string getPath() const { return path; }
    };

    class Unreachable : public std::runtime_error {
      public:
        explicit Unreachable() : std::runtime_error("unreachable code reached") {}
    };

    class FileNotFoundError : public std::runtime_error {
        string path;

      public:
        explicit FileNotFoundError(const string &path)
            : std::runtime_error(std::format("file not found: '{}'", path)), path(path) {}

        const string &getPath() const { return path; }
    };

    class SignatureError : public std::runtime_error {
      public:
        SignatureError(const string &sign, const string &msg)
            : std::runtime_error(std::format("invalid signature: {}: '{}'", msg, sign)) {}

        SignatureError(const string &sign)
            : std::runtime_error(std::format("invalid signature: '{}'", sign)) {}
    };
}    // namespace exceptions
