#include <argparse/argparse.hpp>
#include <cassert>
#include <iostream>
#include <fstream>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "utils/error.hpp"
#include "utils/error_printer.hpp"
#include "utils/graph.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "analyzer/scope_tree.hpp"
#include "analyzer/analyzer.hpp"
#include "utils/options.hpp"

void compile(const std::filesystem::path& file_path) {
    // change log pattern

    using namespace spadec;
    CompilerOptions compiler_options{
            .basic_module_path = fs::path("./spadec/res/basic.sp"),
            .import_search_dirs = {},
            .w_error = false,
    };
    ErrorPrinter error_printer;
    try {
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
    argparse::ArgumentParser program("spadec");
    // program.add_argument("-o", "--output").help("specifies the output filename").metavar("FILEPATH").default_value("");
    program.add_argument("input").required().remaining().nargs(1);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    compile(program.get("input"));
    // graph_test();
    // opcode_test();
    return 0;
}
