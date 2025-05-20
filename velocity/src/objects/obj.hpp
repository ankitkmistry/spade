#pragma once

#include "utils/common.hpp"
#include "memory/manager.hpp"

namespace spade
{
    class Obj;
    class Type;
    class TypeParam;
    class ObjModule;
    class ObjCallable;
    class ObjMethod;

    /*
     *   raw             = 0x 00000000 00000000
     *                        |      | |      |
     *                        +------+ +------+
     *                           |         |
     *   accessor        |-------+         |
     *   modifier        |-----------------+
     *
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
    struct Flags {
        uint16 raw;

#define STATIC_MASK         (0b0000'0000'0000'0001)
#define ABSTRACT_MASK       (0b0000'0000'0000'0010)
#define FINAL_MASK          (0b0000'0000'0000'0100)
#define OVERRIDE_MASK       (0b0000'0000'0000'1000)
#define PRIVATE_MASK        (0b0000'0001'0000'0000)
#define INTERNAL_MASK       (0b0000'0010'0000'0000)
#define MODULE_PRIVATE_MASK (0b0000'0100'0000'0000)
#define PROTECTED_MASK      (0b0000'1000'0000'0000)
#define PUBLIC_MASK         (0b0001'0000'0000'0000)

        constexpr Flags(uint16 raw = 0) : raw(raw) {}

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
    };

    class MemberSlot {
      private:
        Obj *value;
        Flags flags;

      public:
        MemberSlot(Obj *value = null, Flags flags = Flags{0}) : value(value), flags(flags) {}

        Obj *get_value() const {
            return value;
        }

        Obj *&get_value() {
            return value;
        }

        void set_value(Obj *value_) {
            value = value_;
        }

        const Flags &get_flags() const {
            return flags;
        }
    };

    struct MemoryInfo {
        bool marked = false;
        uint32 life = 0;
        MemoryManager *manager = null;
    };

    /**
     * The abstract description of an object in the virtual machine
     */
    class Obj {
      protected:
        MemoryInfo info;
        /// Module where this object belongs to
        ObjModule *module;
        /// Signature of the object
        Sign sign;
        /// Type of the object
        Type *type;
        /// Member slots of the object
        Table<MemberSlot> member_slots = {};
        /// Methods of superclass which have been overrode
        Table<ObjMethod *> super_class_methods = {};

        /**
         * Changes pointer to type params @p pObj specified in @p old_ to pointers specified in @p new_.
         * This function reifies type parameters recursively.
         * @param pObj pointer to the object
         * @param old_ old type parameters
         * @param new_ new type parameters
         */
        static void reify(Obj **pObj, const Table<TypeParam *> &old_, const Table<TypeParam *> &new_);

        static Obj *create_copy_dynamic(const Obj *obj);

      public:
        /**
         * Creates a deep copy of \p obj.
         * This function is more safe than Obj::copy as this prevents
         * unnecessary copies of types, modules and callable objects.
         * The user must always use this function to create safe copies of objects.
         * @param obj
         * @return
         */
        template<typename T>
            requires std::derived_from<T, Obj>
        static T *create_copy(const T *obj) {
            if constexpr (std::same_as<T, Type> || std::derived_from<T, Type> || std::same_as<T, ObjCallable> || std::derived_from<T, ObjCallable> ||
                          std::same_as<T, ObjModule> || std::derived_from<T, ObjModule>)
                // Unique state
                return (T *) obj;
            else
                return (T *) create_copy_dynamic(obj);
        }

        /**
         * 
         * @param sign
         * @param type
         * @param module
         */
        Obj(const Sign &sign, Type *type, ObjModule *module = null);

        /**
         * Performs a complete deep copy on the object.
         * @warning The user should not use this function except in exceptional cases
         * @return a copy of the object
         */
        virtual Obj *copy() const;

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
         * @return the encapsulating module of the object
         */
        virtual ObjModule *get_module() const {
            return module;
        }

        /**
         * @return the signature of the object
         */
        virtual const Sign &get_sign() const {
            return sign;
        }

        /**
         * @return the type of the object
         */
        virtual Type *get_type() const {
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
        virtual const Table<MemberSlot> &get_member_slots() const {
            return member_slots;
        }

        /**
         * @return the members of this object
         */
        virtual Table<MemberSlot> &get_member_slots() {
            return member_slots;
        }

        /**
         * @throws IllegalAccessError if the member cannot be found
         * @param name the name of the member
         * @return the member of this object, the member can be static also
         */
        virtual Obj *get_member(const string &name) const;

        /**
         * Sets the member of this object with \p name and sets it to \p value.
         * If a member with \p name does not exist then creates a new member and sets it to \p value
         * @param name name of the member
         * @param value value to be set to
         */
        virtual void set_member(const string &name, Obj *value);

        /**
         * @throws IllegalAccessError if the superclass method cannot be found
         * @param mSign complete signature of the method
         * @return the method of the superclass has been overrode by this object
         */
        virtual ObjMethod *get_super_class_method(const string &mSign);

        /**
         * @return the meta information of the object
         */
        virtual const Table<string> &get_meta() const;
    };

    class ObjBool;

    /**
     * The abstract description of a comparable object in the virtual machine.
     * Simple comparison operations can be performed on this kind of object
     */
    class ComparableObj : public Obj {
      public:
        ComparableObj(Sign sign, Type *type, ObjModule *module) : Obj(sign, type, module) {}

        /**
         * Performs comparison between two objects
         * @param rhs the other object to compare
         * @return \< 0 if the object is less than this,
         *         = 0 if the object is equal to this,
         *         otherwise > 0 if the object is greater than this
         */
        virtual int32 compare(const Obj *rhs) const = 0;

        ObjBool *operator<(const Obj *rhs) const;

        ObjBool *operator>(const Obj *rhs) const;

        ObjBool *operator<=(const Obj *rhs) const;

        ObjBool *operator>=(const Obj *rhs) const;

        ObjBool *operator==(const Obj *rhs) const;

        ObjBool *operator!=(const Obj *rhs) const;
    };
}    // namespace spade
