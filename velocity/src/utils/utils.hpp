#pragma once

#include "common.hpp"
#include "exceptions.hpp"

#include <sputils.hpp>

namespace spade
{
    template<typename T>
    concept StringConvertibleCamelCase = requires(T t) {
        { t.toString() } -> std::same_as<std::string>;
    };

    /**
     * Converts a std::vector<T> to a comma separated list.
     * Assumes that values have a to_string() method that gives std::string representation of the value.
     * @tparam T type of the vector items
     * @param data the vector to be processed
     * @return the comma separated list as a std::string
     */
    template<StringConvertibleCamelCase T>
    std::string listToString(const std::vector<T> &data) {
        std::string str;
        const int length = data.size();
        for (uint16_t i = 0; i < length; ++i) str += data[i].toString() + (i < length - 1 ? ", " : "");
        return str;
    }

    /**
     * Converts a std::vector<T*> to a comma separated list.
     * Assumes that values have a to_string() method that gives std::string representation of the value.
     * @tparam T type of the std::vectors
     * @param data the vector to be processed
     * @return the comma separated list as a std::string
     */
    template<StringConvertibleCamelCase T>
    std::string listToString(const std::vector<T *> &data) {
        std::string str;
        const int length = data.size();
        for (uint16_t i = 0; i < length; ++i) str += data[i]->toString() + (i < length - 1 ? ", " : "");
        return str;
    }
}    // namespace spade