#include "typeparam.hpp"

namespace spade {
    Type::Kind TypeParam::getKind() const {
        checkPlaceholder();
        return placeholder->getKind();
    }

    const Table<NamedRef *> &TypeParam::getTypeParams() const {
        checkPlaceholder();
        return placeholder->getTypeParams();
    }

    const Table<Type *> &TypeParam::getSupers() const {
        checkPlaceholder();
        return placeholder->getSupers();
    }

    const Table<MemberSlot> &TypeParam::getMemberSlots() const {
        checkPlaceholder();
        return placeholder->getMemberSlots();
    }

    Table<NamedRef *> &TypeParam::getTypeParams() {
        checkPlaceholder();
        return placeholder->getTypeParams();
    }

    Table<Type *> &TypeParam::getSupers() {
        checkPlaceholder();
        return placeholder->getSupers();
    }

    Table<MemberSlot> &TypeParam::getMemberSlots() {
        checkPlaceholder();
        return placeholder->getMemberSlots();
    }

    ObjModule *TypeParam::getModule() const {
        checkPlaceholder();
        return placeholder->getModule();
    }

    const Table<string> &TypeParam::getMeta() const {
        checkPlaceholder();
        return placeholder->getMeta();
    }

    const Sign &TypeParam::getSign() const {
        checkPlaceholder();
        return placeholder->getSign();
    }

    Type *TypeParam::getType() const {
        checkPlaceholder();
        return placeholder->getType();
    }

    Obj *TypeParam::copy() {
        auto newTypeParam = halloc<TypeParam>(info.manager, sign, module);
        newTypeParam->setPlaceholder(placeholder);
        return newTypeParam;
    }

    void TypeParam::checkPlaceholder() const {
        if (placeholder == null) throw IllegalTypeParamAccessError(sign.toString());
    }

    Obj *TypeParam::getMember(const string& name) const {
        checkPlaceholder();
        return placeholder->getMember(name);
    }

    void TypeParam::setMember(const string& name, Obj *value) {
        checkPlaceholder();
        placeholder->setMember(name, value);
    }

    ObjMethod *TypeParam::getSuperClassMethod(const string& sign) {
        checkPlaceholder();
        return placeholder->getSuperClassMethod(sign);
    }

    Obj *TypeParam::getStaticMember(const string& name) const {
        checkPlaceholder();
        return placeholder->getStaticMember(name);
    }

    void TypeParam::setStaticMember(const string& name, Obj *value) {
        checkPlaceholder();
        placeholder->setStaticMember(name, value);
    }

    Type *TypeParam::getReified(Obj **args, uint8 count) {
        checkPlaceholder();
        return placeholder->getReified(args, count);
    }
    TypeParam *TypeParam::getTypeParam(const string& name) const {
        checkPlaceholder();
        return placeholder->getTypeParam(name);
    }
    NamedRef *TypeParam::captureTypeParam(const string& name) {
        checkPlaceholder();
        return placeholder->captureTypeParam(name);
    }
}// namespace spade