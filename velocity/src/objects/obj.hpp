#ifndef OOP_OBJ_HPP_
#define OOP_OBJ_HPP_

#include "../utils/common.hpp"
#include "../memory/memory.hpp"

namespace spade {
    class Type;

    class TypeParam;

    class ObjModule;

    class ObjMethod;

    class NamedRef;

    /*
     *   raw             = 0x 00000000 00000000
     *                        |      | |      |
     *                        +------+ +------+
     *                           |        |
     *   accessor        |-------+        |
     *   modifier        |----------------+
     *
     *
     *   modifier        = 0x  0  0  0  0  0  0  0  0
     *   =================                 |  |  |  |
     *   operator        |-----------------+  |  |  |
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

        Flags(uint16 raw = 0) : raw(raw) {}

        bool isStatic() const {
            return raw & 0b0000'0001;
        }

        bool isAbstract() const {
            return raw & 0b0000'0010;
        }

        bool isFinal() const {
            return raw & 0b0000'0100;
        }

        bool isOperator() const {
            return raw & 0b0000'1000;
        }

        bool isPrivate() const {
            return (raw >> 8) & 0b0000'0001;
        }

        bool isInternal() const {
            return (raw >> 8) & 0b0000'0010;
        }

        bool isPackagePrivate() const {
            return (raw >> 8) & 0b0000'0100;
        }

        bool isProtected() const {
            return (raw >> 8) & 0b0000'1000;
        }

        bool isPublic() const {
            return (raw >> 8) & 0b0001'0000;
        }
    };

    class MemberSlot {
    private:
        Obj *value;
        Flags flags;
    public:
        MemberSlot(Obj *value = null, Flags flags = Flags{0}) : value(value), flags(flags) {}

        Obj *getValue() const { return value; }

        Obj *&getValue() { return value; }

        void setValue(Obj *value_) { value = value_; }

        const Flags &getFlags() const { return flags; }
    };

/**
 * The abstract description of an object in the virtual machine
 */
    class Obj : public Collectible {
    protected:
        /// Module where this object belongs to
        ObjModule *module;
        /// Signature of the object
        Sign sign;
        /// Type of the object
        Type *type;
        /// Member slots of the object
        Table<MemberSlot> memberSlots = {};
        /// Methods of superclass which have been overrode
        Table<ObjMethod *> superClassMethods = {};

        /**
         * Changes pointer to type params @p pObj specified in @p old_ to pointers specified in @p new_.
         * This function reifies type parameters recursively.
         * @param pObj pointer to the object
         * @param old_ old type parameters
         * @param new_ new type parameters
         */
        static void reify(Obj **pObj, const Table<NamedRef *>&old_, const Table<NamedRef *>& new_);

    public:
        /**
         * Creates a deep copy of \p obj.
         * This function is more safe than Obj::copy as this prevents
         * unnecessary copies of types, modules and callable objects.
         * The user must always use this function to create safe copies of objects.
         * @param obj
         * @return
         */
        static Obj *createCopy(Obj *obj);

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
        virtual Obj *copy();

        /**
         * @return the corresponding truth value of the object
         */
        virtual bool truth() const { return true; }

        /**
         * @return a string representation of this object for VM context only
         */
        virtual string toString() const;

        /**
         * @return the encapsulating module of the object
         */
        virtual ObjModule *getModule() const { return module; }

        /**
         * @return the signature of the object
         */
        virtual const Sign &getSign() const { return sign; }

        /**
         * @return the type of the object
         */
        virtual Type *getType() const { return type; }

        /**
         * Sets the type of the object
         * @param destType the destination type
         */
        void setType(Type *destType) { this->type = destType; }

        /**
         * @return the members of this object
         */
        virtual const Table<MemberSlot> &getMemberSlots() const { return memberSlots; }

        /**
         * @return the members of this object
         */
        virtual Table<MemberSlot> &getMemberSlots() { return memberSlots; }

        /**
         * @throws IllegalAccessError if the member cannot be found
         * @param name the name of the member
         * @return the member of this object, the member can be static also
         */
        virtual Obj *getMember(const string& name) const;

        /**
         * Sets the member of this object with \p name and sets it to \p value.
         * If a member with \p name does not exist then creates a new member and sets it to \p value
         * @param name name of the member
         * @param value value to be set to
         */
        virtual void setMember(const string& name, Obj *value);

        /**
         * @throws IllegalAccessError if the superclass method cannot be found
         * @param mSign complete signature of the method
         * @return the method of the superclass has been overrode by this object
         */
        virtual ObjMethod *getSuperClassMethod(const string& mSign);

        /**
         * @return the meta information of the object
         */
        virtual const Table<string> &getMeta() const;
    };

    class ObjBool;

/**
 * The abstract description of a comparable object in the virtual machine.
 * Simple comparison operations can be performed on this kind of object
 */
    class ComparableObj : public Obj {
    public:
        ComparableObj(Sign sign, Type *type, ObjModule *module)
                : Obj(sign, type, module) {}

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
}

#endif /* OOP_OBJ_HPP_ */
