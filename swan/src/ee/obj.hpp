#pragma once

#include "ee/value.hpp"
#include "utils/common.hpp"
#include "memory/manager.hpp"
#include "spinfo/sign.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <boost/functional/hash.hpp>

namespace spade
{
    class Obj;

    /*
     *   raw             = 0x 00000000 00000000
     *                        |      | |      |
     *                        +------+ +------+
     *                           |         |
     *   accessor        |-------+         |
     *   modifier        |-----------------+
     *
     *   modifier        = 0x  0  0  0  0  0  0  0  0
     *   =================                 |  |  |  |
     *   override        |-----------------+  |  |  |
     *   final           |--------------------+  |  |
     *   abstract        |-----------------------+  |
     *   static          |--------------------------+
     *
     *   accessor        = 0x  0  0  0  0  0  0  0  0
     *   =================              |  |  |  |  |
     *   public          |--------------+  |  |  |  |
     *   protected       |-----------------+  |  |  |
     *   package-private |--------------------+  |  |
     *   internal        |-----------------------+  |
     *   private         |--------------------------+
     */
    struct SWAN_EXPORT Flags {
        uint16_t raw;

#define STATIC_MASK         (0b0000'0000'0000'0001)
#define ABSTRACT_MASK       (0b0000'0000'0000'0010)
#define FINAL_MASK          (0b0000'0000'0000'0100)
#define OVERRIDE_MASK       (0b0000'0000'0000'1000)
#define PRIVATE_MASK        (0b0000'0001'0000'0000)
#define INTERNAL_MASK       (0b0000'0010'0000'0000)
#define MODULE_PRIVATE_MASK (0b0000'0100'0000'0000)
#define PROTECTED_MASK      (0b0000'1000'0000'0000)
#define PUBLIC_MASK         (0b0001'0000'0000'0000)

        constexpr Flags(uint16_t raw = 0) : raw(raw) {}

        constexpr Flags(const Flags &) = default;
        constexpr Flags(Flags &&) = default;
        constexpr Flags &operator=(const Flags &) = default;
        constexpr Flags &operator=(Flags &&) = default;
        constexpr ~Flags() = default;

        constexpr Flags &set_static(bool b = true) {
            raw = b ? raw | STATIC_MASK : raw & ~STATIC_MASK;
            return *this;
        }

        constexpr bool is_static() const {
            return raw & STATIC_MASK;
        }

        constexpr Flags &set_abstract(bool b = true) {
            raw = b ? raw | ABSTRACT_MASK : raw & ~ABSTRACT_MASK;
            return *this;
        }

        constexpr bool is_abstract() const {
            return raw & ABSTRACT_MASK;
        }

        constexpr Flags &set_final(bool b = true) {
            raw = b ? raw | FINAL_MASK : raw & ~FINAL_MASK;
            return *this;
        }

        constexpr bool is_final() const {
            return raw & FINAL_MASK;
        }

        constexpr Flags &set_override(bool b = true) {
            raw = b ? raw | OVERRIDE_MASK : raw & ~OVERRIDE_MASK;
            return *this;
        }

        constexpr bool is_override() const {
            return raw & OVERRIDE_MASK;
        }

        constexpr Flags &set_private(bool b = true) {
            raw = b ? raw | PRIVATE_MASK : raw & ~PRIVATE_MASK;
            return *this;
        }

        constexpr bool is_private() const {
            return raw & PRIVATE_MASK;
        }

        constexpr Flags &set_internal(bool b = true) {
            raw = b ? raw | INTERNAL_MASK : raw & ~INTERNAL_MASK;
            return *this;
        }

        constexpr bool is_internal() const {
            return raw & INTERNAL_MASK;
        }

        constexpr Flags &set_module_private(bool b = true) {
            raw = b ? raw | MODULE_PRIVATE_MASK : raw & ~MODULE_PRIVATE_MASK;
            return *this;
        }

        constexpr bool is_module_private() const {
            return raw & MODULE_PRIVATE_MASK;
        }

        constexpr Flags &set_protected(bool b = true) {
            raw = b ? raw | PROTECTED_MASK : raw & ~PROTECTED_MASK;
            return *this;
        }

        constexpr bool is_protected() const {
            return raw & PROTECTED_MASK;
        }

        constexpr Flags &set_public(bool b = true) {
            raw = b ? raw | PUBLIC_MASK : raw & ~PUBLIC_MASK;
            return *this;
        }

