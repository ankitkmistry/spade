#include "color.hpp"

#include <boost/predef.h>

#define MAGIC_OFF

namespace color
{
    static void print(const std::string &str)
#if defined(BOOST_OS_WINDOWS) || defined(BOOST_OS_LINUX)
            ;
#else
    {
        std::fwrite(str.c_str(), sizeof(std::string::value_type), str.size(), stdout);
    }
#endif

    void Console::style(const Style &style) {
#ifndef MAGIC_OFF
        // Refer to https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
        // Set attributes
        print(color::attr(style.attributes));
        // Set foreground color
        print(color::fg(style.fg_color));
        // Set background color
        print(color::bg(style.bg_color));
#endif
    }

    void Console::gotoxy(size_t x, size_t y) {
#ifndef MAGIC_OFF
        print("\x1b[" + std::to_string(y + 1) + ";" + std::to_string(x + 1) + "f");
#endif
    }

    void Console::set_cell(size_t x, size_t y, wchar_t value, Style style) {
#ifndef MAGIC_OFF
        gotoxy(x, y);
        Console::style(style);
        std::fputwc(value, stdout);
#endif
    }
}    // namespace color

// Windows implementation
#if defined(_WIN32)
#    include <windows.h>

namespace color
{
    static void print(const std::string &str) {
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), str.c_str(), str.size(), nullptr, nullptr);
    }

    /**
         * @brief Retrieves the last error message from the system.
         *
         * This function uses the FormatMessage function to get the last error message
         * from the system and returns it as a ConsoleError object.
         *
         * @return ConsoleError The last error message.
         */
    static std::runtime_error _color_win_get_last_error() {
        LPVOID err_msg_buf;
        DWORD n = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER              // Allocates a buffer for the message
                                         | FORMAT_MESSAGE_FROM_SYSTEM        // Searches the system message table
                                         | FORMAT_MESSAGE_IGNORE_INSERTS,    // Ignores insert sequences in the message definition.
                                 NULL,                                       // Handle to the module containing the message table
                                 GetLastError(),                             // Error code to format
                                 0,                                          // Default language
                                 reinterpret_cast<LPSTR>(&err_msg_buf),      // Output buffer for the formatted message
                                 0,                                          // Minimum size of the output buffer
                                 NULL);                                      // No arguments for insert sequences
        // Create a new buffer
        auto buf = std::make_unique<char[]>(n);
        std::memcpy(buf.get(), err_msg_buf, n);
        // Free the old buffer
        LocalFree(err_msg_buf);
        // Build the string
        std::string msg_str{buf.get(), n};
        return std::runtime_error(msg_str);
    }

    bool Console::is_terminal_open() {
        HANDLE h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode;
        return GetConsoleMode(h_stdout, &mode);
    }

    static DWORD old_out_mode;
    static UINT old_console_cp = 0;
    static char *old_locale = nullptr;

    void Console::init() {
#    ifndef MAGIC_OFF
        const char *locale = std::setlocale(LC_CTYPE, nullptr);
        old_locale = new char[std::strlen(locale)];
        std::strcpy(old_locale, locale);
        std::setlocale(LC_CTYPE, "en_US.utf8");

        if (!GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &old_out_mode))
            throw _color_win_get_last_error();
        if ((old_console_cp = GetConsoleOutputCP()) == 0)
            throw _color_win_get_last_error();

        if (!SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE),
                            ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN))
            throw _color_win_get_last_error();
        if (!SetConsoleOutputCP(CP_UTF8))
            throw _color_win_get_last_error();
#    endif
    }

    void Console::restore() {
#    ifndef MAGIC_OFF
        std::setlocale(LC_CTYPE, old_locale);
        if (!SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), old_out_mode))
            throw _color_win_get_last_error();
        if (!SetConsoleOutputCP(old_console_cp))
            throw _color_win_get_last_error();
#    endif
    }

    std::pair<size_t, size_t> Console::size() {
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
            throw _color_win_get_last_error();
        int columns = info.srWindow.Right - info.srWindow.Left + 1;
        int rows = info.srWindow.Bottom - info.srWindow.Top + 1;
        return {rows, columns};
    }

    void Console::clear() {
#    ifndef MAGIC_OFF
        HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD coord_screen = {0, 0};    // Top-left corner
        DWORD chars_written;
        CONSOLE_SCREEN_BUFFER_INFO csbi;

        if (!GetConsoleScreenBufferInfo(h_console, &csbi))
            throw _color_win_get_last_error();
        DWORD con_size = csbi.dwSize.X * csbi.dwSize.Y;
        // Fill the entire screen with blanks.
        if (!FillConsoleOutputCharacter(h_console, ' ', con_size, coord_screen, &chars_written))
            throw _color_win_get_last_error();
        // Get the current text attribute.
        if (!GetConsoleScreenBufferInfo(h_console, &csbi))
            throw _color_win_get_last_error();
        // Set the buffer attributes.
        if (!FillConsoleOutputAttribute(h_console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, con_size, coord_screen, &chars_written))
            throw _color_win_get_last_error();
        // Put the cursor in the top left corner.
        SetConsoleCursorPosition(h_console, coord_screen);
#    endif
    }
}    // namespace color

#elif defined(__linux__)

#    include <cerrno>
#    include <filesystem>
#    include <unistd.h>
#    include <termios.h>
#    include <sys/ioctl.h>
#    include <linux/input.h>
#    include <fcntl.h>
#    include <sys/epoll.h>

#    include "spimp/error.hpp"

namespace color
{
    static void print(const std::string &str) {
        if (write(STDOUT_FILENO, str.c_str(), str.size()) < 0)
            throw std::runtime_error(std::format("failed to write to stdout: {}", std::strerror(errno)));
    }

    bool Console::is_terminal_open() {
        return isatty(STDOUT_FILENO);
    }

    void Console::init() {}

    void Console::restore() {}

    std::pair<size_t, size_t> Console::size() {
        winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return {w.ws_row, w.ws_col};
    }

    void Console::clear() {
        print("\033[2J\033[H");    // ANSI escape code to clear screen
    }
}    // namespace color
#endif
