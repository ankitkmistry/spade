#pragma once

#include "common.hpp"

namespace spasm
{
    class FileOpenError : public SpadeError {
      public:
        explicit FileOpenError(const string &filename) : SpadeError(std::format("failed to open file: {}", filename)) {}
    };

    class AssemblerError : public SpadeError {
        fs::path file_path;
        int line_start = -1;
        int col_start = -1;
        int line_end = -1;
        int col_end = -1;

      protected:
        AssemblerError() : SpadeError("") {}

      public:
        AssemblerError(const string &message, const fs::path &file_path, int line_start, int col_start, int line_end, int col_end)
            : SpadeError(message), file_path(file_path), line_start(line_start), col_start(col_start), line_end(line_end), col_end(col_end) {}

        AssemblerError(const string &message, const fs::path &file_path) : SpadeError(message), file_path(file_path) {}

        bool has_no_location() const {
            return line_start == -1 || col_start == -1 || line_end == -1 || col_end == -1;
        }

        const fs::path &get_file_path() const {
            return file_path;
        }

        int get_line_start() const {
            return line_start;
        }

        int get_col_start() const {
            return col_start;
        }

        int get_line_end() const {
            return line_end;
        }

        int get_col_end() const {
            return col_end;
        }
    };

    class LexerError : public AssemblerError {
      public:
        LexerError(const string &msg, const fs::path &file_path, int line_start, int col_start, int line_end, int col_end)
            : AssemblerError(msg, file_path, line_start, col_start, line_end, col_end) {}

        LexerError(const string &msg, const fs::path &file_path, int line_start, int col_start)
            : AssemblerError(msg, file_path, line_start, col_start, line_start, col_start) {}
    };

    class ParserError : public AssemblerError {
      public:
        ParserError(const string &msg, const fs::path &file_path, int line_start, int col_start, int line_end, int col_end)
            : AssemblerError(msg, file_path, line_start, col_start, line_end, col_end) {}

        ParserError(const string &msg, const fs::path &file_path, int line_start, int col_start)
            : AssemblerError(msg, file_path, line_start, col_start, line_start, col_start) {}
    };

    // class ImportError : public AssemblerError {
    //   public:
    //     ImportError(const string &msg, const fs::path &file_path, const std::shared_ptr<ast::Import> &import)
    //         : AssemblerError(msg, file_path, import->get_line_start(), import->get_col_start(), import->get_line_end(), import->get_col_end()) {}
    // };

    // class AnalyzerError : public AssemblerError {
    //   public:
    //     template<ast::HasLineInfo T>
    //     AnalyzerError(const string &msg, const fs::path &file_path, T node)
    //         : AssemblerError(msg, file_path, node ? node->get_line_start() : -1, node ? node->get_col_start() : -1, node ? node->get_line_end() : -1,
    //                         node ? node->get_col_end() : -1) {}
    // };

    enum class ErrorType { ERROR, WARNING, NOTE };

    template<typename T>
        requires std::derived_from<T, AssemblerError>
    class ErrorGroup : public AssemblerError {
        std::vector<std::pair<ErrorType, T>> errors;

      public:
        ErrorGroup() : AssemblerError() {}

        template<typename... Args>
            requires(sizeof...(Args) > 0) && (std::same_as<Args, std::pair<ErrorType, T>> && ...)
        ErrorGroup(Args... args) : AssemblerError() {
            (errors.emplace_back(args), ...);
        }

        ErrorGroup<T> &error(const T &err) {
            errors.emplace_back(ErrorType::ERROR, err);
            return *this;
        }

        ErrorGroup<T> &warning(const T &err) {
            errors.emplace_back(ErrorType::WARNING, err);
            return *this;
        }

        ErrorGroup<T> &note(const T &err) {
            errors.emplace_back(ErrorType::NOTE, err);
            return *this;
        }

        ErrorGroup<T> &extend(const ErrorGroup<T> &other) {
            extend_vec(errors, other.errors);
            return *this;
        }

        const std::vector<std::pair<ErrorType, T>> &get_errors() const {
            return errors;
        }

        std::vector<std::pair<ErrorType, T>> &get_errors() {
            return errors;
        }
    };
}    // namespace spasm
