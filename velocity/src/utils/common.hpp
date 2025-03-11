#pragma once

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
    using Table = std::unordered_map<string, T, std::hash<string>, std::equal_to<string>>;

    class Type;
}    // namespace spade

/*
 * This is to fix an annoying build error
 */
template<>
class std::less<spade::Table<spade::Type *>> {
    using self = spade::Table<spade::Type *>;

  public:
    bool operator()(const self &lhs, const self &rhs) const {
        if (lhs.size() != rhs.size()) {
            return lhs.size() < rhs.size();
        } else {
            auto lIt = lhs.begin();
            auto rIt = rhs.begin();
            while (lIt != lhs.end()) {
                if (lIt->first != rIt->first)
                    return lIt->first < rIt->first;
                if (lIt->second != rIt->second)
                    return lIt->second < rIt->second;
                lIt++;
                rIt++;
            }
            return false;
        }
    }
};
