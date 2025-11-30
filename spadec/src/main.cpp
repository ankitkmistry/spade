#include <cassert>
#include <clocale>
#include <format>
#include <iostream>
#include <fstream>
#include <cpptrace/utils.hpp>
#include <cpptrace/from_current.hpp>
#include <cpptrace/formatting.hpp>
#include <spdlog/sinks/basic_file_sink.h>

#include "utils/error.hpp"
#include "utils/error_printer.hpp"
#include "utils/graph.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "analyzer/scope_tree.hpp"
#include "analyzer/analyzer.hpp"
#include "utils/options.hpp"

#define ENABLE_BACKTRACE_FILTER (false)

void compile() {
    // change log pattern

    using namespace spadec;
    CompilerOptions compiler_options{
            .basic_module_path = fs::path("./spadec/res/basic.sp"),
            .import_search_dirs = {},
            .w_error = false,
    };
    fs::path file_path;
    ErrorPrinter error_printer;
    try {
        file_path = "./spadec/res/test.sp";

        // Setup the log file
        std::ofstream log_out(file_path.string() + ".log");
        auto logger = spdlog::basic_logger_mt("spadec", file_path.string() + ".log");
        // logger->set_pattern("[%g:%#] [thread %t] [%l] %v");
        logger->set_pattern("[thread %t] [%l] %v");
        spdlog::set_default_logger(logger);
        spdlog::set_level(spdlog::level::debug);

        std::shared_ptr<scope::Module> module;
        {
            std::ifstream in(file_path);
            if (!in)
                throw FileOpenError(file_path.string());
            std::stringstream buffer;
            buffer << in.rdbuf();
            Lexer lexer(file_path, buffer.str());
            Parser parser(file_path, &lexer);
            auto tree = parser.parse();
            ScopeTreeBuilder builder(tree);
            module = builder.build();
            module->claim(tree);
        }
        {
            Analyzer analyzer(module, error_printer, compiler_options);
            analyzer.analyze();
        }
    } catch (const ErrorGroup<ImportError> &err_grp) {
        error_printer.print(err_grp);
    } catch (const ErrorGroup<AnalyzerError> &err_grp) {
        error_printer.print(err_grp);
    } catch (const CompilerError &err) {
        error_printer.print(ErrorType::ERROR, err);
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
    namespace fs = std::filesystem;
    cpptrace::experimental::set_cache_mode(cpptrace::cache_mode::prioritize_memory);
    CPPTRACE_TRY {
        std::setlocale(LC_CTYPE, ".UTF-8");
        color::Console::init();
        compile();
        // graph_test();
        // opcode_test();
        color::Console::restore();
        return 0;
    }
    CPPTRACE_CATCH(const std::exception &err) {
        const auto formatter =
                cpptrace::formatter()
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
                                     (src_path = file_path.parent_path().parent_path(), file_path.parent_path().parent_path().stem() == "src")))
                                    return src_path.has_parent_path() && src_path.parent_path().stem() == "spadec";
                            }
                            return false;
                        });
        std::cerr << std::format("exception occurred:\n    {}: {}\n", spade::cpp_demangle(typeid(err).name()), err.what());
        formatter.print(cpptrace::from_current_exception());
        return 1;
    }
}
