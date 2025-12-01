#pragma once

#include <cstddef>
#include <boost/functional/hash.hpp>

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
        Sign sign;
        Table<TypeParam *> type_params;
        Table<Type *> supers;

      private:
        // The table of all reified types in the form of [type_arg_specifier -> type]
        static std::unordered_map<vector<Type *>, Type *> reification_table;

      public:
        static Type *UNRESOLVED(const Sign &sign, ObjModule *module = null, MemoryManager *manager = null);

        Type(const Sign &sign, Kind kind, const Table<TypeParam *> &type_params, const Table<Type *> &supers, const Table<MemberSlot> &member_slots)
            : Obj(null), kind(kind), sign(sign), type_params(type_params), supers(supers) {
            this->tag = ObjTag::TYPE;
            this->member_slots = member_slots;
        }

        virtual Kind get_kind() const {
            return kind;
        }

        virtual void set_kind(Kind kind) {
            this->kind = kind;
        }

        virtual const Sign &get_sign() const {
            return sign;
        }

        virtual void set_sign(const Sign &sign) {
            this->sign = sign;
        }

        virtual const Table<TypeParam *> &get_type_params() const {
            return type_params;
        }

        virtual const Table<Type *> &get_supers() const {
            return supers;
        }

        virtual Table<Type *> &get_supers() {
            return supers;
        }

        virtual void set_supers(const Table<Type *> &supers) {
            this->supers = supers;
        }

        virtual Table<TypeParam *> &get_type_params() {
            return type_params;
        }

        virtual void set_type_params(const Table<TypeParam *> &type_params) {
            this->type_params = type_params;
        }

        /**
         * Reifies this type and returns the reified type.
        * The returned type may be newly reified or previously reified
         * so as to maintain type uniqueness. The type to be reified not always
         * has to be a generic type. The objects in the array @p args
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
         * has to be a generic type. The objects in the array @p args
         * must be positioned according to the type params present in the signature
         * @throws ArgumentError if count is not correct
         * @throws CastError if objects in the array are not types
         * @param args the type args
         * @param count count of type args
         * @return the reified type
         */
        Type *get_reified(Obj **args, uint8 count) const {
            // TODO: Think of something else
            // Potential UB here, breaking of strict aliasing rules
            return get_reified(*reinterpret_cast<Type ***>(&args), count);
        }

        /**
         * Reifies this type and returns the reified type.
         * The returned type may be newly reified or previously reified
         * so as to maintain type uniqueness. The type to be reified not always
         * has to be a generic type. The objects in the array @p args
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

        Obj *copy() const override;
        bool truth() const override;
        string to_string() const override;
    };
}    // namespace spade

template<>
struct std::hash<std::vector<spade::Type *>> {
    size_t operator()(const std::vector<spade::Type *> &list) const {
        return boost::hash_range(list.begin(), list.end());
    }
};
