#pragma once

#include "utils/common.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace spade
{
    class Obj;

    enum class Ordering {
        LESS,
        EQUAL,
        GREATER,
        UNDEFINED,
    };

    enum ValueTag {
        VALUE_NULL,
        VALUE_BOOL,
        VALUE_CHAR,
        VALUE_INT,
        VALUE_FLOAT,
        VALUE_OBJ,
    };

    class SWAN_EXPORT Value final {
        ValueTag tag;

        union {
            bool b;
            char c;
            int64_t i;
            double f;
            Obj *obj;
        } as;

      public:
        Value(std::nullptr_t) : tag(VALUE_NULL), as{0} {}

        explicit Value() : tag(VALUE_NULL), as{0} {}

        explicit Value(bool b) : tag(VALUE_BOOL), as{.b = b} {}

        explicit Value(char c) : tag(VALUE_CHAR), as{.c = c} {}

        explicit Value(int64_t i) : tag(VALUE_INT), as{.i = i} {}

        explicit Value(double f) : tag(VALUE_FLOAT), as{.f = f} {}

        Value(Obj *obj) : tag(VALUE_OBJ), as{.obj = obj} {}

        Value(const Value &) = default;
        Value(Value &&) = default;
        Value &operator=(const Value &) = default;
        Value &operator=(Value &&) = default;
        ~Value() = default;

        Ordering compare(const Value &other) const;
        Value operator<(const Value &other) const;
        Value operator>(const Value &other) const;
        Value operator<=(const Value &other) const;
        Value operator>=(const Value &other) const;
        Value operator==(const Value &other) const;
        Value operator!=(const Value &other) const;

        Value operator!() const;

        Value operator-() const;
        Value power(const Value &n) const;
        Value operator+(const Value &n) const;
        Value operator-(const Value &n) const;
        Value operator*(const Value &n) const;
        Value operator/(const Value &n) const;

        Value operator~() const;
        Value operator%(const Value &n) const;
        Value operator<<(const Value &n) const;
        Value operator>>(const Value &n) const;
        Value operator&(const Value &n) const;
        Value operator|(const Value &n) const;
        Value operator^(const Value &n) const;
        Value unsigned_right_shift(const Value &n) const;

        /**
         * Performs a complete deep copy on the value.
         * @return a copy of the object
         */
        Value copy() const;

        /**
         * @return the corresponding truth value of the object
         */
        bool truth() const;

        operator bool() const {
            return truth();
        }

        /**
         * @return a string representation of this object for VM context only
         */
        string to_string() const;

        ValueTag get_tag() const {
            return tag;
        }

        bool is_null() const {
            return tag == VALUE_NULL;
        }

        bool is_bool() const {
            return tag == VALUE_BOOL;
        }

        bool is_char() const {
            return tag == VALUE_CHAR;
        }

        bool is_int() const {
            return tag == VALUE_INT;
        }

        bool is_float() const {
            return tag == VALUE_FLOAT;
        }

        bool is_obj() const {
            return tag == VALUE_OBJ;
        }

        bool as_bool() const {
            assert(tag == VALUE_BOOL);
            return as.b;
        }

        char as_char() const {
            assert(tag == VALUE_CHAR);
            return as.c;
        }

        int64_t as_int() const {
            assert(tag == VALUE_INT);
            return as.i;
        }

        double as_float() const {
            assert(tag == VALUE_FLOAT);
            return as.f;
        }

        Obj *as_obj() const {
            assert(tag == VALUE_OBJ);
            return as.obj;
        }

        void set(std::nullptr_t) {
            tag = VALUE_NULL;
        }

        void set(bool b) {
            tag = VALUE_BOOL;
            as.b = b;
        }

        void set(char c) {
            tag = VALUE_CHAR;
            as.c = c;
        }

        void set(int32_t i) {
            tag = VALUE_INT;
            as.i = i;
        }

        void set(double f) {
            tag = VALUE_FLOAT;
            as.f = f;
        }

        void set(Obj *obj) {
            tag = VALUE_OBJ;
            as.obj = obj;
        }
    };

    static_assert(sizeof(Value) == 16, "Size of Value class must be 16 bytes");
}    // namespace spade
