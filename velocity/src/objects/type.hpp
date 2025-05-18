#pragma once

#include "obj.hpp"

namespace spade
{
    class TypeParam;

    class Type : public Obj {
      public:
        enum class Kind {
            /// Represents a class
            CLASS,
            /// Represents an interface
            INTERFACE,
            /// Represents an enumeration class
            ENUM,
            /// Represents an annotation
            ANNOTATION,
            /// Represents an type param
            TYPE_PARAM,
            /// Represents an unresolved type
            UNRESOLVED
        };

      protected:
        Kind kind;
        Table<Type *> supers;
        Table<TypeParam *> type_params;

      private:
        static Table<std::unordered_map<Table<Type *>, Type *>> reification_table;

        Type *return_reified(const Table<Type *> &type_params);

      public:
        Type(const Sign &sign, Kind kind, const Table<TypeParam *> &type_params, const Table<Type *> &supers, const Table<MemberSlot> &member_slots,
             ObjModule *module = null)
            : Obj(sign, null, module), kind(kind), supers(supers), type_params(type_params) {
            this->member_slots = member_slots;
        }

        Type(const Type &type);

        virtual Kind get_kind() const {
            return kind;
        }

        virtual void set_kind(Kind kind) {
            this->kind = kind;
        }

        virtual const Table<Type *> &get_supers() const {
            return supers;
        }

        virtual const Table<TypeParam *> &get_type_params() const {
            return type_params;
        }

        virtual Table<Type *> &get_supers() {
            return supers;
        }

        virtual Table<TypeParam *> &get_type_params() {
            return type_params;
        }

        /**
         * Reifies this type and returns the reified type.
         * The returned type may be newly reified or previously reified
         * so as to maintain type uniqueness. The type to be reified not always
         * has to be a generic type. The objects in the array \p args
         * must be positioned according to the type params present in the signature
         * @throws ArgumentError if count is not correct
         * @throws CastError if objects in the array are not types
         * @param args the type args
         * @param count count of type args
         * @return the reified type
         */
        virtual Type *get_reified(Type *const *args, uint8 count) const;

        /**
         * Reifies this type and returns the reified type.
         * The returned type may be newly reified or previously reified
         * so as to maintain type uniqueness. The type to be reified not always
         * has to be a generic type. The objects in the array \p args
         * must be positioned according to the type params present in the signature
         * @throws ArgumentError if count is not correct
         * @throws CastError if objects in the array are not types
         * @param args the type args
         * @param count count of type args
         * @return the reified type
         */
        virtual Type *get_reified(Obj **args, uint8 count) const {
            return get_reified(*reinterpret_cast<Type ***>(&args), count);
        }

        /**
         * Reifies this type and returns the reified type.
         * The returned type may be newly reified or previously reified
         * so as to maintain type uniqueness. The type to be reified not always
         * has to be a generic type. The objects in the array \p args
         * must be positioned according to the type params present in the signature
         * @throws ArgumentError if args.size() > 256
         * @throws ArgumentError if number of arguments provided is not correct
         * @throws CastError if objects in the array are not types
         * @param args the type args
         * @param count count of type args
         * @return the reified type
         */
        Type *get_reified(const vector<Type *> &args) const;
        virtual TypeParam *get_type_param(const string &name) const;

        Obj *copy() override;
        bool truth() const override;
        string to_string() const override;

        static Type *UNRESOLVED(const Sign &sign, ObjModule *module = null, MemoryManager *manager = null);
    };
}    // namespace spade
