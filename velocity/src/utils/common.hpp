#ifndef SOURCE_UTILS_COMMON_HPP_
#define SOURCE_UTILS_COMMON_HPP_

#include "constants.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <algorithm>
#include <concepts>
#include <stdexcept>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <ranges>
#include <format>
#include <sputils.hpp>

// TODO: For debug only
#include <iostream>

#define null (nullptr)

namespace spade
{
    namespace fs = std::filesystem;

    using std::string, std::vector, std::map, std::function;

    using intptr = std::intptr_t;

    using uint8 = std::uint8_t;
    using uint16 = std::uint16_t;
    using uint32 = std::uint32_t;
    using uint64 = std::uint64_t;

    using int8 = std::int8_t;
    using int16 = std::int16_t;
    using int32 = std::int32_t;
    using int64 = std::int64_t;

    template<class T>
    using Table = std::unordered_map<string, T>;

    template<typename T>
    concept StringConvertible = requires(T t) {
        { t.toString() } -> std::same_as<string>;
    };
} // namespace spade

#endif /* SOURCE_UTILS_COMMON_HPP_ */
