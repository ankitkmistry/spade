#ifndef VELOCITY_TYPE_HPP
#define VELOCITY_TYPE_HPP

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
        Table<NamedRef *> typeParams;

      private:
        static Table<map<Table<Type *>, Type *>> reificationTable;

        Type *returnReified(const Table<Type *>& typeParams);

      public:
        Type(const Sign &sign, Kind kind, const Table<NamedRef *> &typeParams, const Table<Type *> &supers,
             const Table<MemberSlot> &memberSlots, ObjModule *module = null)
            : Obj(sign, null, module), kind(kind), supers(supers), typeParams(typeParams) {
            this->memberSlots = memberSlots;
        }

        Type(const Type &type);

        virtual Kind getKind() const { return kind; }

        virtual const Table<Type *> &getSupers() const { return supers; }

        virtual const Table<NamedRef *> &getTypeParams() const { return typeParams; }

        virtual Table<Type *> &getSupers() { return supers; }

        virtual Table<NamedRef *> &getTypeParams() { return typeParams; }

        virtual Obj *getStaticMember(const string& name) const;

        virtual void setStaticMember(const string& name, Obj *value);

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
        virtual Type *getReified(Obj **args, uint8 count);

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
        Type *getReified(const vector<Type *>& args);

        virtual TypeParam *getTypeParam(const string& name) const;

        virtual NamedRef *captureTypeParam(const string& name);

        Obj *copy() override;

        bool truth() const override;

        string toString() const override;

        static Type *SENTINEL_(const string &sign, MemoryManager *manager);
    };
} // namespace spade

#endif // VELOCITY_TYPE_HPP
