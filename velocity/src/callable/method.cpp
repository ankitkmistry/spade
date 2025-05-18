#include "method.hpp"
#include "ee/thread.hpp"
#include "memory/memory.hpp"
#include "objects/typeparam.hpp"

namespace spade
{
    Table<std::unordered_map<Table<Type *>, ObjMethod *>> ObjMethod::reificationTable = {};

    ObjMethod::ObjMethod(const Sign &sign, Kind kind, const FrameTemplate &frame, const Table<TypeParam *> &type_params, ObjModule *module)
        : ObjCallable(sign, kind, type, module), frame_template(frame), type_params(type_params) {
        frame_template.set_method(this);
    }

    Obj *ObjMethod::copy() {
        // Copy type params
        Table<TypeParam *> new_type_params;
        for (const auto &[name, type_param]: type_params) {
            new_type_params[name] = Obj::create_copy(type_param);
        }
        // Create new type object
        Obj *new_type = halloc_mgr<ObjMethod>(info.manager, sign, kind, frame_template, new_type_params, module);
        // Reify the type params
        reify(&new_type, type_params, new_type_params);
        return new_type;
    }

    void ObjMethod::call(const vector<Obj *> &args) {
        validate_call_site();
        Thread *thread = Thread::current();
        auto new_frame = frame_template.initialize();
        if (new_frame.get_args().count() < args.size()) {
            throw ArgumentError(sign.to_string(), std::format("too less arguments, expected {} got {}", new_frame.get_args().count(), args.size()));
        }
        if (new_frame.get_args().count() > args.size()) {
            throw ArgumentError(sign.to_string(), std::format("too many arguments, expected {} got {}", new_frame.get_args().count(), args.size()));
        }
        for (int i = 0; i < new_frame.get_args().count(); i++) {
            new_frame.get_args().set(i, args[i]);
        }
        thread->get_state()->push_frame(new_frame);
    }

    void ObjMethod::call(Obj **args) {
        validate_call_site();
        Thread *thread = Thread::current();
        auto new_frame = frame_template.initialize();
        for (int i = 0; i < new_frame.get_args().count(); i++) {
            new_frame.get_args().set(i, args[i]);
        }
        thread->get_state()->push_frame(new_frame);
    }

    string ObjMethod::to_string() const {
        static string kind_names[] = {"function", "method", "constructor"};
        return std::format("<{} '{}'>", kind_names[static_cast<int>(kind)], sign.to_string());
    }

    ObjMethod *ObjMethod::return_reified(const Table<Type *> &t_params) const {
        auto method = cast<ObjMethod>(const_cast<ObjMethod *>(this)->copy());
        auto params = method->get_type_params();
        for (auto [name, type_param]: params) {
            type_param->set_placeholder(t_params.at(name));
        }
        return method;
    }

    ObjMethod *ObjMethod::get_reified(Obj **args, uint8 count) const {
        return null;
        // TODO: implement this

        // if (type_params.size() != count) {
        //     throw ArgumentError(sign.to_string(), std::format("expected {} type arguments, but got {}", type_params.size(), count));
        // }

        // Table<Type *> type_args;
        // for (int i = 0; i < count; ++i) {
        //     type_args[get_sign().get_type_params()[i]] = (cast<Type>(args[i]));
        // }

        // std::unordered_map<Table<Type *>, ObjMethod *> table;
        // ObjMethod *reified_method;

        // if (auto it = reificationTable.find(get_sign().to_string()); it != reificationTable.end()) {
        //     table = it->second;
        // } else {
        //     reified_method = return_reified(type_args);
        //     table[type_args] = reified_method;
        //     reificationTable[get_sign().to_string()] = table;
        //     return reified_method;
        // }

        // if (auto it = table.find(type_args); it != table.end())
        //     return it->second;
        // else {
        //     reified_method = return_reified(type_args);
        //     reificationTable[get_sign().to_string()][type_args] = reified_method;
        //     return reified_method;
        // }
    }

    ObjMethod *ObjMethod::get_reified(const vector<Type *> &args) const {
        if (args.size() >= UINT8_MAX) {
            throw ArgumentError(to_string(), "number of type arguments cannot be greater than 256");
        }
        auto data = const_cast<Type **>(args.data());
        return get_reified(reinterpret_cast<Obj **>(data), static_cast<uint8>(args.size()));
    }

    TypeParam *ObjMethod::get_type_param(const string &name) const {
        if (const auto it = type_params.find(name); it != type_params.end())
            return it->second;
        if (type != null)
            return type->get_type_param(name);
        throw IllegalAccessError(std::format("cannot find type param {} in {}", name, to_string()));
    }
}    // namespace spade
