#include "memory/memory.hpp"
#include "objects/inbuilt_types.hpp"
#include "vm.hpp"

#include "debug/debug.hpp"
#include "objects/float.hpp"
#include "objects/int.hpp"
#include "objects/typeparam.hpp"

namespace spade
{
    Obj *SpadeVM::run(Thread *thread) {
        auto state = thread->get_state();
        auto topFrame = state->get_frame();
        while (thread->is_running()) {
            auto opcode = static_cast<Opcode>(state->read_byte());
            auto frame = state->get_frame();
            DebugOp::print_vm_state(state);
            try {
                switch (opcode) {
                    case Opcode::NOP:
                        // Do nothing
                        break;
                    case Opcode::CONST:
                        state->push(state->load_const(state->read_byte()));
                        break;
                    case Opcode::CONST_NULL:
                        state->push(ObjNull::value());
                        break;
                    case Opcode::CONST_TRUE:
                        state->push(ObjBool::value(true));
                        break;
                    case Opcode::CONST_FALSE:
                        state->push(ObjBool::value(false));
                        break;
                    case Opcode::CONSTL:
                        state->push(state->load_const(state->read_short()));
                        break;
                    case Opcode::POP:
                        state->pop();
                        break;
                    case Opcode::NPOP: {
                        uint8 count = (uint8) state->read_byte();
                        frame->sp -= count;
                        break;
                    }
                    case Opcode::DUP:
                        state->push(state->peek());
                        break;
                    case Opcode::NDUP: {
                        uint8 count = (uint8) state->read_byte();
                        for (int i = 0; i < count; ++i) {
                            frame->sp[i] = frame->sp[-1];
                        }
                        frame->sp += count;
                        break;
                    }
                    case Opcode::GLOAD:
                        state->push(get_symbol(state->load_const(state->read_short())->to_string()));
                        break;
                    case Opcode::GSTORE:
                        set_symbol(state->load_const(state->read_short())->to_string(), state->peek());
                        break;
                    case Opcode::LLOAD:
                        state->push(frame->get_locals().get(state->read_short()));
                        break;
                    case Opcode::LSTORE:
                        frame->get_locals().set(state->read_short(), state->peek());
                        break;
                    case Opcode::SPLOAD: {
                        auto obj = state->pop();
                        auto sign = state->load_const(state->read_short())->to_string();
                        state->push(obj->get_super_class_method(sign));
                        break;
                    }
                    case Opcode::GFLOAD:
                        state->push(get_symbol(state->load_const(state->read_byte())->to_string()));
                        break;
                    case Opcode::GFSTORE:
                        set_symbol(state->load_const(state->read_byte())->to_string(), state->peek());
                        break;
                    case Opcode::LFLOAD:
                        state->push(frame->get_locals().get(state->read_byte()));
                        break;
                    case Opcode::LFSTORE:
                        frame->get_locals().set(state->read_byte(), state->peek());
                        break;
                    case Opcode::SPFLOAD: {
                        auto obj = state->pop();
                        auto sign = state->load_const(state->read_byte())->to_string();
                        state->push(obj->get_super_class_method(sign));
                        break;
                    }
                    case Opcode::PGSTORE:
                        set_symbol(state->load_const(state->read_short())->to_string(), state->pop());
                        break;
                    case Opcode::PLSTORE:
                        frame->get_locals().set(state->read_short(), state->pop());
                        break;
                    case Opcode::PGFSTORE:
                        set_symbol(state->load_const(state->read_byte())->to_string(), state->pop());
                        break;
                    case Opcode::PLFSTORE:
                        frame->get_locals().set(state->read_byte(), state->pop());
                        break;
                    case Opcode::ALOAD:
                        state->push(frame->get_args().get(state->read_byte()));
                        break;
                    case Opcode::ASTORE:
                        frame->get_args().set(state->read_byte(), state->peek());
                        break;
                    case Opcode::PASTORE:
                        frame->get_args().set(state->read_byte(), state->pop());
                        break;
                    case Opcode::TLOAD:
                        state->push(frame->get_method()->get_type_param(state->load_const(state->read_short())->to_string()));
                        break;
                    case Opcode::TFLOAD:
                        state->push(frame->get_method()->get_type_param(state->load_const(state->read_byte())->to_string()));
                        break;
                    case Opcode::TSTORE:
                        frame->get_method()
                                ->get_type_param(state->load_const(state->read_short())->to_string())
                                ->set_placeholder(cast<Type>(state->peek()));
                        break;
                    case Opcode::TFSTORE:
                        frame->get_method()
                                ->get_type_param(state->load_const(state->read_byte())->to_string())
                                ->set_placeholder(cast<Type>(state->peek()));
                        break;
                    case Opcode::PTSTORE:
                        frame->get_method()
                                ->get_type_param(state->load_const(state->read_short())->to_string())
                                ->set_placeholder(cast<Type>(state->pop()));
                        break;
                    case Opcode::PTFSTORE:
                        frame->get_method()
                                ->get_type_param(state->load_const(state->read_byte())->to_string())
                                ->set_placeholder(cast<Type>(state->pop()));
                        break;
                    case Opcode::MLOAD: {
                        auto object = state->pop();
                        auto name = Sign(state->load_const(state->read_short())->to_string()).get_name();
                        Obj *member = object->get_member(name);
                        state->push(member);
                        break;
                    }
                    case Opcode::MSTORE: {
                        auto object = state->pop();
                        auto value = state->peek();
                        auto name = Sign(state->load_const(state->read_short())->to_string()).get_name();
                        object->set_member(name, value);
                        break;
                    }
                    case Opcode::MFLOAD: {
                        auto object = state->pop();
                        auto name = Sign(state->load_const(state->read_byte())->to_string()).get_name();
                        Obj *member = object->get_member(name);
                        state->push(member);
                        break;
                    }
                    case Opcode::MFSTORE: {
                        auto object = state->pop();
                        auto value = state->peek();
                        auto name = Sign(state->load_const(state->read_byte())->to_string()).get_name();
                        object->set_member(name, value);
                        break;
                    }
                    case Opcode::PMSTORE: {
                        auto object = state->pop();
                        auto value = state->pop();
                        auto name = Sign(state->load_const(state->read_short())->to_string()).get_name();
                        object->set_member(name, value);
                        break;
                    }
                    case Opcode::PMFSTORE: {
                        auto object = state->pop();
                        auto value = state->pop();
                        auto name = Sign(state->load_const(state->read_byte())->to_string()).get_name();
                        object->set_member(name, value);
                        break;
                    }
                    case Opcode::OBJLOAD: {
                        auto type = cast<Type>(state->pop());
                        auto object = halloc_mgr<Obj>(manager, Sign(""), type, frame->get_method()->get_module());
                        state->push(object);
                        break;
                    }
                    case Opcode::ARRUNPACK: {
                        auto array = cast<ObjArray>(state->pop());
                        array->foreach ([&state](auto item) { state->push(item); });
                        break;
                    }
                    case Opcode::ARRPACK: {
                        uint8 count = state->read_byte();
                        auto array = halloc_mgr<ObjArray>(manager, count);
                        frame->sp -= count;
                        for (int i = 0; i < count; ++i) {
                            array->set(i, frame->sp[i]);
                        }
                        state->push(array);
                        break;
                    }
                    case Opcode::ARRBUILD: {
                        uint8 count = static_cast<uint8>(state->read_short());
                        auto array = halloc_mgr<ObjArray>(manager, count);
                        state->push(array);
                        break;
                    }
                    case Opcode::ARRFBUILD: {
                        uint8 count = (uint8) state->read_byte();
                        auto array = halloc_mgr<ObjArray>(manager, count);
                        state->push(array);
                        break;
                    }
                    case Opcode::ILOAD: {
                        auto array = cast<ObjArray>(state->pop());
                        auto index = cast<ObjInt>(state->pop());
                        state->push(array->get(index->value()));
                        break;
                    }
                    case Opcode::ISTORE: {
                        auto array = cast<ObjArray>(state->pop());
                        auto index = cast<ObjInt>(state->pop());
                        auto value = state->peek();
                        array->set(index->value(), value);
                        break;
                    }
                    case Opcode::PISTORE: {
                        auto array = cast<ObjArray>(state->pop());
                        auto index = cast<ObjInt>(state->pop());
                        auto value = state->pop();
                        array->set(index->value(), value);
                        break;
                    }
                    case Opcode::ARRLEN: {
                        auto array = cast<ObjArray>(state->pop());
                        state->push(halloc_mgr<ObjInt>(manager, array->count()));
                        break;
                    }
                    case Opcode::INVOKE: {
                        // Get the count
                        uint8 count = state->read_byte();
                        // Pop the arguments
                        frame->sp -= count;
                        // Get the method
                        auto method = cast<ObjMethod>(state->pop());
                        // Call it
                        method->call(frame->sp + 1);
                        break;
                    }
                    case Opcode::VINVOKE: {
                        // Get the param
                        Sign sign{state->load_const(state->read_short())->to_string()};
                        // Get name of the method
                        auto name = sign.get_name();
                        // Get the arg count
                        uint8 count = static_cast<uint8>(sign.get_params().size());

                        // Pop the arguments
                        frame->sp -= count;
                        // Get the object
                        auto object = state->pop();
                        // Get the method
                        auto method = cast<ObjMethod>(object->get_member(name));
                        // Call it
                        method->call(frame->sp + 1);
                        break;
                    }
                    case Opcode::SPINVOKE: {
                        auto method = cast<ObjMethod>(get_symbol(state->load_const(state->read_short())->to_string()));
                        uint8 count = method->get_frame_template().get_args().count();
                        frame->sp -= count;
                        Obj *obj = state->pop();
                        method->call(frame->sp + 1);
                        state->get_frame()->get_locals().set(0, obj);
                        break;
                    }
                    case Opcode::SPFINVOKE: {
                        auto method = cast<ObjMethod>(get_symbol(state->load_const(state->read_byte())->to_string()));
                        uint8 count = method->get_frame_template().get_args().count();
                        frame->sp -= count;
                        Obj *obj = state->pop();
                        method->call(frame->sp + 1);
                        state->get_frame()->get_locals().set(0, obj);
                        break;
                    }
                    case Opcode::LINVOKE: {
                        // Get the method
                        auto method = cast<ObjMethod>(frame->get_locals().get(state->read_short()));
                        // Get the arg count
                        uint8 count = (uint8) method->get_frame_template().get_args().count();
                        // Pop the arguments
                        frame->sp -= count;
                        // Call it
                        method->call(frame->sp);
                        break;
                    }
                    case Opcode::GINVOKE: {
                        // Get the method
                        auto method = cast<ObjMethod>(get_symbol(state->load_const(state->read_short())->to_string()));
                        // Get the arg count
                        uint8 count = (uint8) method->get_frame_template().get_args().count();
                        // Pop the arguments
                        frame->sp -= count;
                        // Call it
                        method->call(frame->sp);
                        break;
                    }
                    case Opcode::VFINVOKE: {
                        // Get the param
                        Sign sign{state->load_const(state->read_byte())->to_string()};
                        // Get name of the method
                        auto name = sign.get_name();
                        // Get the arg count
                        uint8 count = static_cast<uint8>(sign.get_params().size());

                        // Pop the arguments
                        frame->sp -= count;
                        // Get the object
                        auto object = state->pop();
                        // Get the method
                        auto method = cast<ObjMethod>(object->get_member(name));
                        // Call it
                        method->call(frame->sp + 1);
                        break;
                    }
                    case Opcode::LFINVOKE: {
                        // Get the method
                        auto method = cast<ObjMethod>(frame->get_locals().get(state->read_byte()));
                        // Get the arg count
                        uint8 count = (uint8) method->get_frame_template().get_args().count();
                        // Pop the arguments
                        frame->sp -= count;
                        // Call it
                        method->call(frame->sp);
                        break;
                    }
                    case Opcode::GFINVOKE: {
                        // Get the method
                        auto method = cast<ObjMethod>(get_symbol(state->load_const(state->read_byte())->to_string()));
                        // Get the arg count
                        uint8 count = (uint8) method->get_frame_template().get_args().count();
                        // Pop the arguments
                        frame->sp -= count;
                        // Call it
                        method->call(frame->sp);
                        break;
                    }
                    case Opcode::AINVOKE: {
                        // Get the method
                        auto method = cast<ObjMethod>(frame->get_args().get(state->read_byte()));
                        // Get the arg count
                        uint8 count = (uint8) method->get_frame_template().get_args().count();
                        // Pop the arguments
                        frame->sp -= count;
                        // Call it
                        method->call(frame->sp);
                        break;
                    }
                    case Opcode::CALLSUB: {
                        auto address = halloc_mgr<ObjInt>(manager, frame->ip - frame->code);
                        state->push(address);
                        auto offset = state->read_short();
                        state->adjust(offset);
                        break;
                    }
                    case Opcode::RETSUB: {
                        auto address = cast<ObjInt>(state->pop());
                        frame->set_ip(frame->code + address->value());
                        break;
                    }
                    case Opcode::JMP: {
                        int16 offset = static_cast<int16>(state->read_short());
                        state->adjust(offset);
                        break;
                    }
                    case Opcode::JT: {
                        auto obj = state->pop();
                        int16 offset = static_cast<int16>(state->read_short());
                        if (obj->truth())
                            state->adjust(offset);
                        break;
                    }
                    case Opcode::JF: {
                        auto obj = state->pop();
                        int16 offset = static_cast<int16>(state->read_short());
                        if (!obj->truth())
                            state->adjust(offset);
                        break;
                    }
                    case Opcode::JLT: {
                        auto b = cast<ComparableObj>(state->pop());
                        auto a = cast<ComparableObj>(state->pop());
                        int16 offset = static_cast<int16>(state->read_short());
                        if ((*a < b)->truth())
                            state->adjust(offset);
                        break;
                    }
                    case Opcode::JLE: {
                        auto b = cast<ComparableObj>(state->pop());
                        auto a = cast<ComparableObj>(state->pop());
                        int16 offset = static_cast<int16>(state->read_short());
                        if ((*a <= b)->truth())
                            state->adjust(offset);
                        break;
                    }
                    case Opcode::JEQ: {
                        auto b = cast<ComparableObj>(state->pop());
                        auto a = cast<ComparableObj>(state->pop());
                        int16 offset = static_cast<int16>(state->read_short());
                        if ((*a == b)->truth())
                            state->adjust(offset);
                        break;
                    }
                    case Opcode::JNE: {
                        auto b = cast<ComparableObj>(state->pop());
                        auto a = cast<ComparableObj>(state->pop());
                        int16 offset = static_cast<int16>(state->read_short());
                        if ((*a != b)->truth())
                            state->adjust(offset);
                        break;
                    }
                    case Opcode::JGE: {
                        auto b = cast<ComparableObj>(state->pop());
                        auto a = cast<ComparableObj>(state->pop());
                        int16 offset = static_cast<int16>(state->read_short());
                        if ((*a >= b)->truth())
                            state->adjust(offset);
                        break;
                    }
                    case Opcode::JGT: {
                        auto b = cast<ComparableObj>(state->pop());
                        auto a = cast<ComparableObj>(state->pop());
                        int16 offset = static_cast<int16>(state->read_short());
                        if ((*a > b)->truth())
                            state->adjust(offset);
                        break;
                    }
                    case Opcode::NOT:
                        state->push(!*cast<ObjBool>(state->pop()));
                        break;
                    case Opcode::INV:
                        state->push(~*cast<ObjInt>(state->pop()));
                        break;
                    case Opcode::NEG:
                        state->push(-*cast<ObjInt>(state->pop()));
                        break;
                    case Opcode::GETTYPE:
                        state->push(state->pop()->get_type());
                        break;
                    case Opcode::SCAST: {
                        auto type = cast<Type>(state->pop());
                        auto obj = state->pop();
                        if (check_cast(obj->get_type(), type)) {
                            obj->set_type(type);
                            state->push(obj);
                        } else
                            state->push(ObjNull::value());
                        break;
                    }
                    case Opcode::CCAST: {
                        auto type = cast<Type>(state->pop());
                        auto obj = state->pop();
                        if (check_cast(obj->get_type(), type)) {
                            obj->set_type(type);
                            state->push(obj);
                        } else
                            runtime_error(std::format("object of type '{}' cannot be cast to object of type '{}'",
                                                      obj->get_type()->get_sign().to_string(), type->get_sign().to_string()));
                        break;
                    }
                    case Opcode::CONCAT: {
                        auto b = cast<ObjString>(state->pop());
                        auto a = cast<ObjString>(state->pop());
                        state->push(halloc_mgr<ObjString>(manager, a->to_string() + b->to_string()));
                        break;
                    }
                    case Opcode::POW: {
                        auto b = cast<ObjNumber>(state->pop());
                        auto a = cast<ObjNumber>(state->pop());
                        state->push(a->power(b));
                        break;
                    }
                    case Opcode::MUL: {
                        auto b = cast<ObjNumber>(state->pop());
                        auto a = cast<ObjNumber>(state->pop());
                        state->push(*a * b);
                        break;
                    }
                    case Opcode::DIV: {
                        auto b = cast<ObjNumber>(state->pop());
                        auto a = cast<ObjNumber>(state->pop());
                        state->push(*a / b);
                        break;
                    }
                    case Opcode::REM: {
                        auto b = cast<ObjInt>(state->pop());
                        auto a = cast<ObjInt>(state->pop());
                        state->push(*a % *b);
                        break;
                    }
                    case Opcode::ADD: {
                        auto b = cast<ObjNumber>(state->pop());
                        auto a = cast<ObjNumber>(state->pop());
                        state->push(*a + b);
                        break;
                    }
                    case Opcode::SUB: {
                        auto b = cast<ObjNumber>(state->pop());
                        auto a = cast<ObjNumber>(state->pop());
                        state->push(*a - b);
                        break;
                    }
                    case Opcode::SHL: {
                        auto b = cast<ObjInt>(state->pop());
                        auto a = cast<ObjInt>(state->pop());
                        state->push(*a << *b);
                        break;
                    }
                    case Opcode::SHR: {
                        auto b = cast<ObjInt>(state->pop());
                        auto a = cast<ObjInt>(state->pop());
                        state->push(*a >> *b);
                        break;
                    }
                    case Opcode::USHR: {
                        auto b = cast<ObjInt>(state->pop());
                        auto a = cast<ObjInt>(state->pop());
                        state->push(a->unsigned_right_shift(*b));
                        break;
                    }
                    case Opcode::AND: {
                        auto b = cast<ObjInt>(state->pop());
                        auto a = cast<ObjInt>(state->pop());
                        state->push(*a & *b);
                        break;
                    }
                    case Opcode::OR: {
                        auto b = cast<ObjInt>(state->pop());
                        auto a = cast<ObjInt>(state->pop());
                        state->push(*a | *b);
                        break;
                    }
                    case Opcode::XOR: {
                        auto b = cast<ObjInt>(state->pop());
                        auto a = cast<ObjInt>(state->pop());
                        state->push(*a ^ *b);
                        break;
                    }
                    case Opcode::LT: {
                        auto b = cast<ComparableObj>(state->pop());
                        auto a = cast<ComparableObj>(state->pop());
                        state->push(*a < b);
                        break;
                    }
                    case Opcode::LE: {
                        auto b = cast<ComparableObj>(state->pop());
                        auto a = cast<ComparableObj>(state->pop());
                        state->push(*a <= b);
                        break;
                    }
                    case Opcode::EQ: {
                        auto b = cast<ComparableObj>(state->pop());
                        auto a = cast<ComparableObj>(state->pop());
                        state->push(*a == b);
                        break;
                    }
                    case Opcode::NE: {
                        auto b = cast<ComparableObj>(state->pop());
                        auto a = cast<ComparableObj>(state->pop());
                        state->push(*a != b);
                        break;
                    }
                    case Opcode::GE: {
                        auto b = cast<ComparableObj>(state->pop());
                        auto a = cast<ComparableObj>(state->pop());
                        state->push(*a >= b);
                        break;
                    }
                    case Opcode::GT: {
                        auto b = cast<ComparableObj>(state->pop());
                        auto a = cast<ComparableObj>(state->pop());
                        state->push(*a > b);
                        break;
                    }
                    case Opcode::IS: {
                        auto b = state->pop();
                        auto a = state->pop();
                        state->push(halloc_mgr<ObjBool>(manager, a == b));
                        break;
                    }
                    case Opcode::NIS: {
                        auto b = state->pop();
                        auto a = state->pop();
                        state->push(halloc_mgr<ObjBool>(manager, a != b));
                        break;
                    }
                    case Opcode::ISNULL:
                        state->push(halloc_mgr<ObjBool>(manager, is<ObjNull>(state->pop())));
                        break;
                    case Opcode::NISNULL:
                        state->push(halloc_mgr<ObjBool>(manager, !is<ObjNull>(state->pop())));
                        break;
                    case Opcode::ENTERMONITOR:
                    case Opcode::EXITMONITOR:
                        // todo: implement
                        break;
                    case Opcode::MTPERF: {
                        auto match = frame->get_matches()[state->read_short()];
                        uint32 offset = match.perform(state->pop());
                        state->adjust(offset);
                        break;
                    }
                    case Opcode::MTFPERF: {
                        auto match = frame->get_matches()[state->read_byte()];
                        uint32 offset = match.perform(state->pop());
                        state->adjust(offset);
                        break;
                    }
                    case Opcode::CLOSURELOAD: {
                        auto method = cast<ObjMethod>(state->pop()->copy());
                        auto &locals = const_cast<LocalsTable &>(method->get_frame_template().get_locals());
                        for (uint16 i = locals.get_closure_start(); i < locals.count(); i++) {
                            NamedRef *ref;
                            switch (state->read_byte()) {
                                case 0x00:    // Arg as closure
                                    ref = &frame->get_args().get_arg(state->read_byte());
                                    break;
                                case 0x01:    // Local as closure
                                    ref = &frame->get_locals().get_local(state->read_short());
                                    break;
                                case 0x02:    // Type param as closure

                                    // TODO: implement this
                                    // ref = frame->get_method()->get_type_param(state->load_const(state->read_short())->to_string());
                                    break;
                                default:
                                    throw Unreachable();
                            }
                            locals.add_closure(ref);
                        }
                        break;
                    }
                    case Opcode::REIFIEDLOAD: {
                        uint8 count = state->read_byte();
                        // Pop the arguments
                        for (int i = 0; i < count; i++) state->pop();
                        auto args = frame->sp;
                        auto obj = state->pop();
                        if (is<ObjMethod>(obj)) {
                            state->push(cast<ObjMethod>(obj)->get_reified(args, count));
                        } else if (is<Type>(obj)) {
                            state->push(cast<Type>(obj)->get_reified(args, count));
                        } else
                            throw runtime_error(std::format("cannot set_placeholder value of type {}", obj->get_type()->to_string()));
                        break;
                    }
                    case Opcode::THROW: {
                        auto value = state->pop();
                        throw ThrowSignal(value);
                    }
                    case Opcode::RET: {
                        auto currentFrame = state->get_frame();
                        // Pop the return value
                        auto val = state->pop();
                        // Pop the current frame
                        state->pop_frame();
                        // Return if encountered end of execution
                        if (topFrame == currentFrame) {
                            return val;
                        }
                        // Push the return value
                        state->get_frame()->push(val);
                        break;
                    }
                    case Opcode::VRET: {
                        auto currentFrame = state->get_frame();
                        // Pop the current frame
                        state->pop_frame();
                        // Return if encountered end of execution
                        if (topFrame == currentFrame) {
                            return ObjNull::value(manager);
                        }
                        break;
                    }
                    case Opcode::PRINTLN:
                        write(state->pop()->to_string() + "\n");
                        break;
                    case Opcode::I2F:
                        state->push(halloc_mgr<ObjFloat>(manager, static_cast<double>(cast<ObjInt>(state->pop())->value())));
                        break;
                    case Opcode::F2I:
                        state->push(halloc_mgr<ObjInt>(manager, static_cast<int64>(cast<ObjFloat>(state->pop())->value())));
                        break;
                    case Opcode::I2B:
                        state->push(ObjBool::value(cast<ObjInt>(state->pop())->value() != 0, manager));
                        break;
                    case Opcode::B2I:
                        state->push(halloc_mgr<ObjInt>(manager, cast<ObjBool>(state->pop())->truth() ? 1 : 0));
                        break;
                    case Opcode::O2B:
                        state->push(ObjBool::value(state->pop()->truth(), manager));
                        break;
                    case Opcode::O2S:
                        state->push(halloc_mgr<ObjString>(manager, state->pop()->to_string()));
                        break;
                }
            } catch (const ThrowSignal &signal) {
                auto value = signal.get_value();
                while (state->get_call_stack_size() > 0) {
                    frame = state->get_frame();
                    auto info = frame->get_exceptions().get_target(state->get_pc(), value->get_type());
                    if (Exception::IS_NO_EXCEPTION(info))
                        state->pop_frame();
                    else {
                        state->set_pc(info.get_target());
                        state->push(value);
                        break;
                    }
                }
                if (state->get_call_stack_size() == 0) {
                    // TODO: show stack trace
                }
            } catch (const FatalError &error) {
                std::cerr << "fatal error: " << error.what() << "\n";
                abort();
            }
        }
        return ObjNull::value(manager);
    }
}    // namespace spade