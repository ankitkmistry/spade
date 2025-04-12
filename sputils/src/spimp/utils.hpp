#pragma once

#include "error.hpp"

#include <stdexcept>
#include <string>
#include <vector>
#include <memory>

namespace spade
{
    template<typename T>
    concept StringConvertible = requires(T t) {
        { t.to_string() } -> std::same_as<std::string>;
    };

    /**
     * @param str mangled name
     * @return the undecorated name of \p str
     */
    std::string cpp_demangle(std::string str);

    /**
     * Casts a value of nullable_type From to a value of nullable_type To.
     * @throws CastError if casting fails
     * @tparam To type of the value to be cast
     * @tparam From type of the cast value
     * @param val the value to be cast
     * @return the cast value
     */
    template<class To, class From>
    To *cast(From *val) {
        auto cast_val = dynamic_cast<To *>(val);
        if (cast_val == nullptr)
            throw CastError(cpp_demangle(typeid(From).name()), cpp_demangle(typeid(To).name()));
        return cast_val;
    }

    /**
     * Casts a shared pointer of nullable_type From to a shared pointer of nullable_type To.
     * @throws CastError if casting fails
     * @tparam To type of the shared pointer to be cast
     * @tparam From type of the cast shared pointer
     * @param val the shared pointer to be cast
     * @return the cast shared pointer
     */
    template<class To, class From>
    std::shared_ptr<To> cast(std::shared_ptr<From> val) {
        auto cast_val = std::dynamic_pointer_cast<To>(val);
        if (cast_val == nullptr)
            throw CastError(cpp_demangle(typeid(From).name()), cpp_demangle(typeid(To).name()));
        return cast_val;
    }

    /**
     * Checks if the nullable_type of key_char is a superclass of nullable_type V.
     * @tparam T compile time type of the value
     * @tparam V type for checking
     * @param obj the value to be checked
     * @return true if the type of value is a superclass of type V, false otherwise
     */
    template<class T, class V>
    bool is(V obj) {
        return dynamic_cast<T *>(obj) != nullptr;
    }

    /**
     * Checks if the nullable_type of key_char is a superclass of nullable_type V.
     * @tparam T compile time type of the value
     * @tparam V type for checking
     * @param obj the value to be checked
     * @return true if the type of value is a superclass of type V, false otherwise
     */
    template<class T, class V>
    bool is(std::shared_ptr<V> obj) {
        return std::dynamic_pointer_cast<T>(obj) != nullptr;
    }

    template<typename T>
    static std::vector<T> slice(const std::vector<T> &list, int64_t start, int64_t end) {
        if (start < 0)
            start += list.size();
        if (end < 0)
            end += list.size();
        if (start >= list.size() || end >= list.size())
            throw std::runtime_error("slice(3): index out of bounds");
        if (start > end)
            std::swap(start, end);
        std::vector<T> result;
        result.reserve(end - start);
        for (int64_t i = start; i < end; ++i) result.push_back(list[i]);
        return result;
    }

    std::string join(const std::vector<std::string> &list, const std::string &delimiter);

    /**
     * Converts a std::vector<T> to a comma separated list.
     * Assumes that values have a to_string() method that gives std::string representation of the value.
     * @tparam T type of the vector items
     * @param data the vector to be processed
     * @return the comma separated list as a std::string
     */
    template<StringConvertible T>
    std::string list_to_string(const std::vector<T> &data) {
        std::string str;
        const int length = data.size();
        for (uint16_t i = 0; i < length; ++i) str += data[i].to_string() + (i < length - 1 ? ", " : "");
        return str;
    }

    /**
     * Converts a std::vector<T*> to a comma separated list.
     * Assumes that values have a to_string() method that gives std::string representation of the value.
     * @tparam T type of the std::vectors
     * @param data the vector to be processed
     * @return the comma separated list as a std::string
     */
    template<StringConvertible T>
    std::string list_to_string(const std::vector<T *> &data) {
        std::string str;
        const int length = data.size();
        for (uint16_t i = 0; i < length; ++i) str += data[i]->to_string() + (i < length - 1 ? ", " : "");
        return str;
    }

    /**
     * Pads a std::string to the left with blank spaces.
     * @param str the std::string to be padded
     * @param length the number of spaces
     * @return the padded std::string
     */
    std::string pad_left(const std::string &str, size_t length);

    /**
     * Pads a std::string to the right with blank spaces.
     * @param str the std::string to be padded
     * @param length the number of spaces
     * @return the padded std::string
     */
    std::string pad_right(const std::string &str, size_t length);

    /**
     * Checks if the string is a number
     * @param s the string
     * @return true if s represents a number, false otherwise
     */
    bool is_number(const std::string &s);

    /**
     * Converts raw IEEE floating point 64 bit representation to a double
     * @param digits the representation
     * @return the converted double
     */
    double raw_to_double(uint64_t digits);

    /**
     * Converts a double to its IEEE floating point 64 bit representation
     * @param number the double
     * @return the raw representation
     */
    uint64_t double_to_raw(double number);

    /**
     * Converts an unsigned uint64 to signed int64.
     * This does not change any bits of the original number.
     * The raw bit representation remains unchanged
     * @param number the unsigned number
     * @return the signed number
     */
    int64_t unsigned_to_signed(uint64_t number);

    /**
     * Converts a signed int64 to unsigned int64.
     * This does not change any bits of the original number.
     * The raw bit representation remains unchanged
     * @param number the signed number
     * @return the unsigned number
     */
    uint64_t signed_to_unsigned(int64_t number);

    int32_t long_to_int(int64_t num);

    std::string get_absolute_path(const std::string &path);

    /**
    * Extends a vector by appending elements from another vector.
    * @tparam T type of the vector elements
    * @param dest destination vector to which elements will be appended
    * @param vec source vector from which elements will be copied
    */
    template<typename T>
    void extend_vec(std::vector<T> &dest, const std::vector<T> &vec) {
        // Check https://stackoverflow.com/a/64102335/17550173
        dest.reserve(dest.size() + vec.size());
        dest.insert(dest.end(), vec.begin(), vec.end());
    }
}    // namespace spade