        constexpr bool is_public() const {
            return raw & PUBLIC_MASK;
        }

#undef STATIC_MASK
#undef ABSTRACT_MASK
#undef FINAL_MASK
#undef OVERRIDE_MASK
#undef PRIVATE_MASK
#undef INTERNAL_MASK
#undef MODULE_PRIVATE_MASK
#undef PROTECTED_MASK
#undef PUBLIC_MASK
    };

    class SWAN_EXPORT MemberSlot {
      private:
        Value value;
        Flags flags;

      public:
        MemberSlot(Value value = Value(), Flags flags = Flags{0}) : value(value), flags(flags) {}

        Value get_value() const {
            return value;
        }

        Value &get_value() {
            return value;
        }

        void set_value(Value value_) {
            value = value_;
        }

        Flags get_flags() const {
            return flags;
        }

        void set_flags(Flags flags) {
            this->flags = flags;
        }
    };

    enum ObjTag : uint8_t {
        // ObjString
        OBJ_STRING,
        // ObjArray
        OBJ_ARRAY,
        // Obj
        OBJ_OBJECT,
        // ObjCapture
        OBJ_CAPTURE,

        // ObjModule
        OBJ_MODULE,
        // ObjMethod
        OBJ_METHOD,
        // ObjForeign
        OBJ_FOREIGN,
        // Type
        OBJ_TYPE,
    };

    struct MemoryInfo {
        // bool marked = false;
        MemoryManager *manager = null;
    };

    class Type;

    class SWAN_EXPORT Obj {
      protected:
        /// Tag of the object
        ObjTag tag;
        /// Monitor of the object
        std::recursive_mutex monitor;
        /// Memory info of the object
        MemoryInfo info;
        /// Type of the object
        Type *type;
        /// Member slots of the object
        Table<MemberSlot> member_slots;
        mutable std::shared_mutex member_slots_mtx;

        Obj(ObjTag tag);

      public:
        Obj(Type *type);

        Obj() = delete;
        Obj(const Obj &) = delete;
        Obj(Obj &&) = delete;
        Obj &operator=(const Obj &) = delete;
        Obj &operator=(Obj &&) = delete;

        /**
         * Virtual destructor for Obj
         */
        virtual ~Obj() = default;

        /**
         * @return the tag of the object
         */
        ObjTag get_tag() const {
            return tag;
        }

        /**
         * @return the memory info of the object
         */
        const MemoryInfo &get_info() const {
            return info;
        }

        /**
         * @return the memory info of the object
         */
        MemoryInfo &get_info() {
            return info;
        }

        /**
         * @return the type of the object
         */
        Type *get_type() const {
            return type;
        }

        /**
         * Sets the type of the object
         * @param destType the destination type
         */
        void set_type(Type *destType);

        /**
         * @return the members of this object
         */
        const Table<MemberSlot> &get_member_slots() const {
            return member_slots;
        }

        /**
         * Performs a complete deep copy on the object.
         * @return a copy of the object
         */
        virtual Obj *copy() const;

        virtual Ordering compare(const Obj *other) const;
        Value operator<(const Obj *other) const;
        Value operator>(const Obj *other) const;
        Value operator<=(const Obj *other) const;
        Value operator>=(const Obj *other) const;
        Value operator==(const Obj *other) const;
        Value operator!=(const Obj *other) const;

        /**
         * @return the corresponding truth value of the object
         */
        virtual bool truth() const {
            return true;
        }

        /**
         * @return a string representation of this object for VM context only
         */
        virtual string to_string() const;

        /**
         * Enters the monitor for this object.
         * The call is blocked if the monitor was already entered.
         * The call is blocked until the monitor is exited.
         * The user may enter the monitor `n` number of times but he should take 
         * responsibility to exit the monitor exactly `n` number of times. 
         */
        void enter_monitor() {
            monitor.lock();
        }

        /**
         * Exits the monitor for this object.
         */
        void exit_monitor() {
            monitor.unlock();
        }

        /**
         * @throws IllegalAccessError if the member cannot be found
         * @param name the name of the member
         * @return the member of this object
         */
        Value get_member(const string &name) const;

        /**
         * Sets the member of this object with @p name and sets it to @p value.
         * If a member with @p name does not exist then creates a new member and sets it to @p value
         * @param name name of the member
         * @param value value to be set to
         */
        void set_member(const string &name, Value value);

        /**
         * @throws IllegalAccessError if the member cannot be found
         * @param name the name of the member
         * @return the access flags of the member of this object
         */
        Flags get_flags(const string &name) const;

