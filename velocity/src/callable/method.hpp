#ifndef VELOCITY_METHOD_HPP
#define VELOCITY_METHOD_HPP

#include "callable.hpp"
#include "frame_template.hpp"

namespace spade
{
    class ObjMethod final : public ObjCallable {
      private:
        static Table<map<Table<Type *>, ObjMethod *>> reificationTable;
        FrameTemplate *frameTemplate;
        Table<NamedRef *> typeParams;

        ObjMethod *returnReified(const Table<Type *>& tParams) const;

      public:
        ObjMethod(const Sign &sign, Kind kind, FrameTemplate *frame, Type *type, const Table<NamedRef *> &typeParams,
                  ObjModule *module = null);

        void call(const vector<Obj *>& args) override;

        void call(Obj **args) override;

        void setSelf(Obj *self) override;

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
        ObjMethod *getReified(Obj **args, uint8 count) const;

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
        ObjMethod *getReified(const vector<Type *>& args) const;

        TypeParam *getTypeParam(const string& name) const;

        Obj *copy() override;

        string toString() const override;

        const FrameTemplate *getFrameTemplate() const { return frameTemplate; }

        const Table<NamedRef *> &getTypeParams() const { return typeParams; }

        Table<NamedRef *> &getTypeParams() { return typeParams; }

        NamedRef *captureTypeParam(const string& name) const;
    };
} // namespace spade

#endif // VELOCITY_METHOD_HPP
