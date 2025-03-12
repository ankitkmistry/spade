#pragma once

#include <sputils.hpp>
#include "parser/ast.hpp"

namespace spade
{
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
        ImportError(const string &msg, const std::shared_ptr<ast::Import> &import)
            : CompilerError(msg, import->get_line_start(), import->get_col_start(), import->get_line_end(),
                            import->get_col_end()) {}
    };

    class AnalyzerError : public CompilerError {
      public:
        template<ast::HasLineInfo T>
        AnalyzerError(const string &msg, T node)
            : CompilerError(msg, node->get_line_start(), node->get_col_start(), node->get_line_end(), node->get_col_end()) {}
    };
}    // namespace spade
