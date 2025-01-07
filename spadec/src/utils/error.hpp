#ifndef KITE_ERROR_HPP
#define KITE_ERROR_HPP

#include <stdexcept>

#include "log.hpp"
#include "parser/ast.hpp"

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
      private:
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

    class FileOpenError : public SpadeError {
      public:
        explicit FileOpenError(const string &filename) : SpadeError(std::format("failed to open file: {}", filename)) {}
    };

    class CompilerError : public SpadeError {
        int line_start;
        int col_start;
        int line_end;
        int col_end;

      public:
        CompilerError(const string &message, int line_start, int col_start, int line_end, int col_end)
            : SpadeError(message), line_start(line_start), col_start(col_start), line_end(line_end), col_end(col_end) {}

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

    class LexerError : public CompilerError {
      public:
        LexerError(const string &msg, int line_start, int col_start, int line_end, int col_end)
            : CompilerError(msg, line_start, col_start, line_end, col_end) {}

        LexerError(const string &msg, int line_start, int col_start)
            : CompilerError(msg, line_start, col_start, line_start, col_start) {}
    };

    class ParserError : public CompilerError {
      public:
        ParserError(const string &msg, int line_start, int col_start, int line_end, int col_end)
            : CompilerError(msg, line_start, col_start, line_end, col_end) {}

        ParserError(const string &msg, int line_start, int col_start)
            : CompilerError(msg, line_start, col_start, line_start, col_start) {}
    };

    class ImportError : public CompilerError {
      public:
        explicit ImportError(const string &msg, const std::shared_ptr<ast::Import> &import)
            : CompilerError(msg, import->get_line_start(), import->get_col_start(), import->get_line_end(),
                            import->get_col_end()) {}
    };
}    // namespace spade

#endif    // KITE_ERROR_HPP
