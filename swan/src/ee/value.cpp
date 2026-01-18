#include "value.hpp"
#include "ee/obj.hpp"
#include "spimp/error.hpp"
#include <bit>
#include <cmath>
#include <string>

namespace spade
{
    Ordering Value::compare(const Value &other) const {
        if (tag != other.tag)
            return Ordering::UNDEFINED;
        switch (tag) {
        case VALUE_NULL:
            return Ordering::EQUAL;
        case VALUE_BOOL:
            if (as.b == other.as.b)
                return Ordering::EQUAL;
            return Ordering::UNDEFINED;
        case VALUE_CHAR:
            if (as.c < other.as.c)
                return Ordering::LESS;
            else if (as.c > other.as.c)
                return Ordering::GREATER;
            else
                return Ordering::EQUAL;
        case VALUE_INT:
            if (as.i < other.as.i)
                return Ordering::LESS;
            else if (as.i > other.as.i)
                return Ordering::GREATER;
            else
                return Ordering::EQUAL;
        case VALUE_FLOAT:
            if (as.f < other.as.f)
                return Ordering::LESS;
            else if (as.f > other.as.f)
                return Ordering::GREATER;
            else
                return Ordering::EQUAL;
        case VALUE_OBJ:
            return as.obj->compare(other.as.obj);
        default:
            throw Unreachable();
        }
    }

    Value Value::operator<(const Value &other) const {
        return Value(compare(other) == Ordering::LESS);
    }

    Value Value::operator>(const Value &other) const {
        return Value(compare(other) == Ordering::GREATER);
    }

    Value Value::operator<=(const Value &other) const {
        switch (compare(other)) {
        case Ordering::LESS:
        case Ordering::EQUAL:
            return Value(true);
        default:
            return Value(false);
        }
    }

    Value Value::operator>=(const Value &other) const {
        switch (compare(other)) {
        case Ordering::EQUAL:
        case Ordering::GREATER:
            return Value(true);
        default:
            return Value(false);
        }
    }

    Value Value::operator==(const Value &other) const {
        return Value(compare(other) == Ordering::EQUAL);
    }

    Value Value::operator!=(const Value &other) const {
        switch (compare(other)) {
        case Ordering::LESS:
        case Ordering::GREATER:
            return Value(true);
        default:
            return Value(false);
        }
    }

    Value Value::operator!() const {
        return Value(!truth());
    }

    Value Value::operator-() const {
        switch (tag) {
        case VALUE_INT:
            return Value(-as.i);
        case VALUE_FLOAT:
            return Value(-as.i);
        default:
            throw Unreachable();
        }
    }

    Value Value::power(const Value &n) const {
        if (tag != n.tag)
            throw Unreachable();

        switch (tag) {
        case VALUE_INT:
            return Value(std::pow(as.i, n.as.i));
        case VALUE_FLOAT:
            return Value(std::pow(as.f, n.as.f));
        default:
            throw Unreachable();
        }
    }

    Value Value::operator+(const Value &n) const {
        if (tag != n.tag)
            throw Unreachable();

        switch (tag) {
        case VALUE_INT:
            return Value(as.i + n.as.i);
        case VALUE_FLOAT:
            return Value(as.f + n.as.f);
        default:
            throw Unreachable();
        }
    }

    Value Value::operator-(const Value &n) const {
        if (tag != n.tag)
            throw Unreachable();

        switch (tag) {
        case VALUE_INT:
            return Value(as.i - n.as.i);
        case VALUE_FLOAT:
            return Value(as.f - n.as.f);
        default:
            throw Unreachable();
        }
    }

    Value Value::operator*(const Value &n) const {
        if (tag != n.tag)
            throw Unreachable();

        switch (tag) {
        case VALUE_INT:
            return Value(as.i * n.as.i);
        case VALUE_FLOAT:
            return Value(as.f * n.as.f);
        default:
            throw Unreachable();
        }
    }

