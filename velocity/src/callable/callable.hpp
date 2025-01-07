#ifndef VELOCITY_CALLABLE_HPP
#define VELOCITY_CALLABLE_HPP

#include "../objects/obj.hpp"
#include "../objects/type.hpp"

namespace spade
{
    class ObjCallable : public Obj {
      public:
        enum class Kind {
            FUNCTION,
            METHOD,
            CONSTRUCTOR,
            FOREIGN
        };

      protected:
        Kind kind;

        void validateCallSite();

      public:
        ObjCallable(const Sign &sign, Kind kind, Type *type, ObjModule *module = null)
            : Obj(sign, type, module), kind(kind) {}

        /**
         * Calls this method with \p args on the current thread
         * @throws IllegalAccessError if the function is called outside a vm thread
         * @param method the method to be called
         * @param args arguments of the method
         */
        virtual void call(const vector<Obj *>& args) = 0;

        /**
         * Calls this method with \p args on the current thread
         * @throws IllegalAccessError if the function is called outside a vm thread
         * @param method the method to be called
         * @param args pointer to the args on the stack
         */
        virtual void call(Obj **args) = 0;

        virtual void setSelf(Obj *self) = 0;

        /**
         * Calls this method with \p args on the current thread.
         * Invokes the VM, completes the execution of
         * the function and returns the return value.
         * In case the function returns void, \c ObjNull is returned
         * @throws IllegalAccessError if the function is called outside a vm thread
         * @param args the method to be called
         * @return the return value of the function
         */
        Obj *invoke(vector<Obj *> args);

        Kind getKind() const { return kind; }

        bool truth() const override { return true; }
    };
}    // namespace spade

#endif    // VELOCITY_CALLABLE_HPP
