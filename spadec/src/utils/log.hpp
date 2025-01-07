#pragma once

#include <iostream>
#include <source_location>
#include <chrono>
#include "utils/common.hpp"

namespace spade
{
    constexpr int LOG_LEVEL_TRACE = 0;    // Trace
    constexpr int LOG_LEVEL_DEBUG = 1;    // Debug
    constexpr int LOG_LEVEL_INFO = 2;     // Info
    constexpr int LOG_LEVEL_WARN = 3;     // Warn
    constexpr int LOG_LEVEL_ERROR = 4;    // Error
    constexpr int LOG_LEVEL_FATAL = 5;    // Fatal

    class Log {
        int level;
        string message;
        std::source_location location;

      public:
        Log(int level, const string &message, const std::source_location &location)
            : level(level), message(message), location(location) {}

        int get_level() const {
            return level;
        }

        const string &get_message() const {
            return message;
        }

        const std::source_location &get_location() const {
            return location;
        }
    };

    class Logger {
      public:
        using FormatString = std::format_string<
                decltype(std::chrono::system_clock::now()), decltype(std::declval<std::source_location>().file_name()),
                decltype(std::declval<std::source_location>().line()), decltype(std::declval<std::source_location>().column()),
                string /* level */, decltype(std::declval<Log>().get_message())>;
        using Filter = std::function<bool(Log)>;

      private:
        std::ostream *file = &std::clog;
        FormatString format = "[{0}] [{1}] [{2}:{3}] [{4}] {5}";
        Filter filter = [](const Log &) { return true; };

        static string string_level(int level) {
            switch (level) {
                case LOG_LEVEL_TRACE:
                    return "TRACE";
                case LOG_LEVEL_DEBUG:
                    return "DEBUG";
                case LOG_LEVEL_INFO:
                    return "INFO";
                case LOG_LEVEL_WARN:
                    return "WARN";
                case LOG_LEVEL_ERROR:
                    return "ERROR";
                case LOG_LEVEL_FATAL:
                    return "FATAL";
                default:
                    return std::to_string(level);
            }
        }

      public:
        void log(int level, const std::string &message,
                 const std::source_location &location = std::source_location::current()) const {
            Log log(level, message, location);
            if (filter(log) && file && file->good()) {
                (*file) << std::format(format, std::chrono::system_clock::now(), log.get_location().file_name(),
                                       log.get_location().line(), log.get_location().column(), string_level(log.get_level()),
                                       log.get_message())
                        << "\n";
            }
        }

        void log_trace(const std::string &message,
                       const std::source_location &location = std::source_location::current()) const {
            log(LOG_LEVEL_TRACE, message, location);
        }

        void log_debug(const std::string &message,
                       const std::source_location &location = std::source_location::current()) const {
            log(LOG_LEVEL_DEBUG, message, location);
        }

        void log_info(const std::string &message,
                      const std::source_location &location = std::source_location::current()) const {
            log(LOG_LEVEL_INFO, message, location);
        }

        void log_warn(const std::string &message,
                      const std::source_location &location = std::source_location::current()) const {
            log(LOG_LEVEL_WARN, message, location);
        }

        void log_error(const std::string &message,
                       const std::source_location &location = std::source_location::current()) const {
            log(LOG_LEVEL_ERROR, message, location);
        }

        void log_fatal(const std::string &message,
                       const std::source_location &location = std::source_location::current()) const {
            log(LOG_LEVEL_FATAL, message, location);
        }

        void set_file(std::ostream &file) {
            this->file = &file;
        }

        std::ostream &get_file() const {
            return *file;
        }

        void set_format(const FormatString &format) {
            this->format = format;
        }

        const FormatString &get_format() const {
            return format;
        }

        void set_filter(const Filter &filter) {
            this->filter = filter;
        }

        const Filter &get_filter() const {
            return filter;
        }
    };

    extern const Logger LOGGER;
}    // namespace spade