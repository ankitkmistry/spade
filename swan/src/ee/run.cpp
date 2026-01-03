#include "vm.hpp"
#include "memory/memory.hpp"
#include <iostream>
#include <sputils.hpp>

namespace spade
{
    Obj *SpadeVM::run(Thread *thread) {
        const auto state = thread->get_vm();
        const auto top_frame = get_frame();
        while (thread->is_running()) {
            const auto opcode = static_cast<Opcode>(read_byte());
            auto frame = get_frame();
            if (debugger)
                debugger->update(this);

            try {
                switch (opcode) {
                case Opcode::NOP:
                    // Do nothing
                    break;
                case Opcode::CONST:
                    push(load_const(read_byte()));
                    break;
                case Opcode::CONST_NULL:
                    push(halloc_mgr<ObjNull>(manager));
                    break;
                case Opcode::CONST_TRUE:
                    push(halloc_mgr<ObjBool>(manager, true));
                    break;
                case Opcode::CONST_FALSE:
                    push(halloc_mgr<ObjBool>(manager, false));
                    break;
                case Opcode::CONSTL:
                    push(load_const(read_short()));
                    break;
                case Opcode::POP:
                    pop();
                    break;
                case Opcode::NPOP:
                    frame->sp -= read_byte();
                    break;
                case Opcode::DUP:
                    push(peek());
                    break;
                case Opcode::NDUP: {
                    const uint8_t count = read_byte();
                    for (uint8_t i = 0; i < count; ++i) {
                        frame->sp[i] = frame->sp[-1];
                    }
                    frame->sp += count;
                    break;
                }
                case Opcode::GLOAD:
                    push(get_symbol(load_const(read_short())->to_string()));
                    break;
                case Opcode::GSTORE:
                    set_symbol(load_const(read_short())->to_string(), peek());
                    break;
                case Opcode::LLOAD:
                    push(frame->get_locals().get(read_short()));
                    break;
                case Opcode::LSTORE:
                    frame->get_locals().set(read_short(), peek());
                    break;
                case Opcode::SPLOAD: {
                    const auto obj = pop();
                    const auto sign = load_const(read_short())->to_string();
                    const auto method = cast<ObjMethod>(get_symbol(sign)->copy());
                    method->get_frame_template().get_locals().ramp_up(0);
                    method->get_frame_template().get_locals().set(0, obj);
                    push(method);
                    break;
                }
                case Opcode::GFLOAD:
                    push(get_symbol(load_const(read_byte())->to_string()));
                    break;
                case Opcode::GFSTORE:
                    set_symbol(load_const(read_byte())->to_string(), peek());
                    break;
                case Opcode::LFLOAD:
                    push(frame->get_locals().get(read_byte()));
                    break;
                case Opcode::LFSTORE:
                    frame->get_locals().set(read_byte(), peek());
                    break;
                case Opcode::SPFLOAD: {
                    const auto obj = pop();
                    const auto sign = load_const(read_byte())->to_string();
                    const auto method = cast<ObjMethod>(get_symbol(sign)->copy());
                    method->get_frame_template().get_locals().ramp_up(0);
                    method->get_frame_template().get_locals().set(0, obj);
                    push(method);
                    break;
                }
                case Opcode::PGSTORE:
                    set_symbol(load_const(read_short())->to_string(), pop());
                    break;
                case Opcode::PLSTORE:
                    frame->get_locals().set(read_short(), pop());
                    break;
                case Opcode::PGFSTORE:
                    set_symbol(load_const(read_byte())->to_string(), pop());
                    break;
                case Opcode::PLFSTORE:
                    frame->get_locals().set(read_byte(), pop());
                    break;
                case Opcode::ALOAD:
                    push(frame->get_args().get(read_byte()));
                    break;
                case Opcode::ASTORE:
                    frame->get_args().set(read_byte(), peek());
                    break;
                case Opcode::PASTORE:
                    frame->get_args().set(read_byte(), pop());
                    break;
                case Opcode::TLOAD:
                    // TODO: implement this
                    // push(frame->get_method()->get_type_param(load_const(read_short())->to_string()));
                    break;
                case Opcode::TFLOAD:
                    // TODO: implement this
                    // push(frame->get_method()->get_type_param(load_const(read_byte())->to_string()));
                    break;
                case Opcode::TSTORE:
                    // TODO: implement this
                    // frame->get_method()
                    //         ->get_type_param(load_const(read_short())->to_string())
                    //         ->set_placeholder(cast<Type>(peek()));
                    break;
                case Opcode::TFSTORE:
                    // TODO: implement this
                    // frame->get_method()
                    //         ->get_type_param(load_const(read_byte())->to_string())
                    //         ->set_placeholder(cast<Type>(peek()));
                    break;
                case Opcode::PTSTORE:
                    // TODO: implement this
                    // frame->get_method()
                    //         ->get_type_param(load_const(read_short())->to_string())
                    //         ->set_placeholder(cast<Type>(pop()));
                    break;
                case Opcode::PTFSTORE:
                    // TODO: implement this
                    // frame->get_method()
                    //         ->get_type_param(load_const(read_byte())->to_string())
                    //         ->set_placeholder(cast<Type>(pop()));
                    break;
                case Opcode::MLOAD: {
                    const auto object = pop();
                    const auto name = Sign(load_const(read_short())->to_string()).get_name();
                    Obj *const member = object->get_member(name);
                    push(member);
                    break;
                }
                case Opcode::MSTORE: {
                    const auto object = pop();
                    const auto value = peek();
                    const auto name = Sign(load_const(read_short())->to_string()).get_name();
                    object->set_member(name, value);
                    break;
                }
                case Opcode::MFLOAD: {
                    const auto object = pop();
                    const auto name = Sign(load_const(read_byte())->to_string()).get_name();
                    Obj *const member = object->get_member(name);
                    push(member);
                    break;
                }
                case Opcode::MFSTORE: {
                    const auto object = pop();
                    const auto value = peek();
                    const auto name = Sign(load_const(read_byte())->to_string()).get_name();
                    object->set_member(name, value);
                    break;
                }
                case Opcode::PMSTORE: {
                    const auto object = pop();
                    const auto value = pop();
                    const auto name = Sign(load_const(read_short())->to_string()).get_name();
                    object->set_member(name, value);
                    break;
                }
                case Opcode::PMFSTORE: {
                    const auto object = pop();
                    const auto value = pop();
                    const auto name = Sign(load_const(read_byte())->to_string()).get_name();
                    object->set_member(name, value);
                    break;
                }
                case Opcode::OBJLOAD: {
                    const auto type = cast<Type>(pop());
                    const auto object = halloc_mgr<Obj>(manager, type);
                    push(object);
                    break;
                }
                case Opcode::ARRUNPACK: {
                    const auto array = cast<ObjArray>(pop());
                    array->for_each([this](const auto item) { push(item); });
                    break;
                }
                case Opcode::ARRPACK: {
                    const uint8_t count = read_byte();
                    const auto array = halloc_mgr<ObjArray>(manager, count);
                    frame->sp -= count;
                    for (size_t i = 0; i < count; ++i) {
                        array->set(i, frame->sp[i]);
                    }
                    push(array);
                    break;
                }
                case Opcode::ARRBUILD: {
                    const uint8_t count = static_cast<uint8_t>(read_short());
                    const auto array = halloc_mgr<ObjArray>(manager, count);
                    push(array);
                    break;
                }
                case Opcode::ARRFBUILD: {
                    const uint8_t count = read_byte();
                    const auto array = halloc_mgr<ObjArray>(manager, count);
                    push(array);
                    break;
                }
                case Opcode::ILOAD: {
                    const auto array = cast<ObjArray>(pop());
                    const auto index = cast<ObjInt>(pop());
                    push(array->get(index->value()));
                    break;
                }
                case Opcode::ISTORE: {
                    const auto array = cast<ObjArray>(pop());
                    const auto index = cast<ObjInt>(pop());
                    const auto value = peek();
                    array->set(index->value(), value);
                    break;
                }
                case Opcode::PISTORE: {
                    const auto array = cast<ObjArray>(pop());
                    const auto index = cast<ObjInt>(pop());
                    const auto value = pop();
                    array->set(index->value(), value);
                    break;
                }
                case Opcode::ARRLEN: {
                    const auto array = cast<ObjArray>(pop());
                    push(halloc_mgr<ObjInt>(manager, array->count()));
                    break;
                }
                case Opcode::INVOKE: {
                    // Get the count
                    const uint8_t count = read_byte();
                    // Pop the arguments
                    frame->sp -= count;
                    // Get the method
                    const auto method = cast<ObjMethod>(pop());
                    // Call it
                    method->call(frame->sp + 1);
                    break;
                }
                case Opcode::VINVOKE: {
                    // Get the param
                    const Sign sign{load_const(read_short())->to_string()};
                    // Get name of the method
                    const auto name = sign.get_name();
                    // Get the arg count
                    const uint8_t count = static_cast<uint8_t>(sign.get_params().size());

                    // Pop the arguments
                    frame->sp -= count;
                    // Get the object
                    const auto object = pop();
                    // Get the method
                    const auto method = cast<ObjMethod>(object->get_member(name));
                    // Call it
                    method->call(frame->sp + 1);
                    // Set this
                    get_frame()->get_locals().set(0, object);
                    break;
                }
                case Opcode::SPINVOKE: {
                    const auto method = cast<ObjMethod>(get_symbol(load_const(read_short())->to_string()));
                    const uint8_t count = method->get_frame_template().get_args().count();
                    frame->sp -= count;
                    Obj *obj = pop();
                    method->call(frame->sp + 1);
                    get_frame()->get_locals().set(0, obj);
                    break;
                }
                case Opcode::SPFINVOKE: {
                    const auto method = cast<ObjMethod>(get_symbol(load_const(read_byte())->to_string()));
                    const uint8_t count = method->get_frame_template().get_args().count();
                    frame->sp -= count;
                    Obj *obj = pop();
                    method->call(frame->sp + 1);
                    get_frame()->get_locals().set(0, obj);
                    break;
                }
                case Opcode::LINVOKE: {
                    // Get the method
                    const auto method = cast<ObjMethod>(frame->get_locals().get(read_short()));
                    // Get the arg count
                    const uint8_t count = method->get_frame_template().get_args().count();
                    // Pop the arguments
                    frame->sp -= count;
                    // Call it
                    method->call(frame->sp);
                    break;
                }
                case Opcode::GINVOKE: {
                    // Get the method
                    const auto method = cast<ObjMethod>(get_symbol(load_const(read_short())->to_string()));
                    // Get the arg count
                    const uint8_t count = method->get_frame_template().get_args().count();
                    // Pop the arguments
                    frame->sp -= count;
                    // Call it
                    method->call(frame->sp);
                    break;
                }
                case Opcode::VFINVOKE: {
                    // Get the param
                    const Sign sign{load_const(read_byte())->to_string()};
                    // Get name of the method
                    const auto name = sign.get_name();
                    // Get the arg count
                    const uint8_t count = static_cast<uint8_t>(sign.get_params().size());

                    // Pop the arguments
                    frame->sp -= count;
                    // Get the object
                    const auto object = pop();
                    // Get the method
                    const auto method = cast<ObjMethod>(object->get_member(name));
                    // Call it
                    method->call(frame->sp + 1);
                    // Set this
                    get_frame()->get_locals().set(0, object);
                    break;
                }
                case Opcode::LFINVOKE: {
                    // Get the method
                    const auto method = cast<ObjMethod>(frame->get_locals().get(read_byte()));
                    // Get the arg count
                    const uint8_t count = method->get_frame_template().get_args().count();
                    // Pop the arguments
                    frame->sp -= count;
                    // Call it
                    method->call(frame->sp);
                    break;
                }
                case Opcode::GFINVOKE: {
                    // Get the method
                    const auto method = cast<ObjMethod>(get_symbol(load_const(read_byte())->to_string()));
                    // Get the arg count
                    const uint8_t count = method->get_frame_template().get_args().count();
                    // Pop the arguments
                    frame->sp -= count;
                    // Call it
                    method->call(frame->sp);
                    break;
                }
                case Opcode::AINVOKE: {
                    // Get the method
                    const auto method = cast<ObjMethod>(frame->get_args().get(read_byte()));
                    // Get the arg count
                    const uint8_t count = method->get_frame_template().get_args().count();
                    // Pop the arguments
                    frame->sp -= count;
                    // Call it
                    method->call(frame->sp);
                    break;
                }
                case Opcode::CALLSUB: {
                    const auto address = halloc_mgr<ObjInt>(manager, frame->ip - &frame->code[0]);
                    push(address);
                    const auto offset = read_short();
                    adjust(offset);
                    break;
                }
                case Opcode::RETSUB: {
                    const auto address = cast<ObjInt>(pop());
                    frame->ip = &frame->code[0] + address->value();
                    break;
                }
                case Opcode::JMP: {
                    const int16_t offset = static_cast<int16_t>(read_short());
                    adjust(offset);
                    break;
                }
                case Opcode::JT: {
                    const auto obj = pop();
                    const int16_t offset = static_cast<int16_t>(read_short());
                    if (obj->truth())
                        adjust(offset);
                    break;
                }
                case Opcode::JF: {
                    const auto obj = pop();
                    const int16_t offset = static_cast<int16_t>(read_short());
                    if (!obj->truth())
                        adjust(offset);
                    break;
                }
                case Opcode::JLT: {
                    Obj const *b = pop();
                    Obj const &a = *pop();
                    const int16_t offset = static_cast<int16_t>(read_short());
                    if ((a < b)->truth())
                        adjust(offset);
                    break;
                }
                case Opcode::JLE: {
                    Obj const *b = pop();
                    Obj const &a = *pop();
                    const int16_t offset = static_cast<int16_t>(read_short());
                    if ((a <= b)->truth())
                        adjust(offset);
                    break;
                }
                case Opcode::JEQ: {
                    Obj const *b = pop();
                    Obj const &a = *pop();
                    const int16_t offset = static_cast<int16_t>(read_short());
                    if ((a == b)->truth())
                        adjust(offset);
                    break;
                }
                case Opcode::JNE: {
                    Obj const *b = pop();
                    Obj const &a = *pop();
                    const int16_t offset = static_cast<int16_t>(read_short());
                    if ((a != b)->truth())
                        adjust(offset);
                    break;
                }
                case Opcode::JGE: {
                    Obj const *b = pop();
                    Obj const &a = *pop();
                    const int16_t offset = static_cast<int16_t>(read_short());
                    if ((a >= b)->truth())
                        adjust(offset);
                    break;
                }
                case Opcode::JGT: {
                    Obj const *b = pop();
                    Obj const &a = *pop();
                    const int16_t offset = static_cast<int16_t>(read_short());
                    if ((a > b)->truth())
                        adjust(offset);
                    break;
                }
                case Opcode::NOT:
                    push(!*cast<ObjBool>(pop()));
                    break;
                case Opcode::INV:
                    push(~*cast<ObjInt>(pop()));
                    break;
                case Opcode::NEG:
                    push(-*cast<ObjInt>(pop()));
                    break;
                case Opcode::GETTYPE:
                    push(pop()->get_type());
                    break;
                case Opcode::SCAST: {
                    const auto type = cast<Type>(pop());
                    const auto obj = pop();
                    if (check_cast(obj->get_type(), type)) {
                        obj->set_type(type);
                        push(obj);
                    } else
                        push(halloc_mgr<ObjNull>(manager));
                    break;
                }
                case Opcode::CCAST: {
                    const auto type = cast<Type>(pop());
                    const auto obj = pop();
                    if (check_cast(obj->get_type(), type)) {
                        obj->set_type(type);
                        push(obj);
                    } else
                        runtime_error(std::format("object of type '{}' cannot be cast to object of type '{}'",
                                                  obj->get_type()->get_sign().to_string(), type->get_sign().to_string()));
                    break;
                }
                case Opcode::CONCAT: {
                    const auto b = cast<ObjString>(pop());
                    const auto a = cast<ObjString>(pop());
                    push(halloc_mgr<ObjString>(manager, a->to_string() + b->to_string()));
                    break;
                }
                case Opcode::POW: {
                    const auto b = cast<ObjNumber>(pop());
                    const auto a = cast<ObjNumber>(pop());
                    push(a->power(b));
                    break;
                }
                case Opcode::MUL: {
                    const auto b = cast<ObjNumber>(pop());
                    const auto a = cast<ObjNumber>(pop());
                    push(*a * b);
                    break;
                }
                case Opcode::DIV: {
                    const auto b = cast<ObjNumber>(pop());
                    const auto a = cast<ObjNumber>(pop());
                    push(*a / b);
                    break;
                }
                case Opcode::REM: {
                    const auto b = cast<ObjInt>(pop());
                    const auto a = cast<ObjInt>(pop());
                    push(*a % *b);
                    break;
                }
                case Opcode::ADD: {
                    const auto b = cast<ObjNumber>(pop());
                    const auto a = cast<ObjNumber>(pop());
                    push(*a + b);
                    break;
                }
                case Opcode::SUB: {
                    const auto b = cast<ObjNumber>(pop());
                    const auto a = cast<ObjNumber>(pop());
                    push(*a - b);
                    break;
                }
                case Opcode::SHL: {
                    const auto b = cast<ObjInt>(pop());
                    const auto a = cast<ObjInt>(pop());
                    push(*a << *b);
                    break;
                }
                case Opcode::SHR: {
                    const auto b = cast<ObjInt>(pop());
                    const auto a = cast<ObjInt>(pop());
                    push(*a >> *b);
                    break;
                }
                case Opcode::USHR: {
                    const auto b = cast<ObjInt>(pop());
                    const auto a = cast<ObjInt>(pop());
                    push(a->unsigned_right_shift(*b));
                    break;
                }
                case Opcode::AND: {
                    const auto b = cast<ObjInt>(pop());
                    const auto a = cast<ObjInt>(pop());
                    push(*a & *b);
                    break;
                }
                case Opcode::OR: {
                    const auto b = cast<ObjInt>(pop());
                    const auto a = cast<ObjInt>(pop());
                    push(*a | *b);
                    break;
                }
                case Opcode::XOR: {
                    const auto b = cast<ObjInt>(pop());
                    const auto a = cast<ObjInt>(pop());
                    push(*a ^ *b);
                    break;
                }
                case Opcode::LT: {
                    Obj const *b = pop();
                    Obj const &a = *pop();
                    push(a < b);
                    break;
                }
                case Opcode::LE: {
                    Obj const *b = pop();
                    Obj const &a = *pop();
                    push(a <= b);
                    break;
                }
                case Opcode::EQ: {
                    Obj const *b = pop();
                    Obj const &a = *pop();
                    push(a == b);
                    break;
                }
                case Opcode::NE: {
                    Obj const *b = pop();
                    Obj const &a = *pop();
                    push(a != b);
                    break;
                }
                case Opcode::GE: {
                    Obj const *b = pop();
                    Obj const &a = *pop();
                    push(a >= b);
                    break;
                }
                case Opcode::GT: {
                    Obj const *b = pop();
                    Obj const &a = *pop();
                    push(a > b);
                    break;
                }
                case Opcode::IS: {
                    const auto b = pop();
                    const auto a = pop();
                    push(halloc_mgr<ObjBool>(manager, a == b));
                    break;
                }
                case Opcode::NIS: {
                    const auto b = pop();
                    const auto a = pop();
                    push(halloc_mgr<ObjBool>(manager, a != b));
                    break;
                }
                case Opcode::ISNULL:
                    push(halloc_mgr<ObjBool>(manager, is<ObjNull>(pop())));
                    break;
                case Opcode::NISNULL:
                    push(halloc_mgr<ObjBool>(manager, !is<ObjNull>(pop())));
                    break;
                case Opcode::ENTERMONITOR:
                    pop()->enter_monitor();
                    break;
                case Opcode::EXITMONITOR:
                    pop()->exit_monitor();
                    break;
                case Opcode::MTPERF: {
                    const auto match = frame->get_matches()[read_short()];
                    const uint32_t offset = match.perform(pop());
                    set_pc(offset);
                    break;
                }
                case Opcode::MTFPERF: {
                    const auto match = frame->get_matches()[read_byte()];
                    const uint32_t offset = match.perform(pop());
                    set_pc(offset);
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
                    const uint8_t capture_count = read_byte();
                    const auto method = cast<ObjMethod>(pop()->copy());
                    VariableTable &locals = method->get_frame_template().get_locals();
                    for (uint8_t i = 0; i < capture_count; i++) {
                        const uint16_t local_idx = read_short();
                        ObjCapture *capture;
                        switch (read_byte()) {
                        case 0x00:
                            capture = frame->get_args().ramp_up(read_byte());
                            break;
                        case 0x01:
                            capture = frame->get_locals().ramp_up(read_short());
                            break;
                        default:
                            throw Unreachable();
                        }
                        locals.set(i, capture);
                    }
                    push(method);
                    break;
                }
                case Opcode::REIFIEDLOAD: {
                    // TODO: implement this
                    // const uint8_t count = read_byte();
                    // // Pop the arguments
                    // frame->sp -= count;
                    // const auto args = frame->sp;
                    // const auto obj = pop();
                    // if (is<ObjMethod>(obj))
                    //     push(cast<ObjMethod>(obj)->get_reified(args, count));
                    // else if (is<Type>(obj))
                    //     push(cast<Type>(obj)->get_reified(args, count));
                    // else
                    //     throw runtime_error(std::format("cannot set_placeholder value of {}", obj->get_type()->to_string()));
                    // break;
                }
                case Opcode::THROW: {
                    const auto value = pop();
                    throw ThrowSignal(value);
                }
                case Opcode::RET: {
                    const auto currentFrame = get_frame();
                    // Pop the return value
                    const auto val = pop();
                    // Pop the current frame
                    pop_frame();
                    // Return if encountered end of execution
                    if (top_frame == currentFrame) {
                        return val;
                    }
                    // Push the return value
                    get_frame()->push(val);
                    break;
                }
                case Opcode::VRET: {
                    const auto currentFrame = get_frame();
                    // Pop the current frame
                    pop_frame();
                    // Return if encountered end of execution
                    if (top_frame == currentFrame) {
                        return halloc_mgr<ObjNull>(manager);
                    }
                    break;
                }
                case Opcode::PRINTLN:
                    write(pop()->to_string() + "\n");
                    break;
                case Opcode::I2F:
                    push(halloc_mgr<ObjFloat>(manager, static_cast<double>(cast<ObjInt>(pop())->value())));
                    break;
                case Opcode::F2I:
                    push(halloc_mgr<ObjInt>(manager, static_cast<int64_t>(cast<ObjFloat>(pop())->value())));
                    break;
                case Opcode::I2B:
                    push(halloc_mgr<ObjBool>(manager, cast<ObjInt>(pop())->value() != 0));
                    break;
                case Opcode::B2I:
                    push(halloc_mgr<ObjInt>(manager, cast<ObjBool>(pop())->truth() ? 1 : 0));
                    break;
                case Opcode::O2B:
                    push(halloc_mgr<ObjBool>(manager, pop()->truth()));
                    break;
                case Opcode::O2S:
                    push(halloc_mgr<ObjString>(manager, pop()->to_string()));
                    break;
                }
            } catch (const ThrowSignal &signal) {
                const auto value = signal.get_value();
                while (get_call_stack_size() > 0) {
                    frame = get_frame();
                    const auto info = frame->get_exceptions().get_target(get_pc(), value->get_type());
                    if (Exception::IS_NO_EXCEPTION(info))
                        pop_frame();
                    else {
                        set_pc(info.get_target());
                        push(value);
                        break;
                    }
                }
                if (get_call_stack_size() == 0) {
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
