#pragma once

#include "common.hpp"

uint64_t doubleToRaw(double number);

uint64_t signedToUnsigned(int64_t number);

template<typename T>
static vector<T> slice(const vector<T> &list, int64_t start, int64_t end) {
    if (start < 0) start += list.size();
    if (end < 0) end += list.size();
    if (start >= list.size() || end >= list.size()) throw std::runtime_error("slice(): index out of bounds");
    if (start > end) std::swap(start, end);
    vector<T> result;
    result.reserve(end - start);
    for (int i = start; i < end; ++i) result.push_back(list[i]);
    return result;
}

string join(const vector<string> &list, const string &delimiter);