    Value Value::operator/(const Value &n) const {
        if (tag != n.tag)
            throw Unreachable();

        switch (tag) {
        case VALUE_INT:
            return Value(as.i / n.as.i);
        case VALUE_FLOAT:
            return Value(as.f / n.as.f);
        default:
            throw Unreachable();
        }
    }

    Value Value::operator~() const {
        if (is_int())
            return Value(~as.i);
        throw Unreachable();
    }

    Value Value::operator%(const Value &n) const {
        if (tag != n.tag)
            throw Unreachable();

        if (is_int())
            return Value(as.i % n.as.i);
        throw Unreachable();
    }

    Value Value::operator<<(const Value &n) const {
        if (tag != n.tag)
            throw Unreachable();

        if (is_int())
            return Value(as.i << n.as.i);
        throw Unreachable();
    }

    Value Value::operator>>(const Value &n) const {
        if (tag != n.tag)
            throw Unreachable();

        if (is_int())
            return Value(as.i >> n.as.i);
        throw Unreachable();
    }

    Value Value::operator&(const Value &n) const {
        if (tag != n.tag)
            throw Unreachable();

        if (is_int())
            return Value(as.i & n.as.i);
        throw Unreachable();
    }

    Value Value::operator|(const Value &n) const {
        if (tag != n.tag)
            throw Unreachable();

        if (is_int())
            return Value(as.i | n.as.i);
        throw Unreachable();
    }

    Value Value::operator^(const Value &n) const {
        if (tag != n.tag)
            throw Unreachable();

        if (is_int())
            return Value(as.i ^ n.as.i);
        throw Unreachable();
    }

    Value Value::unsigned_right_shift(const Value &n) const {
        if (tag != n.tag)
            throw Unreachable();

        if (is_int())
            return Value(static_cast<int64_t>(static_cast<uint64_t>(as.i) >> n.as.i));
        throw Unreachable();
    }

    Value Value::rotate_left(const Value &n) const {
        if (tag != VALUE_INT || n.tag != VALUE_INT)
            throw Unreachable();
        return Value(static_cast<int64_t>(std::rotl(static_cast<uint64_t>(as.i), n.as.i)));
    }

    Value Value::rotate_right(const Value &n) const {
        if (tag != VALUE_INT || n.tag != VALUE_INT)
            throw Unreachable();
        return Value(static_cast<int64_t>(std::rotr(static_cast<uint64_t>(as.i), n.as.i)));
    }

    Value Value::copy() const {
        switch (tag) {
        case VALUE_NULL:
            return Value();
        case VALUE_BOOL:
            return Value(static_cast<bool>(as.b));
        case VALUE_CHAR:
            return Value(as.c);
        case VALUE_INT:
            return Value(as.i);
        case VALUE_FLOAT:
            return Value(as.f);
        case VALUE_OBJ:
            return Value(as.obj);
        default:
            throw Unreachable();
        }
    }

    bool Value::truth() const {
        switch (tag) {
        case VALUE_NULL:
            return false;
        case VALUE_BOOL:
            return as.b;
        case VALUE_CHAR:
            return as.c != '\0';
        case VALUE_INT:
            return as.i != 0;
        case VALUE_FLOAT:
            return as.f != 0.0;
        case VALUE_OBJ:
            return as.obj->truth();
        default:
            throw Unreachable();
        }
    }

    string Value::to_string() const {
        switch (tag) {
        case VALUE_NULL:
            return "null";
        case VALUE_BOOL:
            return as.b ? "true" : "false";
        case VALUE_CHAR:
            return string(1, as.c);
        case VALUE_INT:
            return std::to_string(as.i);
        case VALUE_FLOAT:
            return std::to_string(as.f);
        case VALUE_OBJ:
            return as.obj->to_string();
        default:
            throw Unreachable();
        }
    }
}    // namespace spade
