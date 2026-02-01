#pragma once

#include "utils/common.hpp"
#include <cassert>
#include <concepts>
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

    enum ValueTag : uint64_t {
        VALUE_NULL,
        VALUE_BOOL,
        VALUE_CHAR,
        VALUE_INT,
        VALUE_UINT,
        VALUE_FLOAT,
        VALUE_OBJ,
    };

    class SWAN_EXPORT Value final {
        friend class ValueCheck;

        ValueTag tag;

        union As {
            struct {
                uint8_t b;
                char _pad0[7];
            };

            struct {
                char c;
                char _pad1[7];
            };

            struct {
                int64_t i;
            };

            struct {
                uint64_t u;
            };

            struct {
                double f;
            };

            struct {
                Obj *obj;
                char _pad3[8 - sizeof(Obj *)];
            };
        } as;

      public:
        Value(std::nullptr_t) : tag(VALUE_NULL), as{0} {}

        explicit Value() : tag(VALUE_NULL), as{0} {}

        explicit Value(bool b) : tag(VALUE_BOOL), as{.b = b} {}

        explicit Value(char c) : tag(VALUE_CHAR), as{.c = c} {}

        explicit Value(std::signed_integral auto i) : tag(VALUE_INT), as{.i = i} {}

        explicit Value(std::unsigned_integral auto u) : tag(VALUE_UINT), as{.u = u} {}

        explicit Value(std::floating_point auto f) : tag(VALUE_FLOAT), as{.f = f} {}

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
        Value rotate_left(const Value &n) const;
        Value rotate_right(const Value &n) const;

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

        bool is_uint() const {
            return tag == VALUE_UINT;
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

        uint64_t as_uint() const {
            assert(tag == VALUE_UINT);
            return as.u;
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

        void set(int64_t i) {
            tag = VALUE_INT;
            as.i = i;
        }

        void set(uint64_t u) {
            tag = VALUE_UINT;
            as.u = u;
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

    class ValueCheck {
        static_assert(sizeof(Value) == 16, "Size of Value class must be 16 bytes");

        static_assert(offsetof(Value, tag) == 0, "Value::tag should be at offset 0");
        static_assert(offsetof(Value, as.b) == 8, "Value::as::b should be at offset 8");
        static_assert(offsetof(Value, as.c) == 8, "Value::as::c should be at offset 8");
        static_assert(offsetof(Value, as.i) == 8, "Value::as::i should be at offset 8");
        static_assert(offsetof(Value, as.f) == 8, "Value::as::f should be at offset 8");
        static_assert(offsetof(Value, as.obj) == 8, "Value::as::obj should be at offset 8");

        static_assert(sizeof(Value::tag) == 8, "Value::tag should be 8 bytes");
        static_assert(sizeof(Value::As::f) == 8, "Value::As::f should be 8 bytes");
        static_assert(sizeof(Obj *) == 4 || sizeof(Obj *) == 8, "Value::as::obj should be 4 or 8 bytes");

      public:
        ValueCheck() = delete;
        ValueCheck(const ValueCheck &) = delete;
        ValueCheck(ValueCheck &&) = delete;
        ValueCheck &operator=(const ValueCheck &) = delete;
        ValueCheck &operator=(ValueCheck &&) = delete;
        ~ValueCheck() = delete;
    };
}    // namespace spade
