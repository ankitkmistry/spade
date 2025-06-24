#include <cassert>
#include <clocale>
#include <filesystem>
#include <format>
#include <ios>
#include <iostream>
#include <fstream>
#include <cpptrace/utils.hpp>
#include <cpptrace/from_current.hpp>
#include <cpptrace/formatting.hpp>

#include "utils/error.hpp"
#include "utils/error_printer.hpp"
#include "utils/graph.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "analyzer/scope_tree.hpp"
#include "analyzer/analyzer.hpp"
#include "utils/options.hpp"

constexpr const bool ENABLE_BACKTRACE_FILTER = false;

void compile() {
    using namespace spade;
    CompilerOptions compiler_options{
            //
            .basic_module_path = fs::path(R"(D:\Programming\Projects\spade\spadec\res\basic.sp)"),    //
            .import_search_dirs = {}                                                                  //
    };
    fs::path file_path;
    ErrorPrinter error_printer;
    try {
        file_path = R"(D:\Programming\Projects\spade\spadec\res\test.sp)";
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
    } catch (const ErrorGroup<ImportError> err_grp) {
        error_printer.print(err_grp);
    } catch (const ErrorGroup<AnalyzerError> err_grp) {
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

void opcode_test() {
    using namespace spade;
    std::cout << "num opcodes: " << OpcodeInfo::OPCODE_COUNT << "\n";
    std::cout << std::format("{}          {}   {}\n", "opcode", "p_cnt", "take");
    std::cout << "-------------------------------\n";
    for (const auto &opcode: OpcodeInfo::all_opcodes()) {
        assert(opcode == OpcodeInfo::from_string(OpcodeInfo::to_string(opcode)));
        std::cout << std::format("{:15} ({})\t{}\n", OpcodeInfo::to_string(opcode), OpcodeInfo::params_count(opcode),
                                 OpcodeInfo::take_from_const_pool(opcode));
    }
}

int main(int argc, char *argv[]) {
    namespace fs = std::filesystem;
    cpptrace::experimental::set_cache_mode(cpptrace::cache_mode::prioritize_memory);
    CPPTRACE_TRY {
        std::setlocale(LC_CTYPE, ".UTF-8");
        color::Console::init();
        // std::ofstream file("output.log");
        // LOGGER.set_file(file);
        // std::ios_base::sync_with_stdio(false);
        spade::LOGGER.set_format("[{4}] {5}");
        compile();
        // graph_test();
        // opcode_test();
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
                                             (file_path.parent_path().has_parent_path() && (src_path = file_path.parent_path().parent_path(),
                                                                                            file_path.parent_path().parent_path().stem() == "src")))
                                             return src_path.has_parent_path() && src_path.parent_path().stem() == "spadec";
                                     }
                                     return false;
                                 });
        std::cerr << std::format("exception occurred:\n    {}: {}\n", spade::cpp_demangle(typeid(err).name()), err.what());
        // cpptrace::from_current_exception().print();
        formatter.print(cpptrace::from_current_exception());
        return 1;
    }
}