        /**
         * Sets the access flags of the member of this object with @p name and sets it to @p value.
         * @throws IllegalAccessError if the member cannot be found
         * @param name name of the member
         * @param flags flags to be set to
         */
        void set_flags(const string &name, Flags flags);
    };

    class ObjString;
    class ObjArray;
    class ObjModule;
    class TypeParam;
    class ObjMethod;
    class ObjCapture;

    class SWAN_EXPORT ObjString final : public Obj {
      private:
        string str;

      public:
        ObjString(const string &str);
        ObjString(const uint8_t *bytes, uint16_t len);

        ObjString *concat(const ObjString *other);

        bool truth() const override {
            return !str.empty();
        }

        string to_string() const override {
            return str;
        }

        Obj *copy() const override {
            // immutable state
            return (Obj *) this;
        }

        Ordering compare(const Obj *other) const override;

        string value() const {
            return str;
        }
    };

    class SWAN_EXPORT ObjArray final : public Obj {
      private:
        std::unique_ptr<Value[]> array;
        size_t length;

      public:
        explicit ObjArray(size_t length);

        void for_each(const std::function<void(Value)> &func) const;

        Value get(int64_t i) const;
        Value get(size_t i) const;
        void set(int64_t i, Value value);
        void set(size_t i, Value value);

        size_t count() const {
            return length;
        }

        bool truth() const override {
            return length != 0;
        }

        string to_string() const override;
        Obj *copy() const override;

        /// Does lexicographical comparison
        Ordering compare(const Obj *other) const override;
    };

    class SWAN_EXPORT ObjModule final : public Obj {
      private:
        Sign sign;
        /// Path of the module
        fs::path path;
        /// The constant pool of the module
        vector<Value> constant_pool;
        /// The module init method
        ObjMethod *init = null;

      public:
        static ObjModule *current();

        ObjModule(const Sign &sign);

        Obj *copy() const override {
            return (Obj *) this;
        }

        bool truth() const override {
            return true;
        }

        string to_string() const override;

        const Sign &get_sign() const {
            return sign;
        }

        void set_sign(const Sign &sign) {
            this->sign = sign;
        }

        const fs::path &get_path() const {
            return path;
        }

        void set_path(const fs::path &path) {
            this->path = path;
        }

        const vector<Value> &get_constant_pool() const {
            return constant_pool;
        }

        void set_constant_pool(const vector<Value> &conpool) {
            constant_pool = conpool;
        }

        ObjMethod *get_init() const {
            return init;
        }

        void set_init(ObjMethod *init) {
            this->init = init;
        }
    };

    class SWAN_EXPORT Type final : public Obj {
      public:
        enum class Kind : uint8_t {
            /// Represents a class
            CLASS,
            /// Represents an interface
            INTERFACE,
            /// Represents an enumeration class
            ENUM,
            /// Represents an annotation
            ANNOTATION,
            /// Represents an unresolved type
            UNRESOLVED
        };

      protected:
        Kind kind;
        Sign sign;
        vector<Sign> supers;

        Type(ObjTag tag, Kind kind, Sign sign) : Obj(tag), kind(kind), sign(sign), supers() {}

      public:
        Type(Kind kind, Sign sign, const vector<Sign> &supers);

        Type(Sign sign) : Type(Kind::CLASS, sign, {}) {}

        Obj *copy() const override {
            return (Obj *) this;
        }

        bool truth() const override {
            return true;
        }

        string to_string() const override;

        Kind get_kind() const {
            return kind;
        }

        void set_kind(Kind kind) {
            this->kind = kind;
        }

        const Sign &get_sign() const {
            return sign;
        }

        void set_sign(const Sign &sign) {
            this->sign = sign;
        }

        vector<Sign> &get_supers() {
            return supers;
        }

        void set_supers(const vector<Sign> &supers) {
            this->supers = supers;
        }
    };

    class SWAN_EXPORT ObjCapture final : public Obj {
      private:
        Value value;

      public:
        ObjCapture(Value value);

        Value get() const {
            return value;
        }

        void set(Value value) {
            this->value = value;
        }

        Obj *copy() const override {
            return (Obj *) this;
        }

        bool truth() const override;
        string to_string() const override;
    };
}    // namespace spade

template<>
struct SWAN_EXPORT std::hash<std::vector<spade::Type *>> {
    size_t operator()(const std::vector<spade::Type *> &list) const {
        return boost::hash_range(list.begin(), list.end());
    }
};
