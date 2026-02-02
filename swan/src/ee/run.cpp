#include "ee/obj.hpp"
#include "spimp/utils.hpp"
#include "vm.hpp"
#include "memory/memory.hpp"
#include <cstdint>
#include <iostream>
#include <sputils.hpp>

namespace spade
{
    Value SpadeVM::run(Thread *thread) {
        auto &state = thread->get_state();
        while (thread->is_running()) {
            const auto opcode = static_cast<Opcode>(state.read_byte());
            auto frame = state.get_frame();
            if (debugger)
                debugger->update(this);

            try {
                switch (opcode) {
                case Opcode::NOP:
                    // Do nothing
                    break;
                case Opcode::CONST:
                    state.push(state.load_const(state.read_byte()));
                    break;
                case Opcode::CONST_NULL:
                    state.push(Value());
                    break;
                case Opcode::CONST_TRUE:
                    state.push(Value(true));
                    break;
                case Opcode::CONST_FALSE:
                    state.push(Value(false));
                    break;
                case Opcode::CONSTL:
                    state.push(state.load_const(state.read_short()));
                    break;
                case Opcode::POP:
                    state.pop();
                    break;
                case Opcode::NPOP:
                    frame->sc -= state.read_byte();
                    break;
                case Opcode::DUP:
                    state.push(state.peek());
                    break;
                case Opcode::NDUP: {
                    const uint8_t count = state.read_byte();
                    const auto value = frame->peek();
                    for (uint8_t i = 0; i < count; ++i) {
                        frame->stack[frame->sc + i] = value;
                    }
                    frame->sc += count;
                    break;
                }
                case Opcode::GLOAD:
                    state.push(get_symbol(state.load_const(state.read_short()).to_string()));
                    break;
                case Opcode::GSTORE:
                    set_symbol(state.load_const(state.read_short()).to_string(), state.peek());
                    break;
                case Opcode::LLOAD:
                    state.push(frame->get_local(state.read_short()));
                    break;
                case Opcode::LSTORE:
                    frame->set_local(state.read_short(), state.peek());
                    break;
                case Opcode::GFLOAD:
                    state.push(get_symbol(state.load_const(state.read_byte()).to_string()));
                    break;
                case Opcode::GFSTORE:
                    set_symbol(state.load_const(state.read_byte()).to_string(), state.peek());
                    break;
                case Opcode::LFLOAD:
                    state.push(frame->get_local(state.read_byte()));
                    break;
                case Opcode::LFSTORE:
                    frame->set_local(state.read_byte(), state.peek());
                    break;
                case Opcode::PGSTORE:
                    set_symbol(state.load_const(state.read_short()).to_string(), state.pop());
                    break;
                case Opcode::PLSTORE:
                    frame->set_local(state.read_short(), state.pop());
                    break;
                case Opcode::PGFSTORE:
                    set_symbol(state.load_const(state.read_byte()).to_string(), state.pop());
                    break;
                case Opcode::PLFSTORE:
                    frame->set_local(state.read_byte(), state.pop());
                    break;
                case Opcode::ALOAD:
                    state.push(frame->get_arg(state.read_byte()));
                    break;
                case Opcode::ASTORE:
                    frame->set_arg(state.read_byte(), state.peek());
                    break;
                case Opcode::PASTORE:
                    frame->set_arg(state.read_byte(), state.pop());
                    break;
                case Opcode::MLOAD: {
                    const auto object = state.pop().as_obj();
                    const auto name = Sign(state.load_const(state.read_short()).to_string()).get_name();
                    const auto member = object->get_member(name);
                    state.push(member);
                    break;
                }
                case Opcode::MSTORE: {
                    const auto object = state.pop().as_obj();
                    const auto value = state.peek();
                    const auto name = Sign(state.load_const(state.read_short()).to_string()).get_name();
                    object->set_member(name, value);
                    break;
                }
                case Opcode::MFLOAD: {
                    const auto object = state.pop().as_obj();
                    const auto name = Sign(state.load_const(state.read_byte()).to_string()).get_name();
                    const auto member = object->get_member(name);
                    state.push(member);
                    break;
                }
                case Opcode::MFSTORE: {
                    const auto object = state.pop().as_obj();
                    const auto value = state.peek();
                    const auto name = Sign(state.load_const(state.read_byte()).to_string()).get_name();
                    object->set_member(name, value);
                    break;
                }
                case Opcode::PMSTORE: {
                    const auto object = state.pop().as_obj();
                    const auto value = state.pop();
                    const auto name = Sign(state.load_const(state.read_short()).to_string()).get_name();
                    object->set_member(name, value);
                    break;
                }
                case Opcode::PMFSTORE: {
                    const auto object = state.pop().as_obj();
                    const auto value = state.pop();
                    const auto name = Sign(state.load_const(state.read_byte()).to_string()).get_name();
                    object->set_member(name, value);
                    break;
                }
                case Opcode::OBJLOAD: {
                    const auto type = cast<Type>(state.pop().as_obj());
                    const auto object = halloc_mgr<Obj>(manager, type);
                    state.push(object);
                    break;
                }
                case Opcode::ARRUNPACK: {
                    const auto array = cast<ObjArray>(state.pop().as_obj());
                    array->for_each([&state](const auto item) { state.push(item); });
                    break;
                }
                case Opcode::ARRPACK: {
                    const uint8_t count = state.read_byte();
                    const auto array = halloc_mgr<ObjArray>(manager, count);
                    frame->sc -= count;
                    for (size_t i = 0; i < count; ++i) {
                        array->set(i, frame->stack[frame->sc + i]);
                    }
                    state.push(array);
                    break;
                }
                case Opcode::ARRBUILD: {
                    const auto count = state.read_short();
                    const auto array = halloc_mgr<ObjArray>(manager, count);
                    state.push(array);
                    break;
                }
                case Opcode::ARRFBUILD: {
                    const uint8_t count = state.read_byte();
                    const auto array = halloc_mgr<ObjArray>(manager, count);
                    state.push(array);
                    break;
                }
                case Opcode::ILOAD: {
                    const auto array = cast<ObjArray>(state.pop().as_obj());
                    const auto index = state.pop().as_int();
                    state.push(array->get(index));
                    break;
                }
                case Opcode::ISTORE: {
                    const auto array = cast<ObjArray>(state.pop().as_obj());
                    const auto index = state.pop().as_int();
                    const auto value = state.peek();
                    array->set(index, value);
                    break;
                }
                case Opcode::PISTORE: {
                    const auto array = cast<ObjArray>(state.pop().as_obj());
                    const auto index = state.pop().as_int();
                    const auto value = state.pop();
                    array->set(index, value);
                    break;
                }
                case Opcode::ARRLEN: {
                    const auto array = cast<ObjArray>(state.pop().as_obj());
                    state.push(Value(array->count()));
                    break;
                }
                case Opcode::INVOKE: {
                    // Get the count
                    const uint8_t count = state.read_byte();
                    // Pop the arguments
                    frame->sc -= count;
                    // Get the method
                    const auto method = cast<ObjMethod>(state.pop().as_obj());
                    // Call it
                    method->call(null, &frame->stack[frame->sc + 1]);
                    break;
                }
                case Opcode::VINVOKE: {
                    // Get the param
                    const Sign sign{state.load_const(state.read_short()).to_string()};
                    // Get name of the method
                    const auto name = sign.get_name();
                    // Get the arg count
                    const uint8_t count = static_cast<uint8_t>(sign.get_params().size());

                    // Pop the arguments
                    frame->sc -= count;
                    // Get the object
                    const auto object = state.pop().as_obj();
                    // Get the method
                    const auto method = cast<ObjMethod>(object->get_member(name).as_obj());
                    // Call it
                    method->call(null, &frame->stack[frame->sc + 1]);
                    // Set this
                    break;
                }
                case Opcode::SPINVOKE: {
                    const auto method = cast<ObjMethod>(get_symbol(state.load_const(state.read_short()).to_string()).as_obj());
                    const uint8_t count = method->get_args_count();
                    frame->sc -= count;
                    Obj *obj = state.pop().as_obj();
                    method->call(null, &frame->stack[frame->sc + 1]);
                    state.get_frame()->set_local(0, obj);
                    break;
                }
                case Opcode::SPFINVOKE: {
                    const auto method = cast<ObjMethod>(get_symbol(state.load_const(state.read_byte()).to_string()).as_obj());
                    const uint8_t count = method->get_args_count();
                    frame->sc -= count;
                    Obj *obj = state.pop().as_obj();
                    method->call(null, &frame->stack[frame->sc + 1]);
                    state.get_frame()->set_local(0, obj);
                    break;
                }
                case Opcode::LINVOKE: {
                    // Get the method
                    const auto method = cast<ObjMethod>(frame->get_local(state.read_short()).as_obj());
                    // Get the arg count
                    const uint8_t count = method->get_args_count();
                    // Pop the arguments
                    frame->sc -= count;
                    // Call it
                    method->call(null, &frame->stack[frame->sc]);
                    break;
                }
                case Opcode::GINVOKE: {
                    // Get the method
                    const auto method = cast<ObjMethod>(get_symbol(state.load_const(state.read_short()).to_string()).as_obj());
                    // Get the arg count
                    const uint8_t count = method->get_args_count();
                    // Pop the arguments
                    frame->sc -= count;
                    // Call it
                    method->call(null, &frame->stack[frame->sc]);
                    break;
                }
                case Opcode::VFINVOKE: {
                    // Get the param
                    const Sign sign{state.load_const(state.read_byte()).to_string()};
                    // Get name of the method
                    const auto name = sign.get_name();
                    // Get the arg count
                    const uint8_t count = static_cast<uint8_t>(sign.get_params().size());

                    // Pop the arguments
                    frame->sc -= count;
                    // Get the object
                    const auto object = state.pop().as_obj();
                    // Get the method
                    const auto method = cast<ObjMethod>(object->get_member(name).as_obj());
                    // Call it
                    method->call(null, &frame->stack[frame->sc + 1]);
                    // Set this
                    state.get_frame()->set_local(0, object);
                    break;
                }
                case Opcode::LFINVOKE: {
                    // Get the method
                    const auto method = cast<ObjMethod>(frame->get_local(state.read_byte()).as_obj());
                    // Get the arg count
                    const uint8_t count = method->get_args_count();
                    // Pop the arguments
                    frame->sc -= count;
                    // Call it
                    method->call(null, &frame->stack[frame->sc]);
                    break;
                }
                case Opcode::GFINVOKE: {
                    // Get the method
                    const auto method = cast<ObjMethod>(get_symbol(state.load_const(state.read_byte()).to_string()).as_obj());
                    // Get the arg count
                    const uint8_t count = method->get_args_count();
                    // Pop the arguments
                    frame->sc -= count;
                    // Call it
                    method->call(null, &frame->stack[frame->sc]);
                    break;
                }
                case Opcode::AINVOKE: {
                    // Get the method
                    const auto method = cast<ObjMethod>(frame->get_arg(state.read_byte()).as_obj());
                    // Get the arg count
                    const uint8_t count = method->get_args_count();
                    // Pop the arguments
                    frame->sc -= count;
                    // Call it
                    method->call(null, &frame->stack[frame->sc]);
                    break;
                }
                case Opcode::CALLSUB: {
                    // Get target offset
                    const int16_t offset = static_cast<int16_t>(state.read_short());
                    // Get current pc
                    const Value address(frame->pc);
                    // Save it on the stack as return address
                    state.push(address);
                    // Now go to the target location
                    state.adjust(offset);
                    break;
                }
                case Opcode::RETSUB: {
                    // Get the return address
                    const auto address = state.pop();
                    // Go to the return location
                    state.set_pc(address.as_int());
                    break;
                }
                case Opcode::JMP: {
                    const int16_t offset = static_cast<int16_t>(state.read_short());
                    state.adjust(offset);
                    break;
                }
                case Opcode::JT: {
                    const auto obj = state.pop();
                    const int16_t offset = static_cast<int16_t>(state.read_short());
                    if (obj)
                        state.adjust(offset);
                    break;
                }
                case Opcode::JF: {
                    const auto obj = state.pop();
                    const int16_t offset = static_cast<int16_t>(state.read_short());
                    if (!obj)
                        state.adjust(offset);
                    break;
                }
                case Opcode::JLT: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    const int16_t offset = static_cast<int16_t>(state.read_short());
                    if (a < b)
                        state.adjust(offset);
                    break;
                }
                case Opcode::JLE: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    const int16_t offset = static_cast<int16_t>(state.read_short());
                    if (a <= b)
                        state.adjust(offset);
                    break;
                }
                case Opcode::JEQ: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    const int16_t offset = static_cast<int16_t>(state.read_short());
                    if (a == b)
                        state.adjust(offset);
                    break;
                }
                case Opcode::JNE: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    const int16_t offset = static_cast<int16_t>(state.read_short());
                    if (a != b)
                        state.adjust(offset);
                    break;
                }
                case Opcode::JGE: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    const int16_t offset = static_cast<int16_t>(state.read_short());
                    if (a >= b)
                        state.adjust(offset);
                    break;
                }
                case Opcode::JGT: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    const int16_t offset = static_cast<int16_t>(state.read_short());
                    if (a > b)
                        state.adjust(offset);
                    break;
                }
                case Opcode::NOT:
                    state.push(!state.pop());
                    break;
                case Opcode::INV:
                    state.push(~state.pop());
                    break;
                case Opcode::NEG:
                    state.push(-state.pop());
                    break;
                case Opcode::GETTYPE:
                    // TODO: What about primitive types
                    state.push(state.pop().as_obj()->get_type());
                    break;
                case Opcode::SCAST: {
                    const auto type = cast<Type>(state.pop().as_obj());
                    const auto obj = state.pop().as_obj();
                    if (check_cast(obj, type))
                        // obj->set_type(type); // Types are dynamic
                        state.push(obj);
                    else
                        state.push(Value());
                    break;
                }
                case Opcode::CCAST: {
                    const auto type = cast<Type>(state.pop().as_obj());
                    const auto obj = state.pop().as_obj();
                    if (check_cast(obj, type))
                        // obj->set_type(type); // Types are dynamic
                        state.push(obj);
                    else
                        runtime_error(std::format("object of type '{}' cannot be cast to object of type '{}'",
                                                  obj->get_type()->get_sign().to_string(), type->get_sign().to_string()));
                    break;
                }
                case Opcode::CONCAT: {
                    const auto b = cast<ObjString>(state.pop().as_obj());
                    const auto a = cast<ObjString>(state.pop().as_obj());
                    state.push(a->concat(b));
                    break;
                }
                case Opcode::POW: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a.power(b));
                    break;
                }
                case Opcode::MUL: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a * b);
                    break;
                }
                case Opcode::DIV: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a / b);
                    break;
                }
                case Opcode::REM: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a % b);
                    break;
                }
                case Opcode::ADD: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a + b);
                    break;
                }
                case Opcode::SUB: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a - b);
                    break;
                }
                case Opcode::SHL: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a << b);
                    break;
                }
                case Opcode::SHR: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a >> b);
                    break;
                }
                case Opcode::USHR: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a.unsigned_right_shift(b));
                    break;
                }
                case Opcode::ROL: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a.rotate_left(b));
                    break;
                }
                case Opcode::ROR: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a.rotate_right(b));
                    break;
                }
                case Opcode::AND: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a & b);
                    break;
                }
                case Opcode::OR: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a | b);
                    break;
                }
                case Opcode::XOR: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a ^ b);
                    break;
                }
                case Opcode::LT: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a < b);
                    break;
                }
                case Opcode::LE: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a <= b);
                    break;
                }
                case Opcode::EQ: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a == b);
                    break;
                }
                case Opcode::NE: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a != b);
                    break;
                }
                case Opcode::GE: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a >= b);
                    break;
                }
                case Opcode::GT: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(a > b);
                    break;
                }
                case Opcode::IS: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    if (a.is_obj() && b.is_obj()) {
                        state.push(Value(a.as_obj() == b.as_obj()));
                    } else
                        state.push(a == b);
                    break;
                }
                case Opcode::NIS: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    if (a.is_obj() && b.is_obj()) {
                        state.push(Value(a.as_obj() != b.as_obj()));
                    } else
                        state.push(a != b);
                    break;
                }
                case Opcode::ISNULL:
                    state.push(Value(state.pop().is_null()));
                    break;
                case Opcode::NISNULL:
                    state.push(!Value(state.pop().is_null()));
                    break;
                case Opcode::ENTERMONITOR:
                    state.pop().as_obj()->enter_monitor();
                    break;
                case Opcode::EXITMONITOR:
                    state.pop().as_obj()->exit_monitor();
                    break;
                case Opcode::MTPERF: {
                    const auto match = frame->get_method()->get_matches()[state.read_short()];
                    const uint32_t offset = match.perform(state.pop());
                    state.set_pc(offset);
                    break;
                }
                case Opcode::MTFPERF: {
                    const auto match = frame->get_method()->get_matches()[state.read_byte()];
                    const uint32_t offset = match.perform(state.pop());
                    state.set_pc(offset);
                    break;
                }
                case Opcode::CLOSURELOAD: {
                    const uint8_t capture_count = state.read_byte();
                    const auto method = cast<ObjMethod>(state.pop().as_obj())->force_copy();
                    for (uint8_t i = 0; i < capture_count; i++) {
                        const uint16_t local_index = state.read_short();
                        ObjCapture *capture;
                        switch (state.read_byte()) {
                        case 0x00:
                            capture = frame->ramp_up_arg(state.read_byte());
                            break;
                        case 0x01:
                            capture = frame->ramp_up_local(state.read_short());
                            break;
                        default:
                            throw Unreachable();
                        }
                        method->set_capture(local_index, capture);
                    }
                    state.push(method);
                    break;
                }
                case Opcode::THROW: {
                    const auto value = state.pop();
                    throw ThrowSignal(value);
                }
                case Opcode::RET: {
                    const auto current_frame = state.get_frame();
                    // Pop the return value
                    const auto val = state.pop();
                    // Pop the current frame
                    state.pop_frame();
                    // Return if encountered end of execution
                    if (state.get_call_stack_size() == 0)
                        return val;
                    // Push the return value
                    state.get_frame()->push(val);
                    break;
                }
                case Opcode::VRET: {
                    const auto current_frame = state.get_frame();
                    // Pop the current frame
                    state.pop_frame();
                    // Return if encountered end of execution
                    if (state.get_call_stack_size() == 0)
                        return Value();
                    break;
                }
                case Opcode::PRINTLN:
                    write(state.pop().to_string() + "\n");
                    break;
                case Opcode::I2U:
                    state.push(Value(static_cast<uint64_t>(state.pop().as_int())));
                    break;
                case Opcode::U2I:
                    state.push(Value(static_cast<int64_t>(state.pop().as_uint())));
                    break;
                case Opcode::U2F:
                    state.push(Value(static_cast<double>(state.pop().as_uint())));
                    break;
                case Opcode::I2F:
                    state.push(Value(static_cast<double>(state.pop().as_int())));
                    break;
                case Opcode::F2I:
                    state.push(Value(static_cast<int64_t>(state.pop().as_float())));
                    break;
                case Opcode::I2B:
                    state.push(Value(state.pop().as_int() != 0));
                    break;
                case Opcode::B2I:
                    state.push(Value(static_cast<int64_t>(state.pop().as_bool() ? 1 : 0)));
                    break;
                case Opcode::O2B:
                    state.push(Value(state.pop().truth()));
                    break;
                case Opcode::O2S:
                    state.push(halloc_mgr<ObjString>(manager, state.pop().to_string()));
                    break;
                }
            } catch (const ThrowSignal &signal) {
                const auto value = signal.get_value();
                while (state.get_call_stack_size() > 0) {
                    frame = state.get_frame();
                    const auto info = frame->get_method()->get_exceptions().get_target(state.get_pc(), value.as_obj()->get_type());
                    if (Exception::IS_NO_EXCEPTION(info))
                        state.pop_frame();
                    else {
                        state.set_pc(info.get_target());
                        state.push(value);
                        break;
                    }
                }
                if (state.get_call_stack_size() == 0) {
                    // TODO: show stack trace
                }
            } catch (const FatalError &error) {
                std::cerr << "fatal error: " << error.what() << std::endl;
                std::exit(1);
            }
        }

#if 1
        throw Unreachable();
#else
        return Value();
#endif
    }
}    // namespace spade
