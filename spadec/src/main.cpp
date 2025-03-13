#include <clocale>
#include <filesystem>
#include <iostream>
#include <fstream>

#include "analyzer/analyzer.hpp"
#include "lexer/lexer.hpp"
#include "parser/import.hpp"
#include "parser/parser.hpp"
#include "parser/printer.hpp"
#include "spimp/utils.hpp"
#include "utils/error.hpp"

using namespace spade;

static int num_digits(int x) {
    x = abs(x);
    return x < 10           ? 1
           : x < 100        ? 2
           : x < 1000       ? 3
           : x < 10000      ? 4
           : x < 100000     ? 5
           : x < 1000000    ? 6
           : x < 10000000   ? 7
           : x < 100000000  ? 8
           : x < 1000000000 ? 9
                            : 10;
}

static void print_code(const fs::path &path, const CompilerError &err, bool underline, char underline_char, int max_lines) {
    std::ifstream in(path);
    int max_digits = num_digits(err.get_line_end());

    int num_lines = err.get_line_end() - err.get_line_start() + 1;
    bool snip = num_lines > max_lines;
    int snip_start = -1, snip_end = -1;
    if (snip) {
        snip_start = err.get_line_start() + std::floor(max_lines / 2);
        snip_end = err.get_line_end() - std::ceil(max_lines / 2);
    }

    string line;
    for (int lineno = 1; !in.eof(); lineno++) {
        std::getline(in, line);
        if (err.get_line_start() <= lineno && lineno <= err.get_line_end()) {
            if (snip && snip_start <= lineno && lineno <= snip_end) {
                std::cout << std::format(" {} | ... <snipped {} lines of code> ...\n", string(max_digits, ' '),
                                         snip_end - snip_start + 1);
                for (; lineno <= snip_end; lineno++) std::getline(in, line);    // snip
            }

            std::cout << std::format(" {} | {}\n", pad_right(std::to_string(lineno), max_digits), line);
            if (underline) {
                std::cout << std::format(" {} | ", string(max_digits, ' '));

                if (lineno == err.get_line_start() && lineno == err.get_line_end()) {
                    for (int col = 1; col <= line.size(); ++col)
                        if (err.get_col_start() <= col && col <= err.get_col_end())
                            std::cout << (isspace(line[col - 1]) ? line[col - 1] : underline_char);
                        else
                            std::cout << (isspace(line[col - 1]) ? line[col - 1] : ' ');
                } else if (lineno == err.get_line_start()) {
                    for (int col = 1; col <= line.size(); ++col)
                        if (err.get_col_start() <= col)
                            std::cout << (isspace(line[col - 1]) ? line[col - 1] : underline_char);
                        else
                            std::cout << (isspace(line[col - 1]) ? line[col - 1] : ' ');
                } else if (lineno == err.get_line_end()) {
                    for (int col = 1; col <= line.size(); ++col)
                        if (col <= err.get_col_end())
                            std::cout << (isspace(line[col - 1]) ? line[col - 1] : underline_char);
                        else
                            std::cout << (isspace(line[col - 1]) ? line[col - 1] : ' ');
                } else {
                    for (int col = 1; col <= line.size(); ++col) {
                        std::cout << (isspace(line[col - 1]) ? line[col - 1] : underline_char);
                    }
                }
                std::cout << '\n';
            }
        }
    }
}

static void print_error(ErrorType type, const CompilerError &err) {
    std::vector<string> lines;
    fs::path path = err.get_file_path();
    string error_type_str;
    bool underline;
    char underline_char;
    switch (type) {
        case ErrorType::ERROR:
            error_type_str = "error";
            underline = true;
            underline_char = '^';
            break;
        case ErrorType::WARNING:
            error_type_str = "warning";
            underline = true;
            underline_char = '~';
            break;
        case ErrorType::NOTE:
            error_type_str = "note";
            underline = false;
            underline_char = ' ';
            break;
    }
    std::cout << std::format("{} [{}:{}]->[{}:{}]: {}\n", error_type_str, err.get_line_start(), err.get_col_start(),
                             err.get_line_end(), err.get_col_end(), err.what());
    std::cout << std::format("in file: {}\n", path.generic_string());
    print_code(path, err, underline, underline_char, 6);
}

void compile() {
    fs::path file_path;
    try {
        file_path = R"(D:\Programming\Projects\spade\spadec\res\test.sp)";
        std::vector<std::shared_ptr<ast::Module>> modules;
        {
            std::ifstream in(file_path);
            if (!in)
                throw FileOpenError(file_path.string());
            std::stringstream buffer;
            buffer << in.rdbuf();
            Lexer lexer(file_path, buffer.str());
            Parser parser(file_path, &lexer);
            auto tree = parser.parse();
            ImportResolver resolver(file_path.parent_path(), tree);
            modules = resolver.resolve_imports();
        }
        {
            Analyzer analyzer;
            analyzer.analyze(modules);
        }
        // for (const auto &module: modules) {
        //     ast::Printer printer{module};
        //     std::cout << printer << '\n';
        // }
    } catch (const ErrorGroup<AnalyzerError> err_grp) {
        for (const auto &[type, err]: err_grp.get_errors()) {
            print_error(type, err);
        }
    } catch (const CompilerError &err) {
        print_error(ErrorType::ERROR, err);
    }
}

void repl() {
    while (true) {
        std::stringstream code;
        std::cout << ">>> ";
        while (true) {
            if (!code.str().empty())
                std::cout << "... ";
            string line;
            std::getline(std::cin, line);
            if (!line.empty() && line.back() == ';') {
                if (line.size() > 1)
                    code << line.substr(0, line.size() - 1) << '\n';
                break;
            }
            code << line << '\n';
        }
        if (code.str() == "exit" || code.str() == "quit")
            return;
        Lexer lexer("", code.str());
        Parser parser("", &lexer);
        auto tree = parser.parse();
        ast::Printer printer{tree};
        std::cout << printer;
    }
}

int main() {
    try {
        std::setlocale(LC_CTYPE, ".UTF-8");
        compile();
        // repl();
    } catch (const CompilerError &err) {
        std::cerr << std::format("error [{}:{}]: {}\n", err.get_line_start(), err.get_col_start(), err.what());
    }
    return 0;
}
