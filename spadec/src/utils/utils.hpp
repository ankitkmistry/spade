#pragma once

#include <sstream>

#include "common.hpp"
#include "error.hpp"

namespace spade
{
    /**
     * @param str mangled name
     * @return the undecorated name of \p str
     */
    string cpp_demangle(string str);

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
        if (cast_val == null) throw CastError(cpp_demangle(typeid(From).name()), cpp_demangle(typeid(To).name()));
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
        if (cast_val == null) throw CastError(cpp_demangle(typeid(From).name()), cpp_demangle(typeid(To).name()));
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
        return dynamic_cast<T *>(obj) != null;
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
        return std::dynamic_pointer_cast<T>(obj) != null;
    }

    template<typename T, StringConvertible C>
    string join(const std::vector<T> &vec, const string &delimiter, std::function<C(T)> converter) {
        std::stringstream ss;
        for (int i = 0; i < vec.size(); ++i) {
            ss << converter(vec[i]);
            if (i < vec.size() - 1) ss << delimiter;
        }
        return ss.str();
    }

    template<typename T>
    string join(const std::vector<T> &vec, const string &delimiter, std::function<string(T)> converter) {
        std::stringstream ss;
        for (int i = 0; i < vec.size(); ++i) {
            ss << converter(vec[i]);
            if (i < vec.size() - 1) ss << delimiter;
        }
        return ss.str();
    }

    bool unescape(string& text);
}    // namespace spade