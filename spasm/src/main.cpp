#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <argparse/argparse.hpp>

#include "utils/error.hpp"
#include "utils/error_printer.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

using namespace spasm;

// #define ENABLE_CMD_LINE

int main(int argc, char *argv[]) {
#ifdef ENABLE_CMD_LINE
    argparse::ArgumentParser program("spasm");
    program.add_argument("-o", "--output").help("specifies the output filename").metavar("FILEPATH").default_value("");
    program.add_argument("input-files").required().remaining().nargs(1, -1);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    fs::path file_path = program.get<vector<string>>("input-files")[0];
    fs::path output_path;
    if (program.get("-o").empty())
        output_path = file_path.parent_path() / (file_path.stem().string() + ".elp");
    else
        output_path = program.get("-o") + ".elp";
#endif

#ifndef ENABLE_CMD_LINE
    fs::path file_path(R"(D:\Programming\Projects\spade\spasm\res\hello.spa)");
    fs::path output_path = file_path.parent_path() / (file_path.stem().string() + ".elp");
#endif

    ErrorPrinter error_printer;
    try {
        std::ifstream in(file_path);
        if (!in)
            throw FileOpenError(file_path.string());
        std::stringstream ss;
        ss << in.rdbuf();
        Lexer lexer(file_path, ss.str());
        Parser parser(lexer);
        ElpInfo elp = parser.parse();
        ElpWriter writer(output_path);
        writer.write(elp);
        writer.close();
    } catch (const AssemblerError &err) {
        error_printer.print(ErrorType::ERROR, err);
    } catch (const SpadeError &err) {
        std::cerr << std::format("error occurred:\n    {}: {}\n", spade::cpp_demangle(typeid(err).name()), err.what());
    }
    return 0;
}