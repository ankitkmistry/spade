#include "utils.hpp"

uint64_t doubleToRaw(double number) {
    union Converter {
        uint64_t digits;
        double number;
    } converter{.number = number};
    return converter.digits;
}

uint64_t signedToUnsigned(int64_t number) {
    union Converter {
        uint64_t number1;
        int64_t number2;
    } converter{.number2 = number};
    return converter.number1;
}

string join(const vector<string>& list, const string& delimiter) {
    string text;
    for (int i = 0; i < list.size(); ++i) {
        text += list[i];
        if (i < list.size() - 1) text += delimiter;
    }
    return text;
}
