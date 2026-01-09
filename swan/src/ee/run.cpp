#include "vm.hpp"
#include "memory/memory.hpp"
#include <iostream>
#include <sputils.hpp>

namespace spade
{
    Obj *SpadeVM::run(Thread *thread) {
        auto &state = thread->get_state();
        const auto top_frame = state.get_frame();
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
                    state.push(halloc_mgr<ObjNull>(manager));
                    break;
                case Opcode::CONST_TRUE:
                    state.push(halloc_mgr<ObjBool>(manager, true));
                    break;
                case Opcode::CONST_FALSE:
                    state.push(halloc_mgr<ObjBool>(manager, false));
                    break;
                case Opcode::CONSTL:
                    state.push(state.load_const(state.read_short()));
                    break;
                case Opcode::POP:
                    state.pop();
                    break;
                case Opcode::NPOP:
                    frame->sp -= state.read_byte();
                    break;
                case Opcode::DUP:
                    state.push(state.peek());
                    break;
                case Opcode::NDUP: {
                    const uint8_t count = state.read_byte();
                    for (uint8_t i = 0; i < count; ++i) {
                        frame->sp[i] = frame->sp[-1];
                    }
                    frame->sp += count;
                    break;
                }
                case Opcode::GLOAD:
                    state.push(get_symbol(state.load_const(state.read_short())->to_string()));
                    break;
                case Opcode::GSTORE:
                    set_symbol(state.load_const(state.read_short())->to_string(), state.peek());
                    break;
                case Opcode::LLOAD:
                    state.push(frame->get_locals().get(state.read_short()));
                    break;
                case Opcode::LSTORE:
                    frame->get_locals().set(state.read_short(), state.peek());
                    break;
                case Opcode::SPLOAD: {
                    const auto obj = state.pop();
                    const auto sign = state.load_const(state.read_short())->to_string();
                    const auto method = cast<ObjMethod>(get_symbol(sign)->copy());
                    method->get_frame_template().get_locals().ramp_up(0);
                    method->get_frame_template().get_locals().set(0, obj);
                    state.push(method);
                    break;
                }
                case Opcode::GFLOAD:
                    state.push(get_symbol(state.load_const(state.read_byte())->to_string()));
                    break;
                case Opcode::GFSTORE:
                    set_symbol(state.load_const(state.read_byte())->to_string(), state.peek());
                    break;
                case Opcode::LFLOAD:
                    state.push(frame->get_locals().get(state.read_byte()));
                    break;
                case Opcode::LFSTORE:
                    frame->get_locals().set(state.read_byte(), state.peek());
                    break;
                case Opcode::SPFLOAD: {
                    const auto obj = state.pop();
                    const auto sign = state.load_const(state.read_byte())->to_string();
                    const auto method = cast<ObjMethod>(get_symbol(sign)->copy());
                    method->get_frame_template().get_locals().ramp_up(0);
                    method->get_frame_template().get_locals().set(0, obj);
                    state.push(method);
                    break;
                }
                case Opcode::PGSTORE:
                    set_symbol(state.load_const(state.read_short())->to_string(), state.pop());
                    break;
                case Opcode::PLSTORE:
                    frame->get_locals().set(state.read_short(), state.pop());
                    break;
                case Opcode::PGFSTORE:
                    set_symbol(state.load_const(state.read_byte())->to_string(), state.pop());
                    break;
                case Opcode::PLFSTORE:
                    frame->get_locals().set(state.read_byte(), state.pop());
                    break;
                case Opcode::ALOAD:
                    state.push(frame->get_args().get(state.read_byte()));
                    break;
                case Opcode::ASTORE:
                    frame->get_args().set(state.read_byte(), state.peek());
                    break;
                case Opcode::PASTORE:
                    frame->get_args().set(state.read_byte(), state.pop());
                    break;
                case Opcode::MLOAD: {
                    const auto object = state.pop();
                    const auto name = Sign(state.load_const(state.read_short())->to_string()).get_name();
                    Obj *const member = object->get_member(name);
                    state.push(member);
                    break;
                }
                case Opcode::MSTORE: {
                    const auto object = state.pop();
                    const auto value = state.peek();
                    const auto name = Sign(state.load_const(state.read_short())->to_string()).get_name();
                    object->set_member(name, value);
                    break;
                }
                case Opcode::MFLOAD: {
                    const auto object = state.pop();
                    const auto name = Sign(state.load_const(state.read_byte())->to_string()).get_name();
                    Obj *const member = object->get_member(name);
                    state.push(member);
                    break;
                }
                case Opcode::MFSTORE: {
                    const auto object = state.pop();
                    const auto value = state.peek();
                    const auto name = Sign(state.load_const(state.read_byte())->to_string()).get_name();
                    object->set_member(name, value);
                    break;
                }
                case Opcode::PMSTORE: {
                    const auto object = state.pop();
                    const auto value = state.pop();
                    const auto name = Sign(state.load_const(state.read_short())->to_string()).get_name();
                    object->set_member(name, value);
                    break;
                }
                case Opcode::PMFSTORE: {
                    const auto object = state.pop();
                    const auto value = state.pop();
                    const auto name = Sign(state.load_const(state.read_byte())->to_string()).get_name();
                    object->set_member(name, value);
                    break;
                }
                case Opcode::OBJLOAD: {
                    const auto type = cast<Type>(state.pop());
                    const auto object = halloc_mgr<Obj>(manager, type);
                    state.push(object);
                    break;
                }
                case Opcode::ARRUNPACK: {
                    const auto array = cast<ObjArray>(state.pop());
                    array->for_each([&state](const auto item) { state.push(item); });
                    break;
                }
                case Opcode::ARRPACK: {
                    const uint8_t count = state.read_byte();
                    const auto array = halloc_mgr<ObjArray>(manager, count);
                    frame->sp -= count;
                    for (size_t i = 0; i < count; ++i) {
                        array->set(i, frame->sp[i]);
                    }
                    state.push(array);
                    break;
                }
                case Opcode::ARRBUILD: {
                    const uint8_t count = static_cast<uint8_t>(state.read_short());
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
                    const auto array = cast<ObjArray>(state.pop());
                    const auto index = cast<ObjInt>(state.pop());
                    state.push(array->get(index->value()));
                    break;
                }
                case Opcode::ISTORE: {
                    const auto array = cast<ObjArray>(state.pop());
                    const auto index = cast<ObjInt>(state.pop());
                    const auto value = state.peek();
                    array->set(index->value(), value);
                    break;
                }
                case Opcode::PISTORE: {
                    const auto array = cast<ObjArray>(state.pop());
                    const auto index = cast<ObjInt>(state.pop());
                    const auto value = state.pop();
                    array->set(index->value(), value);
                    break;
                }
                case Opcode::ARRLEN: {
                    const auto array = cast<ObjArray>(state.pop());
                    state.push(halloc_mgr<ObjInt>(manager, array->count()));
                    break;
                }
                case Opcode::INVOKE: {
                    // Get the count
                    const uint8_t count = state.read_byte();
                    // Pop the arguments
                    frame->sp -= count;
                    // Get the method
                    const auto method = cast<ObjMethod>(state.pop());
                    // Call it
                    method->call(null, frame->sp + 1);
                    break;
                }
                case Opcode::VINVOKE: {
                    // Get the param
                    const Sign sign{state.load_const(state.read_short())->to_string()};
                    // Get name of the method
                    const auto name = sign.get_name();
                    // Get the arg count
                    const uint8_t count = static_cast<uint8_t>(sign.get_params().size());

                    // Pop the arguments
                    frame->sp -= count;
                    // Get the object
                    const auto object = state.pop();
                    // Get the method
                    const auto method = cast<ObjMethod>(object->get_member(name));
                    // Call it
                    method->call(object, frame->sp + 1);
                    // Set this
                    break;
                }
                case Opcode::SPINVOKE: {
                    const auto method = cast<ObjMethod>(get_symbol(state.load_const(state.read_short())->to_string()));
                    const uint8_t count = method->get_frame_template().get_args().count();
                    frame->sp -= count;
                    Obj *obj = state.pop();
                    method->call(null, frame->sp + 1);
                    state.get_frame()->get_locals().set(0, obj);
                    break;
                }
                case Opcode::SPFINVOKE: {
                    const auto method = cast<ObjMethod>(get_symbol(state.load_const(state.read_byte())->to_string()));
                    const uint8_t count = method->get_frame_template().get_args().count();
                    frame->sp -= count;
                    Obj *obj = state.pop();
                    method->call(null, frame->sp + 1);
                    state.get_frame()->get_locals().set(0, obj);
                    break;
                }
                case Opcode::LINVOKE: {
                    // Get the method
                    const auto method = cast<ObjMethod>(frame->get_locals().get(state.read_short()));
                    // Get the arg count
                    const uint8_t count = method->get_frame_template().get_args().count();
                    // Pop the arguments
                    frame->sp -= count;
                    // Call it
                    method->call(null, frame->sp);
                    break;
                }
                case Opcode::GINVOKE: {
                    // Get the method
                    const auto method = cast<ObjMethod>(get_symbol(state.load_const(state.read_short())->to_string()));
                    // Get the arg count
                    const uint8_t count = method->get_frame_template().get_args().count();
                    // Pop the arguments
                    frame->sp -= count;
                    // Call it
                    method->call(null, frame->sp);
                    break;
                }
                case Opcode::VFINVOKE: {
                    // Get the param
                    const Sign sign{state.load_const(state.read_byte())->to_string()};
                    // Get name of the method
                    const auto name = sign.get_name();
                    // Get the arg count
                    const uint8_t count = static_cast<uint8_t>(sign.get_params().size());

                    // Pop the arguments
                    frame->sp -= count;
                    // Get the object
                    const auto object = state.pop();
                    // Get the method
                    const auto method = cast<ObjMethod>(object->get_member(name));
                    // Call it
                    method->call(object, frame->sp + 1);
                    // Set this
                    state.get_frame()->get_locals().set(0, object);
                    break;
                }
                case Opcode::LFINVOKE: {
                    // Get the method
                    const auto method = cast<ObjMethod>(frame->get_locals().get(state.read_byte()));
                    // Get the arg count
                    const uint8_t count = method->get_frame_template().get_args().count();
                    // Pop the arguments
                    frame->sp -= count;
                    // Call it
                    method->call(null, frame->sp);
                    break;
                }
                case Opcode::GFINVOKE: {
                    // Get the method
                    const auto method = cast<ObjMethod>(get_symbol(state.load_const(state.read_byte())->to_string()));
                    // Get the arg count
                    const uint8_t count = method->get_frame_template().get_args().count();
                    // Pop the arguments
                    frame->sp -= count;
                    // Call it
                    method->call(null, frame->sp);
                    break;
                }
                case Opcode::AINVOKE: {
                    // Get the method
                    const auto method = cast<ObjMethod>(frame->get_args().get(state.read_byte()));
                    // Get the arg count
                    const uint8_t count = method->get_frame_template().get_args().count();
                    // Pop the arguments
                    frame->sp -= count;
                    // Call it
                    method->call(null, frame->sp);
                    break;
                }
                case Opcode::CALLSUB: {
                    const auto address = halloc_mgr<ObjInt>(manager, frame->ip - &frame->code[0]);
                    state.push(address);
                    const auto offset = state.read_short();
                    state.adjust(offset);
                    break;
                }
                case Opcode::RETSUB: {
                    const auto address = cast<ObjInt>(state.pop());
                    frame->ip = &frame->code[0] + address->value();
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
                    if (obj->truth())
                        state.adjust(offset);
                    break;
                }
                case Opcode::JF: {
                    const auto obj = state.pop();
                    const int16_t offset = static_cast<int16_t>(state.read_short());
                    if (!obj->truth())
                        state.adjust(offset);
                    break;
                }
                case Opcode::JLT: {
                    Obj const *b = state.pop();
                    Obj const &a = *state.pop();
                    const int16_t offset = static_cast<int16_t>(state.read_short());
                    if ((a < b)->truth())
                        state.adjust(offset);
                    break;
                }
                case Opcode::JLE: {
                    Obj const *b = state.pop();
                    Obj const &a = *state.pop();
                    const int16_t offset = static_cast<int16_t>(state.read_short());
                    if ((a <= b)->truth())
                        state.adjust(offset);
                    break;
                }
                case Opcode::JEQ: {
                    Obj const *b = state.pop();
                    Obj const &a = *state.pop();
                    const int16_t offset = static_cast<int16_t>(state.read_short());
                    if ((a == b)->truth())
                        state.adjust(offset);
                    break;
                }
                case Opcode::JNE: {
                    Obj const *b = state.pop();
                    Obj const &a = *state.pop();
                    const int16_t offset = static_cast<int16_t>(state.read_short());
                    if ((a != b)->truth())
                        state.adjust(offset);
                    break;
                }
                case Opcode::JGE: {
                    Obj const *b = state.pop();
                    Obj const &a = *state.pop();
                    const int16_t offset = static_cast<int16_t>(state.read_short());
                    if ((a >= b)->truth())
                        state.adjust(offset);
                    break;
                }
                case Opcode::JGT: {
                    Obj const *b = state.pop();
                    Obj const &a = *state.pop();
                    const int16_t offset = static_cast<int16_t>(state.read_short());
                    if ((a > b)->truth())
                        state.adjust(offset);
                    break;
                }
                case Opcode::NOT:
                    state.push(!*cast<ObjBool>(state.pop()));
                    break;
                case Opcode::INV:
                    state.push(~*cast<ObjInt>(state.pop()));
                    break;
                case Opcode::NEG:
                    state.push(-*cast<ObjInt>(state.pop()));
                    break;
                case Opcode::GETTYPE:
                    state.push(state.pop()->get_type());
                    break;
                case Opcode::SCAST: {
                    const auto type = cast<Type>(state.pop());
                    const auto obj = state.pop();
                    if (check_cast(obj->get_type(), type)) {
                        obj->set_type(type);
                        state.push(obj);
                    } else
                        state.push(halloc_mgr<ObjNull>(manager));
                    break;
                }
                case Opcode::CCAST: {
                    const auto type = cast<Type>(state.pop());
                    const auto obj = state.pop();
                    if (check_cast(obj->get_type(), type)) {
                        obj->set_type(type);
                        state.push(obj);
                    } else
                        runtime_error(std::format("object of type '{}' cannot be cast to object of type '{}'",
                                                  obj->get_type()->get_sign().to_string(), type->get_sign().to_string()));
                    break;
                }
                case Opcode::CONCAT: {
                    const auto b = cast<ObjString>(state.pop());
                    const auto a = cast<ObjString>(state.pop());
                    state.push(halloc_mgr<ObjString>(manager, a->to_string() + b->to_string()));
                    break;
                }
                case Opcode::POW: {
                    const auto b = cast<ObjNumber>(state.pop());
                    const auto a = cast<ObjNumber>(state.pop());
                    state.push(a->power(b));
                    break;
                }
                case Opcode::MUL: {
                    const auto b = cast<ObjNumber>(state.pop());
                    const auto a = cast<ObjNumber>(state.pop());
                    state.push(*a * b);
                    break;
                }
                case Opcode::DIV: {
                    const auto b = cast<ObjNumber>(state.pop());
                    const auto a = cast<ObjNumber>(state.pop());
                    state.push(*a / b);
                    break;
                }
                case Opcode::REM: {
                    const auto b = cast<ObjInt>(state.pop());
                    const auto a = cast<ObjInt>(state.pop());
                    state.push(*a % *b);
                    break;
                }
                case Opcode::ADD: {
                    const auto b = cast<ObjNumber>(state.pop());
                    const auto a = cast<ObjNumber>(state.pop());
                    state.push(*a + b);
                    break;
                }
                case Opcode::SUB: {
                    const auto b = cast<ObjNumber>(state.pop());
                    const auto a = cast<ObjNumber>(state.pop());
                    state.push(*a - b);
                    break;
                }
                case Opcode::SHL: {
                    const auto b = cast<ObjInt>(state.pop());
                    const auto a = cast<ObjInt>(state.pop());
                    state.push(*a << *b);
                    break;
                }
                case Opcode::SHR: {
                    const auto b = cast<ObjInt>(state.pop());
                    const auto a = cast<ObjInt>(state.pop());
                    state.push(*a >> *b);
                    break;
                }
                case Opcode::USHR: {
                    const auto b = cast<ObjInt>(state.pop());
                    const auto a = cast<ObjInt>(state.pop());
                    state.push(a->unsigned_right_shift(*b));
                    break;
                }
                case Opcode::AND: {
                    const auto b = cast<ObjInt>(state.pop());
                    const auto a = cast<ObjInt>(state.pop());
                    state.push(*a & *b);
                    break;
                }
                case Opcode::OR: {
                    const auto b = cast<ObjInt>(state.pop());
                    const auto a = cast<ObjInt>(state.pop());
                    state.push(*a | *b);
                    break;
                }
                case Opcode::XOR: {
                    const auto b = cast<ObjInt>(state.pop());
                    const auto a = cast<ObjInt>(state.pop());
                    state.push(*a ^ *b);
                    break;
                }
                case Opcode::LT: {
                    Obj const *b = state.pop();
                    Obj const &a = *state.pop();
                    state.push(a < b);
                    break;
                }
                case Opcode::LE: {
                    Obj const *b = state.pop();
                    Obj const &a = *state.pop();
                    state.push(a <= b);
                    break;
                }
                case Opcode::EQ: {
                    Obj const *b = state.pop();
                    Obj const &a = *state.pop();
                    state.push(a == b);
                    break;
                }
                case Opcode::NE: {
                    Obj const *b = state.pop();
                    Obj const &a = *state.pop();
                    state.push(a != b);
                    break;
                }
                case Opcode::GE: {
                    Obj const *b = state.pop();
                    Obj const &a = *state.pop();
                    state.push(a >= b);
                    break;
                }
                case Opcode::GT: {
                    Obj const *b = state.pop();
                    Obj const &a = *state.pop();
                    state.push(a > b);
                    break;
                }
                case Opcode::IS: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(halloc_mgr<ObjBool>(manager, a == b));
                    break;
                }
                case Opcode::NIS: {
                    const auto b = state.pop();
                    const auto a = state.pop();
                    state.push(halloc_mgr<ObjBool>(manager, a != b));
                    break;
                }
                case Opcode::ISNULL:
                    state.push(halloc_mgr<ObjBool>(manager, is<ObjNull>(state.pop())));
                    break;
                case Opcode::NISNULL:
                    state.push(halloc_mgr<ObjBool>(manager, !is<ObjNull>(state.pop())));
                    break;
                case Opcode::ENTERMONITOR:
                    state.pop()->enter_monitor();
                    break;
                case Opcode::EXITMONITOR:
                    state.pop()->exit_monitor();
                    break;
                case Opcode::MTPERF: {
                    const auto match = frame->get_matches()[state.read_short()];
                    const uint32_t offset = match.perform(state.pop());
                    state.set_pc(offset);
                    break;
                }
                case Opcode::MTFPERF: {
                    const auto match = frame->get_matches()[state.read_byte()];
                    const uint32_t offset = match.perform(state.pop());
                    state.set_pc(offset);
                    break;
                }
                case Opcode::CLOSURELOAD: {
                    // * Stack layout
                    // Initial  -> [ ... | <method>]
                    // Final    -> [ ... ]
                    //
                    // * Instruction layout
                    // +---------------------------------------------------------------+
                    // | closureload capture_count:u8                                  |
                    // |     capture_dest:u16 capture_type:u8 capture_from:(u8 or u16) |
                    // +---------------------------------------------------------------+
                    // If capture_type is 0x00 -> capture_from is u8
                    // If capture_type is 0x01 -> capture_from is u16
                    //
                    const uint8_t capture_count = state.read_byte();
                    const auto method = cast<ObjMethod>(state.pop()->copy());
                    VariableTable &locals = method->get_frame_template().get_locals();
                    for (uint8_t i = 0; i < capture_count; i++) {
                        const uint16_t local_idx = state.read_short();
                        ObjCapture *capture;
                        switch (state.read_byte()) {
                        case 0x00:
                            capture = frame->get_args().ramp_up(state.read_byte());
                            break;
                        case 0x01:
                            capture = frame->get_locals().ramp_up(state.read_short());
                            break;
                        default:
                            throw Unreachable();
                        }
                        locals.set(i, capture);
                    }
                    state.push(method);
                    break;
                }
                case Opcode::THROW: {
                    const auto value = state.pop();
                    throw ThrowSignal(value);
                }
                case Opcode::RET: {
                    const auto currentFrame = state.get_frame();
                    // Pop the return value
                    const auto val = state.pop();
                    // Pop the current frame
                    state.pop_frame();
                    // Return if encountered end of execution
                    if (top_frame == currentFrame) {
                        return val;
                    }
                    // Push the return value
                    state.get_frame()->push(val);
                    break;
                }
                case Opcode::VRET: {
                    const auto currentFrame = state.get_frame();
                    // Pop the current frame
                    state.pop_frame();
                    // Return if encountered end of execution
                    if (top_frame == currentFrame) {
                        return halloc_mgr<ObjNull>(manager);
                    }
                    break;
                }
                case Opcode::PRINTLN:
                    write(state.pop()->to_string() + "\n");
                    break;
                case Opcode::I2F:
                    state.push(halloc_mgr<ObjFloat>(manager, static_cast<double>(cast<ObjInt>(state.pop())->value())));
                    break;
                case Opcode::F2I:
                    state.push(halloc_mgr<ObjInt>(manager, static_cast<int64_t>(cast<ObjFloat>(state.pop())->value())));
                    break;
                case Opcode::I2B:
                    state.push(halloc_mgr<ObjBool>(manager, cast<ObjInt>(state.pop())->value() != 0));
                    break;
                case Opcode::B2I:
                    state.push(halloc_mgr<ObjInt>(manager, cast<ObjBool>(state.pop())->truth() ? 1 : 0));
                    break;
                case Opcode::O2B:
                    state.push(halloc_mgr<ObjBool>(manager, state.pop()->truth()));
                    break;
                case Opcode::O2S:
                    state.push(halloc_mgr<ObjString>(manager, state.pop()->to_string()));
                    break;
                }
            } catch (const ThrowSignal &signal) {
                const auto value = signal.get_value();
                while (state.get_call_stack_size() > 0) {
                    frame = state.get_frame();
                    const auto info = frame->get_exceptions().get_target(state.get_pc(), value->get_type());
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
        return halloc_mgr<ObjNull>(manager);
    }
}    // namespace spade
