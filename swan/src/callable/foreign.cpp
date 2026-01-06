#include "foreign.hpp"
#include "ee/thread.hpp"
#include "utils/errors.hpp"
#include <ffi.h>

namespace spade
{
    void ObjForeign::call(Obj *self, const vector<Obj *> &args) {
        validate_call_site();

        const uint8_t arg_count = sign.get_elements().back().get_params().size() & 0xFF;

        if (arg_count < args.size())
            throw ArgumentError(sign.to_string(), std::format("too less arguments, expected {} got {}", arg_count, args.size()));
        if (arg_count > args.size())
            throw ArgumentError(sign.to_string(), std::format("too many arguments, expected {} got {}", arg_count, args.size()));

        foreign_call(self, args);
    }

    void ObjForeign::call(Obj *self, Obj **args) {
        validate_call_site();

        const uint8_t arg_count = sign.get_elements().back().get_params().size() & 0xFF;
        foreign_call(self, vector(args, args + arg_count));
    }

    void ObjForeign::foreign_call(Obj *self, vector<Obj *> args) {
        // Call the foreign function as follows:
        // If has_self:
        //     handle(thread, self, args);
        // If not has_self:
        //     handle(thread, args)

        auto thread = Thread::current();
        Obj *return_value = null;

        ffi_cif cif;
        const size_t ffi_args_count = has_self ? 2 + args.size() : 1 + args.size();
        const auto ffi_arg_types = std::make_unique<ffi_type *[]>(ffi_args_count);
        const auto ffi_values = std::make_unique<void *[]>(ffi_args_count);

        // Fill arg types
        for (size_t i = 0; i < ffi_args_count; i++) {
            ffi_arg_types[i] = &ffi_type_pointer;
        }

        {    // Fill arg values
            size_t i = 0;
            ffi_values[i++] = &thread;
            if (has_self)
                ffi_values[i++] = &self;
            for (size_t j = 0; j < args.size(); j++) {
                ffi_values[i + j] = &args[j];
            }
        }

        const auto result = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, ffi_args_count, &ffi_type_pointer, &ffi_arg_types[0]);
        switch (result) {
        case FFI_OK:
            ffi_call(&cif, (void (*)()) handle, &return_value, &ffi_values[0]);
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

        if (return_value) {
            thread->get_state().push(return_value);
        }
    }
}    // namespace spade
