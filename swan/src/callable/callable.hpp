#pragma once

#include "ee/obj.hpp"

namespace spade
{
    class SWAN_EXPORT ObjCallable : public Obj {
      public:
        enum class Kind { FUNCTION, METHOD, CONSTRUCTOR, FOREIGN };

      protected:
        Kind kind;
        Sign sign;

        void validate_call_site();

      public:
        ObjCallable(ObjTag tag, Kind kind, const Sign &sign) : Obj(tag), kind(kind), sign(sign) {}

        Kind get_kind() const {
            return kind;
        }

        const Sign &get_sign() const {
            return sign;
        }

        void set_sign(const Sign &sign) {
            this->sign = sign;
        }

        bool truth() const override {
            return true;
        }

        /**
         * Calls this method with @p args on the current thread
         * @throws IllegalAccessError if the function is called outside a vm thread
         * @param self the self object (or this pointer) can be null if the method 
         *             does not require a self object
         * @param method the method to be called
         * @param args arguments of the method
         */
        virtual void call(Obj *self, const vector<Obj *> &args) = 0;

        /**
         * Calls this method with @p args on the current thread
         * @throws IllegalAccessError if the function is called outside a vm thread
         * @param self the self object (or this pointer) can be null if the method 
         *             does not require a self object
         * @param method the method to be called
         * @param args pointer to the args on the stack
         */
        virtual void call(Obj *self, Obj **args) = 0;

        /**
         * Calls this method with @p args on the current thread.
         * Invokes the VM, completes the execution of
         * the function and returns the return value.
         * In case the function returns void, @c ObjNull is returned
         * @throws IllegalAccessError if the function is called outside a vm thread
         * @param self the self object (or this pointer) can be null if the method 
         *             does not require a self object
         * @param args the method to be called
         * @return the return value of the function
         */
        Obj *invoke(Obj *self, const vector<Obj *> &args);
    };
}    // namespace spade
