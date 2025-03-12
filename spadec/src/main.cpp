#include <clocale>
#include <iostream>
#include <fstream>

#include "analyzer/analyzer.hpp"
#include "lexer/lexer.hpp"
#include "parser/import.hpp"
#include "parser/parser.hpp"
#include "parser/printer.hpp"
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

static void print_error(const CompilerError &err, const fs::path &path) {
    std::vector<string> lines;
    std::ifstream in(path);
    int max_digits = num_digits(err.get_line_end());
    std::cout << std::format("error [{}:{}]->[{}:{}]: {}\n", err.get_line_start(), err.get_col_start(), err.get_line_end(),
                             err.get_col_end(), err.what());
    std::cout << std::format("in file: {}\n", path.generic_string());
    for (int i = 1; !in.eof(); i++) {
        string line;
        std::getline(in, line);
        if (err.get_line_start() <= i && i <= err.get_line_end()) {
            std::cout << std::format("{: {}d} | {}\n", i, max_digits, line);
            std::cout << string(max_digits + 4, ' ');
            if (i == err.get_line_start() && i == err.get_line_end())
                for (int j = 1; j <= line.size(); ++j)
                    std::cout << (err.get_col_start() <= j && j <= err.get_col_end() ? '^' : ' ');
            else if (i == err.get_line_start())
                for (int j = 1; j <= line.size(); ++j) std::cout << (err.get_col_start() <= j ? '^' : ' ');
            else if (i == err.get_line_end())
                for (int j = 1; j <= line.size(); ++j) std::cout << (j <= err.get_col_end() ? '^' : ' ');
            else
                std::cout << string(line.size(), '^');
        }
    }
}

void compile() {
    fs::path file_path;
    try {
        file_path = R"(D:\Programming\Projects\spade\spadec\res\test.sp)";
        std::vector<std::shared_ptr<ast::Module>> modules;
        {
            std::ifstream in(file_path);
            if (!in) throw FileOpenError(file_path.string());
            std::stringstream buffer;
            buffer << in.rdbuf();
            Lexer lexer(buffer.str());
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
    } catch (const CompilerError &err) {
        print_error(err, file_path);
    }
}

void repl() {
    while (true) {
        std::stringstream code;
        std::cout << ">>> ";
        while (true) {
            if (!code.str().empty()) std::cout << "... ";
            string line;
            std::getline(std::cin, line);
            if (!line.empty() && line.back() == ';') {
                if (line.size() > 1) code << line.substr(0, line.size() - 1) << '\n';
                break;
            }
            code << line << '\n';
        }
        if (code.str() == "exit" || code.str() == "quit") return;
        Lexer lexer(code.str());
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
