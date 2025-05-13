#include <fstream>
#include <iostream>
#include <sstream>

#include "lexer/lexer.hpp"
#include "lexer/token.hpp"
#include "utils/error_printer.hpp"

using namespace spasm;

int main() {
    fs::path file_path(R"(D:\Programming\Projects\spade\spasm\res\hello.spa)");
    ErrorPrinter error_printer;
    try {
        std::ifstream in(file_path);
        if (!in)
            throw FileOpenError(file_path.string());
        std::stringstream ss;
        ss << in.rdbuf();
        Lexer lexer(file_path, ss.str());
        while (true) {
            auto token = lexer.next_token();
            std::cout << token->to_string() << std::endl;
            if (token->get_type() == spasm::TokenType::END_OF_FILE)
                break;
        }
    } catch (const CompilerError &err) {
        error_printer.print(ErrorType::ERROR, err);
    }
    return 0;
}