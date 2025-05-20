#include "method.hpp"
#include "ee/thread.hpp"
#include "memory/memory.hpp"
#include "objects/typeparam.hpp"

namespace spade
{
    std::unordered_map<string, ObjMethod *> ObjMethod::reification_table = {};

    ObjMethod::ObjMethod(const Sign &sign, Kind kind, const FrameTemplate &frame, const Table<TypeParam *> &type_params, ObjModule *module)
        : ObjCallable(sign, kind, type, module), frame_template(frame), type_params(type_params) {
        frame_template.set_method(this);
    }

    Obj *ObjMethod::copy() const {
        const auto obj = halloc_mgr<ObjMethod>(info.manager, sign, kind, frame_template, type_params, module);
        // Copy members
        for (const auto &[name, slot]: member_slots) {
            obj->set_member(name, create_copy(slot.get_value()));
        }
        // Create new type params
        Table<TypeParam *> new_type_params;
        for (const auto &[name, type_param]: type_params) {
            new_type_params[name] = cast<TypeParam>(type_param->copy());
        }
        Obj::reify(obj, type_params, new_type_params);
        return obj;
    }

    void ObjMethod::call(const vector<Obj *> &args) {
        validate_call_site();
        Thread *thread = Thread::current();
        auto new_frame = frame_template.initialize();
        if (new_frame.get_args().count() < args.size())
            throw ArgumentError(sign.to_string(), std::format("too less arguments, expected {} got {}", new_frame.get_args().count(), args.size()));
        if (new_frame.get_args().count() > args.size())
            throw ArgumentError(sign.to_string(), std::format("too many arguments, expected {} got {}", new_frame.get_args().count(), args.size()));
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
        const static string kind_names[] = {"function", "method", "constructor"};
        return std::format("<{} '{}'>", kind_names[static_cast<int>(kind)], sign.to_string());
    }

    ObjMethod *ObjMethod::get_reified(Obj **args, uint8 count) const {
        // Check if the number of type args is correct
        if (type_params.size() < count)
            throw ArgumentError(sign.to_string(), std::format("too less type arguments, expected {} got {}", type_params.size(), count));
        if (type_params.size() > count)
            throw ArgumentError(sign.to_string(), std::format("too many type arguments, expected {} got {}", type_params.size(), count));

        Table<Type *> type_args;
        string ta_specifier = "[";    // Type argument specifier
        for (int i = 0; i < count; ++i) {
            // Build the type arg specifier and the list of type args
            const auto type = cast<Type>(args[i]);
            type_args["[" + get_sign().get_type_params()[i] + "]"] = type;
            ta_specifier.append(type->get_sign().to_string());
            ta_specifier.append(", ");
        }
        ta_specifier.pop_back();
        ta_specifier.pop_back();
        ta_specifier.append("]");

        if (const auto it = reification_table.find(ta_specifier); it != reification_table.end())
            // Return the method if it was already reified
            return it->second;
        // Or else, create a new method and reify it
        const auto reified_met = cast<ObjMethod>(copy());
        for (const auto &[name, tp]: reified_met->type_params) {
            tp->set_placeholder(type_args[name]);
        }
        // Return the reified method
        return reified_met;
    }

    ObjMethod *ObjMethod::get_reified(const vector<Type *> &args) const {
        if (args.size() >= UINT8_MAX)
            throw ArgumentError(to_string(), "number of type arguments cannot be greater than 256");
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
