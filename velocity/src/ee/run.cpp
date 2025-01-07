#include "vm.hpp"

#include "../debug/debug.hpp"
#include "../objects/float.hpp"
#include "../objects/int.hpp"

namespace spade
{
    Obj *SpadeVM::run(Thread *thread) {
        auto state = thread->getState();
        auto topFrame = state->getFrame();
        while (thread->isRunning()) {
            auto opcode = static_cast<Opcode>(state->readByte());
            auto frame = state->getFrame();
            DebugOp::printVMState(state);
            try {
                switch (opcode) {
                    case Opcode::NOP:
                        // Do nothing
                        break;
                    case Opcode::CONST:
                        state->push(state->loadConst(state->readByte()));
                        break;
                    case Opcode::CONSTL:
                        state->push(state->loadConst(state->readShort()));
                        break;
                    case Opcode::POP:
                        state->pop();
                        break;
                    case Opcode::NPOP: {
                        uint8 count = (uint8) state->readByte();
                        frame->sp -= count;
                        break;
                    }
                    case Opcode::DUP:
                        state->push(state->peek());
                        break;
                    case Opcode::NDUP: {
                        uint8 count = (uint8) state->readByte();
                        for (int i = 0; i < count; ++i) { frame->sp[i] = frame->sp[-1]; }
                        frame->sp += count;
                        break;
                    }
                    case Opcode::GLOAD:
                        state->push(getSymbol(state->loadConst(state->readShort())->toString()));
                        break;
                    case Opcode::GSTORE:
                        setSymbol(state->loadConst(state->readShort())->toString(), state->peek());
                        break;
                    case Opcode::LLOAD:
                        state->push(frame->getLocals().get(state->readShort()));
                        break;
                    case Opcode::LSTORE:
                        frame->getLocals().set(state->readShort(), state->peek());
                        break;
                    case Opcode::BLOAD:
                        state->push(frame->getLambdas()[state->readShort()]);
                        break;
                    case Opcode::SPLOAD: {
                        auto obj = state->pop();
                        auto sign = state->loadConst(state->readShort())->toString();
                        state->push(obj->getSuperClassMethod(sign));
                        break;
                    }
                    case Opcode::GFLOAD:
                        state->push(getSymbol(state->loadConst(state->readByte())->toString()));
                        break;
                    case Opcode::GFSTORE:
                        setSymbol(state->loadConst(state->readByte())->toString(), state->peek());
                        break;
                    case Opcode::LFLOAD:
                        state->push(frame->getLocals().get(state->readByte()));
                        break;
                    case Opcode::LFSTORE:
                        frame->getLocals().set(state->readByte(), state->peek());
                        break;
                    case Opcode::BFLOAD:
                        state->push(frame->getLambdas()[state->readByte()]);
                        break;
                    case Opcode::SPFLOAD: {
                        auto obj = state->pop();
                        auto sign = state->loadConst(state->readByte())->toString();
                        state->push(obj->getSuperClassMethod(sign));
                        break;
                    }
                    case Opcode::PGSTORE:
                        setSymbol(state->loadConst(state->readShort())->toString(), state->pop());
                        break;
                    case Opcode::PLSTORE:
                        frame->getLocals().set(state->readShort(), state->pop());
                        break;
                    case Opcode::PGFSTORE:
                        setSymbol(state->loadConst(state->readByte())->toString(), state->pop());
                        break;
                    case Opcode::PLFSTORE:
                        frame->getLocals().set(state->readByte(), state->pop());
                        break;
                    case Opcode::ALOAD:
                        state->push(frame->getArgs().get(state->readByte()));
                        break;
                    case Opcode::ASTORE:
                        frame->getArgs().set(state->readByte(), state->peek());
                        break;
                    case Opcode::PASTORE:
                        frame->getArgs().set(state->readByte(), state->pop());
                        break;
                    case Opcode::TLOAD:
                        state->push(frame->getMethod()->getTypeParam(state->loadConst(state->readShort())->toString()));
                        break;
                    case Opcode::TFLOAD:
                        state->push(frame->getMethod()->getTypeParam(state->loadConst(state->readByte())->toString()));
                        break;
                    case Opcode::TSTORE:
                        frame->getMethod()
                                ->getTypeParam(state->loadConst(state->readShort())->toString())
                                ->setPlaceholder(cast<Type *>(state->peek()));
                        break;
                    case Opcode::TFSTORE:
                        frame->getMethod()
                                ->getTypeParam(state->loadConst(state->readByte())->toString())
                                ->setPlaceholder(cast<Type *>(state->peek()));
                        break;
                    case Opcode::PTSTORE:
                        frame->getMethod()
                                ->getTypeParam(state->loadConst(state->readShort())->toString())
                                ->setPlaceholder(cast<Type *>(state->pop()));
                        break;
                    case Opcode::PTFSTORE:
                        frame->getMethod()
                                ->getTypeParam(state->loadConst(state->readByte())->toString())
                                ->setPlaceholder(cast<Type *>(state->pop()));
                        break;
                    case Opcode::MLOAD: {
                        auto object = state->pop();
                        auto name = Sign(state->loadConst(state->readShort())->toString()).getName();
                        Obj *member = object->getMember(name);
                        state->push(member);
                        break;
                    }
                    case Opcode::MSTORE: {
                        auto object = state->pop();
                        auto value = state->peek();
                        auto name = Sign(state->loadConst(state->readShort())->toString()).getName();
                        object->setMember(name, value);
                        break;
                    }
                    case Opcode::SLOAD: {
                        auto type = cast<Type *>(state->pop());
                        auto name = Sign(state->loadConst(state->readShort())->toString()).getName();
                        state->push(type->getMember(name));
                        break;
                    }
                    case Opcode::SSTORE: {
                        auto type = cast<Type *>(state->pop());
                        auto value = state->peek();
                        auto name = Sign(state->loadConst(state->readShort())->toString()).getName();
                        type->setMember(name, value);
                        break;
                    }
                    case Opcode::MFLOAD: {
                        auto object = state->pop();
                        auto name = Sign(state->loadConst(state->readByte())->toString()).getName();
                        Obj *member = object->getMember(name);
                        state->push(member);
                        break;
                    }
                    case Opcode::MFSTORE: {
                        auto object = state->pop();
                        auto value = state->peek();
                        auto name = Sign(state->loadConst(state->readByte())->toString()).getName();
                        object->setMember(name, value);
                        break;
                    }
                    case Opcode::SFLOAD: {
                        auto type = cast<Type *>(state->pop());
                        auto name = Sign(state->loadConst(state->readByte())->toString()).getName();
                        state->push(type->getMember(name));
                        break;
                    }
                    case Opcode::SFSTORE: {
                        auto type = cast<Type *>(state->pop());
                        auto value = state->peek();
                        auto name = Sign(state->loadConst(state->readByte())->toString()).getName();
                        type->setMember(name, value);
                        break;
                    }
                    case Opcode::PMSTORE: {
                        auto object = state->pop();
                        auto value = state->pop();
                        auto name = Sign(state->loadConst(state->readShort())->toString()).getName();
                        object->setMember(name, value);
                        break;
                    }
                    case Opcode::PSSTORE: {
                        auto type = cast<Type *>(state->pop());
                        auto value = state->pop();
                        auto name = Sign(state->loadConst(state->readShort())->toString()).getName();
                        type->setMember(name, value);
                        break;
                    }
                    case Opcode::PMFSTORE: {
                        auto object = state->pop();
                        auto value = state->pop();
                        auto name = Sign(state->loadConst(state->readByte())->toString()).getName();
                        object->setMember(name, value);
                        break;
                    }
                    case Opcode::PSFSTORE: {
                        auto type = cast<Type *>(state->pop());
                        auto value = state->pop();
                        auto name = Sign(state->loadConst(state->readByte())->toString()).getName();
                        type->setMember(name, value);
                        break;
                    }
                    case Opcode::OBJLOAD: {
                        auto type = cast<Type *>(state->pop());
                        auto object = halloc<Obj>(manager, Sign(""), type, frame->getMethod()->getModule());
                        state->push(object);
                        break;
                    }
                    case Opcode::ARRUNPACK: {
                        auto array = cast<ObjArray *>(state->pop());
                        array->foreach ([&state](auto item) { state->push(item); });
                        break;
                    }
                    case Opcode::ARRPACK: {
                        uint8 count = state->readByte();
                        auto array = halloc<ObjArray>(manager, count);
                        frame->sp -= count;
                        for (int i = 0; i < count; ++i) { array->set(i, frame->sp[i]); }
                        state->push(array);
                        break;
                    }
                    case Opcode::ARRBUILD: {
                        uint8 count = static_cast<uint8>(state->readShort());
                        auto array = halloc<ObjArray>(manager, count);
                        state->push(array);
                        break;
                    }
                    case Opcode::ARRFBUILD: {
                        uint8 count = (uint8) state->readByte();
                        auto array = halloc<ObjArray>(manager, count);
                        state->push(array);
                        break;
                    }
                    case Opcode::ILOAD: {
                        auto array = cast<ObjArray *>(state->pop());
                        auto index = cast<ObjInt *>(state->pop());
                        state->push(array->get(index->value()));
                        break;
                    }
                    case Opcode::ISTORE: {
                        auto array = cast<ObjArray *>(state->pop());
                        auto index = cast<ObjInt *>(state->pop());
                        auto value = state->peek();
                        array->set(index->value(), value);
                        break;
                    }
                    case Opcode::PISTORE: {
                        auto array = cast<ObjArray *>(state->pop());
                        auto index = cast<ObjInt *>(state->pop());
                        auto value = state->pop();
                        array->set(index->value(), value);
                        break;
                    }
                    case Opcode::ARRLEN: {
                        auto array = cast<ObjArray *>(state->pop());
                        state->push(halloc<ObjInt>(manager, array->count()));
                        break;
                    }
                    case Opcode::INVOKE: {
                        // Get the count
                        uint8 count = state->readByte();
                        // Pop the arguments
                        frame->sp -= count;
                        // Get the method
                        auto method = cast<ObjMethod *>(state->pop());
                        // Call it
                        method->call(frame->sp + 1);
                        break;
                    }
                    case Opcode::VINVOKE: {
                        // Get the param
                        Sign sign{state->loadConst(state->readShort())->toString()};
                        // Get name of the method
                        auto name = sign.getName();
                        // Get the arg count
                        uint8 count = static_cast<uint8>(sign.getParams().size());

                        // Pop the arguments
                        frame->sp -= count;
                        // Get the object
                        auto object = state->pop();
                        // Get the method
                        auto method = cast<ObjMethod *>(object->getMember(name));
                        // Call it
                        method->call(frame->sp + 1);
                        break;
                    }
                    case Opcode::SINVOKE: {
                        // Get the param
                        Sign sign{state->loadConst(state->readShort())->toString()};
                        // Get name of the method
                        auto name = sign.getName();
                        // Get the arg count
                        uint8 count = static_cast<uint8>(sign.getParams().size());

                        // Pop the arguments
                        frame->sp -= count;
                        // Get the type
                        auto type = cast<Type *>(state->pop());
                        // Get the method
                        auto method = cast<ObjMethod *>(type->getMember(name));
                        // Call it
                        method->call(frame->sp + 1);
                        break;
                    }
                    case Opcode::SPINVOKE: {
                        auto method = cast<ObjMethod *>(getSymbol(state->loadConst(state->readShort())->toString()));
                        uint8 count = method->getFrameTemplate()->getArgs().count();
                        frame->sp -= count;
                        Obj *obj = state->pop();
                        method->call(frame->sp + 1);
                        state->getFrame()->getLocals().set(0, obj);
                        break;
                    }
                    case Opcode::SPFINVOKE: {
                        auto method = cast<ObjMethod *>(getSymbol(state->loadConst(state->readByte())->toString()));
                        uint8 count = method->getFrameTemplate()->getArgs().count();
                        frame->sp -= count;
                        Obj *obj = state->pop();
                        method->call(frame->sp + 1);
                        state->getFrame()->getLocals().set(0, obj);
                        break;
                    }
                    case Opcode::LINVOKE: {
                        // Get the method
                        auto method = cast<ObjMethod *>(frame->getLocals().get(state->readShort()));
                        // Get the arg count
                        uint8 count = (uint8) method->getFrameTemplate()->getArgs().count();
                        // Pop the arguments
                        frame->sp -= count;
                        // Call it
                        method->call(frame->sp);
                        break;
                    }
                    case Opcode::GINVOKE: {
                        // Get the method
                        auto method = cast<ObjMethod *>(getSymbol(state->loadConst(state->readShort())->toString()));
                        // Get the arg count
                        uint8 count = (uint8) method->getFrameTemplate()->getArgs().count();
                        // Pop the arguments
                        frame->sp -= count;
                        // Call it
                        method->call(frame->sp);
                        break;
                    }
                    case Opcode::VFINVOKE: {
                        // Get the param
                        Sign sign{state->loadConst(state->readByte())->toString()};
                        // Get name of the method
                        auto name = sign.getName();
                        // Get the arg count
                        uint8 count = static_cast<uint8>(sign.getParams().size());

                        // Pop the arguments
                        frame->sp -= count;
                        // Get the object
                        auto object = state->pop();
                        // Get the method
                        auto method = cast<ObjMethod *>(object->getMember(name));
                        // Call it
                        method->call(frame->sp + 1);
                        break;
                    }
                    case Opcode::SFINVOKE: {
                        // Get the param
                        Sign sign{state->loadConst(state->readByte())->toString()};
                        // Get name of the method
                        auto name = sign.getName();
                        // Get the arg count
                        uint8 count = static_cast<uint8>(sign.getParams().size());

                        // Pop the arguments
                        frame->sp -= count;
                        // Get the type
                        auto type = cast<Type *>(state->pop());
                        // Get the method
                        auto method = cast<ObjMethod *>(type->getMember(name));
                        // Call it
                        method->call(frame->sp + 1);
                        break;
                    }
                    case Opcode::LFINVOKE: {
                        // Get the method
                        auto method = cast<ObjMethod *>(frame->getLocals().get(state->readByte()));
                        // Get the arg count
                        uint8 count = (uint8) method->getFrameTemplate()->getArgs().count();
                        // Pop the arguments
                        frame->sp -= count;
                        // Call it
                        method->call(frame->sp);
                        break;
                    }
                    case Opcode::GFINVOKE: {
                        // Get the method
                        auto method = cast<ObjMethod *>(getSymbol(state->loadConst(state->readByte())->toString()));
                        // Get the arg count
                        uint8 count = (uint8) method->getFrameTemplate()->getArgs().count();
                        // Pop the arguments
                        frame->sp -= count;
                        // Call it
                        method->call(frame->sp);
                        break;
                    }
                    case Opcode::AINVOKE: {
                        // Get the method
                        auto method = cast<ObjMethod *>(frame->getArgs().get(state->readByte()));
                        // Get the arg count
                        uint8 count = (uint8) method->getFrameTemplate()->getArgs().count();
                        // Pop the arguments
                        frame->sp -= count;
                        // Call it
                        method->call(frame->sp);
                        break;
                    }
                    case Opcode::CALLSUB: {
                        auto address = halloc<ObjInt>(manager, frame->ip - frame->code);
                        state->push(address);
                        auto offset = state->readShort();
                        state->adjust(offset);
                        break;
                    }
                    case Opcode::RETSUB: {
                        auto address = cast<ObjInt *>(state->pop());
                        frame->setIp(frame->code + address->value());
                        break;
                    }
                    case Opcode::JFW: {
                        auto offset = state->readShort();
                        state->adjust(offset);
                        break;
                    }
                    case Opcode::JBW: {
                        auto offset = state->readShort();
                        state->adjust(-offset);
                        break;
                    }
                    case Opcode::JT: {
                        auto obj = state->pop();
                        auto offset = state->readShort();
                        if (obj->truth()) state->adjust(offset);
                        break;
                    }
                    case Opcode::JF: {
                        auto obj = state->pop();
                        auto offset = state->readShort();
                        if (!obj->truth()) state->adjust(offset);
                        break;
                    }
                    case Opcode::JLT: {
                        auto b = cast<ComparableObj *>(state->pop());
                        auto a = cast<ComparableObj *>(state->pop());
                        auto offset = state->readShort();
                        if ((*a < b)->truth()) state->adjust(offset);
                        break;
                    }
                    case Opcode::JLE: {
                        auto b = cast<ComparableObj *>(state->pop());
                        auto a = cast<ComparableObj *>(state->pop());
                        auto offset = state->readShort();
                        if ((*a <= b)->truth()) state->adjust(offset);
                        break;
                    }
                    case Opcode::JEQ: {
                        auto b = cast<ComparableObj *>(state->pop());
                        auto a = cast<ComparableObj *>(state->pop());
                        auto offset = state->readShort();
                        if ((*a == b)->truth()) state->adjust(offset);
                        break;
                    }
                    case Opcode::JNE: {
                        auto b = cast<ComparableObj *>(state->pop());
                        auto a = cast<ComparableObj *>(state->pop());
                        auto offset = state->readShort();
                        if ((*a != b)->truth()) state->adjust(offset);
                        break;
                    }
                    case Opcode::JGE: {
                        auto b = cast<ComparableObj *>(state->pop());
                        auto a = cast<ComparableObj *>(state->pop());
                        auto offset = state->readShort();
                        if ((*a >= b)->truth()) state->adjust(offset);
                        break;
                    }
                    case Opcode::JGT: {
                        auto b = cast<ComparableObj *>(state->pop());
                        auto a = cast<ComparableObj *>(state->pop());
                        auto offset = state->readShort();
                        if ((*a > b)->truth()) state->adjust(offset);
                        break;
                    }
                    case Opcode::NOT:
                        state->push(!*cast<ObjBool *>(state->pop()));
                        break;
                    case Opcode::INV:
                        state->push(~*cast<ObjInt *>(state->pop()));
                        break;
                    case Opcode::NEG:
                        state->push(-*cast<ObjInt *>(state->pop()));
                        break;
                    case Opcode::GETTYPE:
                        state->push(state->pop()->getType());
                        break;
                    case Opcode::SCAST: {
                        auto type = cast<Type *>(state->pop());
                        auto obj = state->pop();
                        if (checkCast(obj->getType(), type)) {
                            obj->setType(type);
                            state->push(obj);
                        } else
                            state->push(ObjNull::value());
                        break;
                    }
                    case Opcode::CCAST: {
                        auto type = cast<Type *>(state->pop());
                        auto obj = state->pop();
                        if (checkCast(obj->getType(), type)) {
                            obj->setType(type);
                            state->push(obj);
                        } else
                            runtimeError(std::format("object of type '{}' cannot be cast to object of type '{}'",
                                                     obj->getType()->getSign().toString(), type->getSign().toString()));
                        break;
                    }
                    case Opcode::POW: {
                        auto b = cast<ObjNumber *>(state->pop());
                        auto a = cast<ObjNumber *>(state->pop());
                        state->push(a->power(b));
                        break;
                    }
                    case Opcode::MUL: {
                        auto b = cast<ObjNumber *>(state->pop());
                        auto a = cast<ObjNumber *>(state->pop());
                        state->push(*a * b);
                        break;
                    }
                    case Opcode::DIV: {
                        auto b = cast<ObjNumber *>(state->pop());
                        auto a = cast<ObjNumber *>(state->pop());
                        state->push(*a / b);
                        break;
                    }
                    case Opcode::REM: {
                        auto b = cast<ObjInt *>(state->pop());
                        auto a = cast<ObjInt *>(state->pop());
                        state->push(*a % *b);
                        break;
                    }
                    case Opcode::ADD: {
                        auto b = cast<ObjNumber *>(state->pop());
                        auto a = cast<ObjNumber *>(state->pop());
                        state->push(*a + b);
                        break;
                    }
                    case Opcode::SUB: {
                        auto b = cast<ObjNumber *>(state->pop());
                        auto a = cast<ObjNumber *>(state->pop());
                        state->push(*a - b);
                        break;
                    }
                    case Opcode::SHL: {
                        auto b = cast<ObjInt *>(state->pop());
                        auto a = cast<ObjInt *>(state->pop());
                        state->push(*a << *b);
                        break;
                    }
                    case Opcode::SHR: {
                        auto b = cast<ObjInt *>(state->pop());
                        auto a = cast<ObjInt *>(state->pop());
                        state->push(*a >> *b);
                        break;
                    }
                    case Opcode::USHR: {
                        auto b = cast<ObjInt *>(state->pop());
                        auto a = cast<ObjInt *>(state->pop());
                        state->push(a->unsignedRightShift(*b));
                        break;
                    }
                    case Opcode::AND: {
                        auto b = cast<ObjInt *>(state->pop());
                        auto a = cast<ObjInt *>(state->pop());
                        state->push(*a & *b);
                        break;
                    }
                    case Opcode::OR: {
                        auto b = cast<ObjInt *>(state->pop());
                        auto a = cast<ObjInt *>(state->pop());
                        state->push(*a | *b);
                        break;
                    }
                    case Opcode::XOR: {
                        auto b = cast<ObjInt *>(state->pop());
                        auto a = cast<ObjInt *>(state->pop());
                        state->push(*a ^ *b);
                        break;
                    }
                    case Opcode::LT: {
                        auto b = cast<ComparableObj *>(state->pop());
                        auto a = cast<ComparableObj *>(state->pop());
                        state->push(*a < b);
                        break;
                    }
                    case Opcode::LE: {
                        auto b = cast<ComparableObj *>(state->pop());
                        auto a = cast<ComparableObj *>(state->pop());
                        state->push(*a <= b);
                        break;
                    }
                    case Opcode::EQ: {
                        auto b = cast<ComparableObj *>(state->pop());
                        auto a = cast<ComparableObj *>(state->pop());
                        state->push(*a == b);
                        break;
                    }
                    case Opcode::NE: {
                        auto b = cast<ComparableObj *>(state->pop());
                        auto a = cast<ComparableObj *>(state->pop());
                        state->push(*a != b);
                        break;
                    }
                    case Opcode::GE: {
                        auto b = cast<ComparableObj *>(state->pop());
                        auto a = cast<ComparableObj *>(state->pop());
                        state->push(*a >= b);
                        break;
                    }
                    case Opcode::GT: {
                        auto b = cast<ComparableObj *>(state->pop());
                        auto a = cast<ComparableObj *>(state->pop());
                        state->push(*a > b);
                        break;
                    }
                    case Opcode::IS: {
                        auto b = state->pop();
                        auto a = state->pop();
                        state->push(halloc<ObjBool>(manager, a == b));
                        break;
                    }
                    case Opcode::NIS: {
                        auto b = state->pop();
                        auto a = state->pop();
                        state->push(halloc<ObjBool>(manager, a != b));
                        break;
                    }
                    case Opcode::ISNULL:
                        state->push(halloc<ObjBool>(manager, is<ObjNull *>(state->pop())));
                        break;
                    case Opcode::NISNULL:
                        state->push(halloc<ObjBool>(manager, !is<ObjNull *>(state->pop())));
                        break;
                    case Opcode::ENTERMONITOR:
                    case Opcode::EXITMONITOR:
                        // todo: implement
                        break;
                    case Opcode::MTPERF: {
                        auto match = frame->getMatches()[state->readShort()];
                        uint32 offset = match.perform(state->pop());
                        state->adjust(offset);
                        break;
                    }
                    case Opcode::MTFPERF: {
                        auto match = frame->getMatches()[state->readByte()];
                        uint32 offset = match.perform(state->pop());
                        state->adjust(offset);
                        break;
                    }
                    case Opcode::CLOSURELOAD: {
                        auto method = cast<ObjMethod *>(state->pop()->copy());
                        auto &locals = const_cast<LocalsTable &>(method->getFrameTemplate()->getLocals());
                        for (uint16 i = locals.getClosureStart(); i < locals.count(); i++) {
                            NamedRef *ref;
                            switch (state->readByte()) {
                                case 0x00: // Arg as closure
                                    ref = frame->getArgs().getArg(state->readByte());
                                    break;
                                case 0x01: // Local as closure
                                    ref = frame->getLocals().getLocal(state->readShort());
                                    break;
                                case 0x02: // Type param as closure
                                    ref = frame->getMethod()->captureTypeParam(
                                            state->loadConst(state->readShort())->toString());
                                    break;
                                default:
                                    throw Unreachable();
                            }
                            locals.addClosure(ref);
                        }
                        break;
                    }
                    case Opcode::REIFIEDLOAD: {
                        uint8 count = state->readByte();
                        // Pop the arguments
                        for (int i = 0; i < count; i++) state->pop();
                        auto args = frame->sp;
                        auto obj = state->pop();
                        if (is<ObjMethod *>(obj)) {
                            state->push(cast<ObjMethod *>(obj)->getReified(args, count));
                        } else if (is<Type *>(obj)) {
                            state->push(cast<Type *>(obj)->getReified(args, count));
                        } else
                            throw runtimeError(
                                    std::format("cannot setPlaceholder value of type {}", obj->getType()->toString()));
                        break;
                    }
                    case Opcode::THROW: {
                        auto value = state->pop();
                        throw ThrowSignal(value);
                    }
                    case Opcode::RET: {
                        auto currentFrame = state->getFrame();
                        // Pop the return value
                        auto val = state->pop();
                        // Pop the current frame
                        state->popFrame();
                        // Return if encountered end of execution
                        if (topFrame == currentFrame) { return val; }
                        // Push the return value
                        state->getFrame()->push(val);
                        break;
                    }
                    case Opcode::VRET: {
                        auto currentFrame = state->getFrame();
                        // Pop the current frame
                        state->popFrame();
                        // Return if encountered end of execution
                        if (topFrame == currentFrame) { return ObjNull::value(manager); }
                        break;
                    }
                    case Opcode::PRINTLN:
                        // TODO: For debug only
                        write(state->pop()->toString() + "\n");
                        break;
                    case Opcode::NUM_OPCODES:
                        // No use
                        break;
                    case Opcode::I2F:
                        state->push(halloc<ObjFloat>(manager, static_cast<double>(cast<ObjInt *>(state->pop())->value())));
                        break;
                    case Opcode::F2I:
                        state->push(halloc<ObjInt>(manager, static_cast<int64>(cast<ObjFloat *>(state->pop())->value())));
                        break;
                    case Opcode::I2B:
                        state->push(ObjBool::value(cast<ObjInt *>(state->pop())->value() != 0, manager));
                        break;
                    case Opcode::B2I:
                        state->push(halloc<ObjInt>(manager, cast<ObjBool *>(state->pop())->truth() ? 1 : 0));
                        break;
                    case Opcode::O2B:
                        state->push(ObjBool::value(state->pop()->truth(), manager));
                        break;
                    case Opcode::O2S:
                        state->push(halloc<ObjString>(manager, state->pop()->toString()));
                        break;
                }
            } catch (const ThrowSignal &signal) {
                auto value = signal.getValue();
                while (state->getCallStackSize() > 0) {
                    frame = state->getFrame();
                    auto info = frame->getExceptions().getTarget(state->getPc(), value->getType());
                    if (Exception::IS_NO_EXCEPTION(info)) state->popFrame();
                    else {
                        state->setPc(info.getTarget());
                        state->push(value);
                        break;
                    }
                }
                if (state->getCallStackSize() == 0) {
                    // TODO: show stack trace
                }
            } catch (const FatalError &error) {
                std::cerr << "fatal error: " << error.what() << "\n";
                abort();
            }
        }
        return ObjNull::value(manager);
    }
} // namespace spade