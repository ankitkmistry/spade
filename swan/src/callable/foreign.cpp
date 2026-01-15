#include "foreign.hpp"
#include "ee/thread.hpp"
#include "utils/errors.hpp"
#include <ffi.h>

namespace spade
{
    void ObjForeign::call(Obj *self, const vector<Value> &args) {
        validate_call_site();

        const uint8_t arg_count = sign.get_elements().back().get_params().size() & 0xFF;

        if (arg_count < args.size())
            throw ArgumentError(sign.to_string(), std::format("too less arguments, expected {} got {}", arg_count, args.size()));
        if (arg_count > args.size())
            throw ArgumentError(sign.to_string(), std::format("too many arguments, expected {} got {}", arg_count, args.size()));

        foreign_call(self, args);
    }

    void ObjForeign::call(Obj *self, Value *args) {
        validate_call_site();

        const uint8_t arg_count = sign.get_elements().back().get_params().size() & 0xFF;
        foreign_call(self, vector(args, args + arg_count));
    }

    void ObjForeign::foreign_call(Obj *self, vector<Value> args) {
        /// Call the foreign function as follows:
        /// If has_self:
        ///     handle(thread, self, args);
        /// If not has_self:
        ///     handle(thread, args)
        ///
        /// An equivalent C function can be declared as (the function can never be variadic):
        ///     spade::Value handle_with_self(spade::Thread *thread, Obj *self, Value *arg0, Value *arg1);
        ///     spade::Value handle(spade::Thread *thread, Value *arg0, Value *arg1);
        /// or
        ///     spade::Value handle_with_self(spade::Thread *thread, Obj *self);
        ///     spade::Value handle(spade::Thread *thread);
        /// or
        ///     spade::Value handle_with_self(spade::Thread *thread, Obj *self, Value *arg0);
        ///     spade::Value handle(spade::Thread *thread, Value *arg0);

        // class Value is always 16 bytes
        ffi_type *value_type_elements[3];
        value_type_elements[0] = &ffi_type_uint64;
        value_type_elements[1] = &ffi_type_uint64;
        value_type_elements[2] = null;

        ffi_type ffi_type_value;
        ffi_type_value.size = ffi_type_value.alignment = 0;
        ffi_type_value.type = FFI_TYPE_STRUCT;
        ffi_type_value.elements = value_type_elements;

        auto thread = Thread::current();
        Value return_value;

        ffi_cif cif;
        std::vector<ffi_type *> ffi_arg_types;
        std::vector<void *> ffi_values;

        // `thread` argument
        ffi_arg_types.push_back(&ffi_type_pointer);
        ffi_values.push_back(&thread);
        // `self` argument
        if (has_self) {
            ffi_arg_types.push_back(&ffi_type_pointer);
            ffi_values.push_back(&self);
        }
        // function arguments
        for (size_t i = 0; i < args.size(); i++) {
            Value &arg = args[i];
            switch (arg.get_tag()) {
            case VALUE_NULL:
            case VALUE_BOOL:
            case VALUE_CHAR:
            case VALUE_INT:
            case VALUE_FLOAT:
            case VALUE_OBJ:
                ffi_arg_types.push_back(&ffi_type_pointer);
                ffi_values.push_back(&arg);
            default:
                throw Unreachable();
            }
        }

        const auto result = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, ffi_arg_types.size(), &ffi_type_value, ffi_arg_types.data());
        switch (result) {
        case FFI_OK:
            ffi_call(&cif, (void (*)()) handle, &return_value, ffi_values.data());
            break;
        case FFI_BAD_TYPEDEF:
            throw ForeignCallError(sign.to_string(), "FFI_BAD_TYPEDEF");
        case FFI_BAD_ABI:
            throw ForeignCallError(sign.to_string(), "FFI_BAD_ABI");
        case FFI_BAD_ARGTYPE:
            throw ForeignCallError(sign.to_string(), "FFI_BAD_ARGTYPE");
        default:
            throw Unreachable();
        }

        if (return_value)
            thread->get_state().push(return_value);
    }
}    // namespace spade
