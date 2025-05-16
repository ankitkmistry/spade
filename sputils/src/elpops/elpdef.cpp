#include "elpdef.hpp"
#include "spimp/error.hpp"
#include "spimp/utils.hpp"

namespace spade
{
    bool CpInfo::operator==(const CpInfo &rhs) const {
        if (tag != rhs.tag)
            return false;
        switch (tag) {
            case 0x00:
            case 0x01:
            case 0x02:
                return true;
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
                return value == rhs.value;
            default:
                throw Unreachable();
        }
    }

    bool CpInfo::operator!=(const CpInfo &rhs) const {
        return !(rhs == *this);
    }

    CpInfo CpInfo::from_bool(bool b) {
        return CpInfo{.tag = static_cast<uint8_t>(b ? 0x01 : 0x02)};
    }

    CpInfo CpInfo::from_char(uint32_t c) {
        return CpInfo{.tag = 0x03, .value = c};
    }

    CpInfo CpInfo::from_int(int64_t i) {
        return CpInfo{.tag = 0x04, .value = signed_to_unsigned(i)};
    }

    CpInfo CpInfo::from_float(double d) {
        return CpInfo{.tag = 0x05, .value = double_to_raw(d)};
    }

    CpInfo CpInfo::from_string(const string &str) {
        _UTF8 utf8;
        utf8.len = str.size();
        utf8.bytes = vector<uint8_t>(utf8.len);
        std::memcpy(utf8.bytes.data(), str.data(), utf8.len);
        return CpInfo{.tag = 0x06, .value = utf8};
    }

    CpInfo CpInfo::from_array(const std::vector<CpInfo> &v) {
        _Container arr;
        arr.len = v.size();
        arr.items = vector<CpInfo>(arr.len);
        for (int i = 0; i < arr.len; ++i) {
            arr.items[i] = v[i];
        }
        return CpInfo{.tag = 0x07, .value = arr};
    }

    bool _UTF8::operator==(const _UTF8 &rhs) const {
        return len == rhs.len && std::memcmp(bytes.data(), rhs.bytes.data(), len) == 0;
    }

    bool _UTF8::operator!=(const _UTF8 &rhs) const {
        return !(rhs == *this);
    }

    bool _Container::operator==(const _Container &rhs) const {
        if (len != rhs.len)
            return false;
        for (int i = 0; i < len; ++i)
            if (items[i] != rhs.items[i])
                return false;
        return true;
    }

    bool _Container::operator!=(const _Container &rhs) const {
        return !(rhs == *this);
    }
}    // namespace spade