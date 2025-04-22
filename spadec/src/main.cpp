#include <clocale>
#include <filesystem>
#include <ios>
#include <iostream>
#include <fstream>
#include <cpptrace/utils.hpp>
#include <cpptrace/from_current.hpp>
#include <cpptrace/formatting.hpp>

#include "utils/color.hpp"
#include "utils/error.hpp"
#include "utils/error_printer.hpp"
#include "utils/graph.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "parser/import.hpp"
#include "parser/printer.hpp"
#include "analyzer/scope_tree.hpp"
#include "analyzer/analyzer.hpp"

constexpr bool ENABLE_BACKTRACE_FILTER = false;

using namespace spade;

void compile() {
    fs::path file_path;
    ErrorPrinter error_printer;
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
            ScopeTreeBuilder builder(modules);
            auto module_scopes = builder.build();
            Analyzer analyzer(module_scopes, error_printer);
            analyzer.analyze();
        }
        // for (const auto &module: modules) {
        //     ast::Printer printer{module};
        //     std::cout << printer << '\n';
        // }
    } catch (const ErrorGroup<AnalyzerError> err_grp) {
        error_printer.print(err_grp);
    } catch (const CompilerError &err) {
        error_printer.print(ErrorType::ERROR, err);
    }
}

void repl() {
    try {
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
    } catch (const CompilerError &err) {
        std::cerr << std::format("error [{}:{}]: {}\n", err.get_line_start(), err.get_col_start(), err.what());
    }
}

void graph_test() {
    DirectedGraph<int> graph;
    graph.insert_vertex(0);
    graph.insert_vertex(1);
    graph.insert_vertex(2);
    graph.insert_vertex(3);
    graph.insert_edge(0, 1);
    graph.insert_edge(0, 2);
    graph.insert_edge(1, 3);
    std::cout << "Vertices: ";
    for (const auto &vertex: graph.vertices()) {
        std::cout << vertex << " ";
    }
    std::cout << std::endl;
    std::cout << "Edges:\n";
    for (const auto &vertex: graph.vertices()) {
        for (const auto &edge: graph.edges(vertex)) {
            std::cout << edge.origin() << " -> " << edge.destination() << "\n";
        }
    }
}

int main(int argc, char *argv[]) {
    cpptrace::experimental::set_cache_mode(cpptrace::cache_mode::prioritize_memory);
    CPPTRACE_TRY {
        std::setlocale(LC_CTYPE, ".UTF-8");
        color::Console::init();
        // std::ofstream file("output.log");
        // LOGGER.set_file(file);
        // std::ios_base::sync_with_stdio(false);
        LOGGER.set_format("[{4}] {5}");
        compile();
        // repl();
        // graph_test();
        color::Console::restore();
        return 0;
    }
    CPPTRACE_CATCH(const std::exception &err) {
        auto formatter = cpptrace::formatter{}
                                 .colors(cpptrace::formatter::color_mode::automatic)
                                 .addresses(cpptrace::formatter::address_mode::object)
                                 .snippets(false)
                                 //  .filtered_frame_placeholders(false)
                                 .filter([](const cpptrace::stacktrace_frame &frame) -> bool {
                                     if (!ENABLE_BACKTRACE_FILTER)
                                         return true;
                                     auto file_path = fs::absolute(fs::path(frame.filename));
                                     fs::path src_path;
                                     if (file_path.has_parent_path()) {
                                         if ((src_path = file_path.parent_path(), file_path.parent_path().stem() == "src") ||
                                             (file_path.parent_path().has_parent_path() &&
                                              (src_path = file_path.parent_path().parent_path(),
                                               file_path.parent_path().parent_path().stem() == "src")))
                                             return src_path.has_parent_path() && src_path.parent_path().stem() == "spadec";
                                     }
                                     return false;
                                 });
        std::cerr << std::format("exception occurred:\n    {}: {}\n", cpp_demangle(typeid(err).name()), err.what());
        // cpptrace::from_current_exception().print();
        formatter.print(cpptrace::from_current_exception());
        return 1;
    }
}
