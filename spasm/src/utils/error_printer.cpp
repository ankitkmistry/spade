#include "error_printer.hpp"
#include <iostream>

namespace spasm
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

    struct CodePrintInfo {
        string line_info_color = color::fg(color::White);
        bool underline;
        string underline_char;
        int max_lines;
    };

    static void print_code(const fs::path &path, const AssemblerError &err, CodePrintInfo info) {
        std::ifstream in(path);
        int max_digits = num_digits(err.get_line_end());

        int num_lines = err.get_line_end() - err.get_line_start() + 1;
        bool snip = num_lines > info.max_lines;
        int snip_start = -1, snip_end = -1;
        if (snip) {
            snip_start = err.get_line_start() + std::floor(info.max_lines / 2);
            snip_end = err.get_line_end() - std::ceil(info.max_lines / 2);
        }

        string line;
        string pipe = color::fg(color::from_hex(0x3b9c6c)) + "|" + color::attr(color::RESET);
        for (int lineno = 1; !in.eof(); lineno++) {
            std::getline(in, line);
            if (err.get_line_start() <= lineno && lineno <= err.get_line_end()) {
                if (snip && snip_start <= lineno && lineno <= snip_end) {
                    std::cout << std::format(" {} {} ... <snipped {} lines of code> ...\n", string(max_digits, ' '), pipe, snip_end - snip_start + 1);
                    for (; lineno <= snip_end; lineno++) std::getline(in, line);    // snip
                }

                auto line_str = info.line_info_color + pad_right(std::to_string(lineno), max_digits) + color::attr(color::RESET);
                std::cout << std::format(" {} {} {}\n", line_str, pipe, line);
                if (info.underline) {
                    std::cout << std::format(" {} {} ", string(max_digits, ' '), pipe);

                    if (lineno == err.get_line_start() && lineno == err.get_line_end()) {
                        for (int col = 1; col <= line.size(); ++col)
                            if (err.get_col_start() <= col && col <= err.get_col_end())
                                std::cout << (isspace(line[col - 1]) ? string(1, line[col - 1]) : info.underline_char);
                            else
                                std::cout << (isspace(line[col - 1]) ? line[col - 1] : ' ');
                    } else if (lineno == err.get_line_start()) {
                        for (int col = 1; col <= line.size(); ++col)
                            if (err.get_col_start() <= col)
                                std::cout << (isspace(line[col - 1]) ? string(1, line[col - 1]) : info.underline_char);
                            else
                                std::cout << (isspace(line[col - 1]) ? line[col - 1] : ' ');
                    } else if (lineno == err.get_line_end()) {
                        for (int col = 1; col <= line.size(); ++col)
                            if (col <= err.get_col_end())
                                std::cout << (isspace(line[col - 1]) ? string(1, line[col - 1]) : info.underline_char);
                            else
                                std::cout << (isspace(line[col - 1]) ? line[col - 1] : ' ');
                    } else {
                        for (int col = 1; col <= line.size(); ++col) {
                            std::cout << (isspace(line[col - 1]) ? string(1, line[col - 1]) : info.underline_char);
                        }
                    }
                    std::cout << '\n';
                }
            }
        }
    }

    void ErrorPrinter::print(ErrorType type, const AssemblerError &err) const {
        string err_str;
        bool quote_open = false;
        string err_what = err.what();
        for (auto c: err_what) {
            if (c == '\'') {
                if (!quote_open) {
                    err_str += color::fg(color::from_hex(0xd619e0));
                    err_str += color::attr(color::BOLD);
                    err_str += c;
                    quote_open = true;
                } else {
                    err_str += c;
                    err_str += color::attr(color::RESET);
                    quote_open = false;
                }
            } else
                err_str += c;
        }


        std::vector<string> lines;
        fs::path path = err.get_file_path();
        string error_type_str;
        CodePrintInfo info;
        info.max_lines = 6;

        switch (type) {
            case ErrorType::ERROR:
                error_type_str = color::fg(color::Red) + color::attr(color::BOLD) + "error" + color::attr(color::RESET);
                info.underline = true;
                info.underline_char = color::fg(color::from_hex(0xfe5455)) + '^' + color::attr(color::RESET);
                break;
            case ErrorType::WARNING:
                error_type_str = color::fg(color::Orange) + color::attr(color::BOLD) + "warning" + color::attr(color::RESET);
                info.underline = true;
                info.underline_char = color::fg(color::from_hex(0xffbd2a)) + '~' + color::attr(color::RESET);
                break;
            case ErrorType::NOTE:
                error_type_str = color::fg(color::from_hex(0x07acf2)) + color::attr(color::BOLD) + "note" + color::attr(color::RESET);
                info.underline = false;
                info.underline_char = ' ';
                break;
        }
        auto file_path = color::fg(color::from_hex(0x4e8ed3)) + path.generic_string();
        if (err.has_no_location()) {
            std::cout << std::format("{}: {}\n", error_type_str, err_str);
            std::cout << std::format("in file: {}{}\n", file_path, color::attr(color::RESET));
        } else {
            std::cout << std::format("[{}:{}]->[{}:{}] {}: {}\n", err.get_line_start(), err.get_col_start(), err.get_line_end(), err.get_col_end(),
                                     error_type_str, err_str);
            std::cout << std::format("in file: {}:{}:{}{}\n", file_path, err.get_line_start(), err.get_col_start(), color::attr(color::RESET));
            print_code(path, err, info);
        }
    }
}    // namespace spasm
