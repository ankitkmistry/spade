#include <fstream>
#include <iostream>
#include <sstream>

#include "utils/error.hpp"
#include "utils/error_printer.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

using namespace spasm;

int main() {
    fs::path file_path(R"(D:\Programming\Projects\spade\spasm\res\hello.spa)");
    fs::path output_path = file_path.parent_path() / (file_path.stem().string() + ".elp");
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
        // ElpReader reader();
        ElpWriter writer(output_path);
        writer.write(elp);
        writer.close();
    } catch (const AssemblerError &err) {
        error_printer.print(ErrorType::ERROR, err);
    }
    return 0;
}