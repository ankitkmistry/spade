#include "error_printer.hpp"

namespace spade
{
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

    void ErrorPrinter::print(ErrorType type, const CompilerError &err) {
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
        if (err.has_no_location()) {
            std::cout << std::format("{}: {}\n", error_type_str, err.what());
            std::cout << std::format("in file: {}\n", path.generic_string());
        } else {
            std::cout << std::format("[{}:{}]->[{}:{}] {}: {}\n", err.get_line_start(), err.get_col_start(), err.get_line_end(),
                                     err.get_col_end(), error_type_str, err.what());
            std::cout << std::format("in file: {}\n", path.generic_string());
            print_code(path, err, underline, underline_char, 6);
        }
    }
}    // namespace spade