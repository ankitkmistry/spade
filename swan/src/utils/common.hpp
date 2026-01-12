#pragma once

#include <filesystem>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <variant>

#if defined _WIN32
#    define OS_WINDOWS
#    pragma warning(disable : 4996)
#    pragma warning(disable : 4291)
#elif defined(__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
#    define OS_LINUX
#elif defined __APPLE__
#    define OS_MAC
#else
#    define OS_OTHER
#endif

#if defined __GNUC__
#    define COMPILER_GCC
#elif defined _MSC_VER
#    define COMPILER_MSVC
#elif defined __clang__
#    define COMPILER_CLANG
#else
#    define COMPILER_OTHER
#endif

#define null (nullptr)

#ifdef COMPILER_MSVC
#define SWAN_EXPORT __declspec(dllexport)
#endif

#ifndef SWAN_EXPORT
#define SWAN_EXPORT
#endif

namespace spade
{
    namespace fs = std::filesystem;

    using std::string;

    template<class T>
    using Table = std::unordered_map<string, T>;

    template<typename E>
    class Error {
        E m_error;

      public:
        constexpr Error(E &&err) : m_error(err) {}

        constexpr Error(const Error &other) = default;
        constexpr Error(Error &&other) = default;

        constexpr const E &error() const & noexcept {
            return m_error;
        }

        constexpr E &error() & noexcept {
            return m_error;
        }

        constexpr const E &&error() const && noexcept {
            return m_error;
        }

        constexpr E &&error() && noexcept {
            return m_error;
        }
    };

    template<typename E>
    Error(E) -> Error<E>;

    template<typename T, typename E>
    class Result final {
        std::variant<T, Error<E>> m_result;

      public:
        using value_type = T;
        using error_type = E;

        // Constructors
        constexpr Result() = default;
        constexpr Result(const Result &other) = default;
        constexpr Result(Result &&other) = default;

        template<typename U = std::remove_cv_t<T>>
        constexpr explicit(!std::is_convertible_v<U, T>) Result(U &&value) : m_result(value) {}

        template<typename G = std::remove_cv_t<T>>
        constexpr explicit(!std::is_convertible_v<const G &, T>) Result(const Error<G> &error) : m_result(error) {}

        template<typename G = std::remove_cv_t<T>>
        constexpr explicit(!std::is_convertible_v<G, T>) Result(Error<G> &&error) : m_result(error) {}

        // Destructor
        ~Result() = default;

        // Assignment
        constexpr Result &operator=(const Result &other) = default;
        constexpr Result &operator=(Result &&other) = default;

        template<typename U = std::remove_cv_t<T>>
        constexpr Result &operator=(U &&value) {
            m_result.emplace<0>(value);
        }

        constexpr const T *operator->() const {
            return std::addressof(value());
        }

        constexpr T *operator->() {
            return std::addressof(value());
        }

        constexpr T &operator*() & {
            return value();
        }

        constexpr const T &operator*() const & {
            return value();
        }

        constexpr T &&operator*() && {
            return value();
        }

        constexpr const T &&operator*() const && {
            return value();
        }

        constexpr operator bool() const noexcept {
            return m_result.index() == 0;
        }

        constexpr bool has_value() const noexcept {
            return m_result.index() == 0;
        }

        constexpr T &value() & {
            return std::get<0>(m_result);
        }

        constexpr const T &value() const & {
            return std::get<0>(m_result);
        }

        constexpr T &&value() && {
            return std::get<0>(m_result);
        }

        constexpr const T &&value() const && {
            return std::get<0>(m_result);
        }

        template<typename U = std::remove_cv_t<T>>
        constexpr T value_or(U &&default_value) const & {
            return has_value() ? value() : static_cast<T>(std::forward<U>(default_value));
        }

        template<typename U = std::remove_cv_t<T>>
        constexpr T value_or(U &&default_value) && {
            return has_value() ? std::move(value()) : static_cast<T>(std::forward<U>(default_value));
        }

        constexpr E &error() & {
            return std::get<1>(m_result).error();
        }

        constexpr const E &error() const & {
            return std::get<1>(m_result).error();
        }

        constexpr E &&error() && {
            return std::get<1>(m_result).error();
        }

        constexpr const E &&error() const && {
            return std::get<1>(m_result).error();
        }

        template<typename U = std::remove_cv_t<T>>
        constexpr T error_or(U &&default_error) const & {
            return !has_value() ? error() : static_cast<T>(std::forward<U>(default_error));
        }

        template<typename U = std::remove_cv_t<T>>
        constexpr T error_or(U &&default_error) && {
            return !has_value() ? std::move(error()) : static_cast<T>(std::forward<U>(default_error));
        }
    };

    template<typename E>
    class Result<void, E> {
        std::optional<Error<E>> m_error;

      public:
        using value_type = void;
        using error_type = E;

        // Constructors
        constexpr Result() = default;
        constexpr Result(const Result &other) = default;
        constexpr Result(Result &&other) = default;

        template<typename G = std::remove_cv_t<void>>
        constexpr explicit(!std::is_convertible_v<const G &, void>) Result(const Error<G> &error) : m_error(error) {}

        template<typename G = std::remove_cv_t<void>>
        constexpr explicit(!std::is_convertible_v<G, void>) Result(Error<G> &&error) : m_error(error) {}

        // Destructor
        ~Result() = default;

        // Assignment
        constexpr Result &operator=(const Result &other) = default;
        constexpr Result &operator=(Result &&other) = default;

        constexpr operator bool() const noexcept {
            return has_value();
        }

        constexpr bool has_value() const noexcept {
            return !m_error.has_value();
        }

        constexpr E &error() & {
            return m_error.value();
        }

        constexpr const E &error() const & {
            return m_error.value();
        }

        constexpr E &&error() && {
            return m_error.value();
        }

        constexpr const E &&error() const && {
            return m_error.value();
        }

        template<typename U = std::remove_cv_t<void>>
        constexpr void error_or(U &&default_error) const & {
            return !has_value() ? error() : static_cast<void>(std::forward<U>(default_error));
        }

        template<typename U = std::remove_cv_t<void>>
        constexpr void error_or(U &&default_error) && {
            return !has_value() ? std::move(error()) : static_cast<void>(std::forward<U>(default_error));
        }
    };

    enum class ErrorKind {
        NATIVE_LIBRARY,
    };

    class SwanError final {
        ErrorKind kind;
        string message;

      public:
        SwanError() = default;

        SwanError(ErrorKind kind, const string &message) : kind(kind), message(message) {}

        SwanError(const SwanError &other) = default;
        SwanError(SwanError &&other) = default;
        SwanError &operator=(const SwanError &other) = default;
        SwanError &operator=(SwanError &&other) = default;
        ~SwanError() = default;
    };

    template<typename T>
    using SwanResult = Result<T, SwanError>;
}    // namespace spade
