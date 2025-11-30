#include "debug.hpp"
#include "objects/float.hpp"
#include "objects/inbuilt_types.hpp"
#include "objects/int.hpp"
#include "table.hpp"
#include "ee/vm.hpp"
#include <string>

namespace spade
{
    static void clear_console() {
#if defined(OS_WINDOWS)
        (void) system("cls");
#elif defined(OS_LINUX) || defined(OS_MAC)
        (void) system("clear");
#endif
    }

    void DebugOp::print_vm_state(const VMState *state) {
        string dummy;
        // Clear the console
        clear_console();
        // TODO Print memory
        // Print the call stack
        print_call_stack(state);
        // Print the current frameTemplate
        print_frame(state->get_frame());
        // Print the output
        std::cout << "Output\n" << state->get_vm()->get_output() << "\n";
        // Wait for input
        std::getline(std::cin, dummy);
    }

    void DebugOp::print_call_stack(const VMState *state) {
        CallStackTable table;
        auto callStack = state->get_call_stack();
        for (int i = state->get_call_stack_size() - 1; i >= 0; i--) {
            auto frame = &callStack[i];
            table.add(i, frame->get_method(), frame->get_args(), state->get_pc());
        }
        std::cout << table;
    }

    void DebugOp::print_frame(const Frame *frame) {
        print_const_pool(frame->get_const_pool());
        std::cout << "\n";
        std::cout << "Method: " << frame->get_method()->to_string() << "\n";
        std::cout << "\n";
        print_args(frame->get_args());
        print_locals(frame->get_locals());
        print_stack(frame->stack, frame->get_stack_count());
        std::cout << "\n";
        print_code(frame->code, frame->ip, frame->get_code_count(), frame->get_const_pool(), frame->get_lines());
        std::cout << "\n";
        print_exceptions(frame->get_exceptions());
    }

    void DebugOp::print_stack(Obj **const stack, uint32 count) {
        vector<const Obj *> items;
        items.reserve(count);
        for (int i = 0; i < count; ++i) items.push_back(stack[i]);
        std::cout << "Stack: [" << list_to_string(items) << "]\n";
    }

    void DebugOp::print_exceptions(const ExceptionTable &exceptions) {
        if (exceptions.count() == 0)
            return;
        ExcTable table;
        for (int i = 0; i < exceptions.count(); ++i) {
            auto const &exception = exceptions.get(i);
            table.add(exception.get_from(), exception.get_to(), exception.get_target(), exception.get_type());
        }
        std::cout << table;
    }

    void DebugOp::print_code(const uint8 *code, const uint8 *ip, const uint32 codeCount, const vector<Obj *> &pool, LineNumberTable lineTable) {
        if (codeCount == 0)
            return;
        auto byteLineMaxLen = std::to_string(codeCount - 1).length();
        auto sourceLineMaxLen = std::to_string(lineTable.get_line_infos().back().sourceLine).length() + 2;
        uint64 sourceLine = 0;
        uint32 i = 0;
        const auto read_byte = [&i, code]() -> uint8 { return code[i++]; };
        const auto read_short = [&i, code]() -> uint16 {
            i += 2;
            return code[i - 2] << 8 | code[i - 1];
        };
        while (i < codeCount) {
            uint64 sourceLineTemp = lineTable.get_source_line(i);
            string sourceLineStr;
            if (sourceLine != sourceLineTemp) {
                sourceLine = sourceLineTemp;
                sourceLineStr = pad_right(std::to_string(sourceLine) + " |", sourceLineMaxLen);
            } else {
                sourceLineStr = pad_right(" |", sourceLineMaxLen);
            }
            // Get the start of the line
            auto start = i;
            // Get the opcode
            auto opcode = static_cast<Opcode>(read_byte());
            // The parameters of the opcode, if any
            string param;
            switch (OpcodeInfo::params_count(opcode)) {
                case 1: {
                    uint8 num = read_byte();
                    string valStr = OpcodeInfo::take_from_const_pool(opcode) ? std::format("({})", pool[num]->to_string()) : "";
                    param = std::format("{} {}", num, valStr);
                    break;
                }
                case 2: {
                    uint16 num = read_short();
                    switch (opcode) {
                        case Opcode::JMP:
                        case Opcode::JT:
                        case Opcode::JF:
                        case Opcode::JLT:
                        case Opcode::JLE:
                        case Opcode::JEQ:
                        case Opcode::JNE:
                        case Opcode::JGE:
                        case Opcode::JGT:
                            num = static_cast<int8>(num);
                            break;
                        default:
                            break;
                    }
                    string valStr = OpcodeInfo::take_from_const_pool(opcode) ? std::format("({})", pool[num]->to_string()) : "";
                    param = std::format("{} {}", num, valStr);
                    break;
                }
                default:
                    param = "";
                    if (opcode == Opcode::CLOSURELOAD) {
                        auto count = read_byte();
                        param.append("[");
                        for (uint8 i = 0; i < count; i++) {
                            auto local_idx = read_short();
                            param.append(std::to_string(local_idx));
                            param.append("->");
                            switch (read_byte()) {
                                case 0:
                                    param.append("arg(");
                                    param.append(std::to_string(read_byte()));
                                    param.append(")");
                                    break;
                                case 1:
                                    param.append("local(");
                                    param.append(std::to_string(read_short()));
                                    param.append(")");
                                    break;
                                default:
                                    throw Unreachable();
                            }
                            param.append(", ");
                        }
                        param.pop_back();
                        param.pop_back();
                        param.append("]");
                    }
                    break;
            }
            string finalStr = std::format(" {} {: >{}}: {} {} {}\n", (start == ip - code - 1 ? ">" : " "), start, byteLineMaxLen, sourceLineStr,
                                          OpcodeInfo::to_string(opcode), param);
            std::cout << finalStr;
        }
    }

    void DebugOp::print_locals(const VariableTable &locals) {
        if (locals.count() == 0)
            return;
        LocalVarTable table;
        uint8 i;
        for (i = 0; i < locals.count(); ++i) {
            string name;
            if (locals.get_meta(i).contains("name"))
                name = locals.get_meta(i).at("name");
            else
                name = std::format("var{}", i);
            auto const &value = locals.get(i);
            table.add(i, name, value);
        }
        std::cout << table;
    }

    void DebugOp::print_args(const VariableTable &args) {
        if (args.count() == 0)
            return;
        ArgumentTable table;
        for (uint8 i = 0; i < args.count(); ++i) {
            string name;
            if (args.get_meta(i).contains("name"))
                name = args.get_meta(i).at("name");
            else
                name = std::format("var{}", i);
            auto const &value = args.get(i);
            table.add(i, name, value);
        }
        std::cout << table;
    }

    void DebugOp::print_const_pool(const vector<Obj *> &pool) {
        if (pool.empty())
            return;
        auto max = std::to_string(pool.size() - 1).length();
        std::cout << "Constant Pool\n";
        std::cout << "-------------\n";
        for (int i = 0; i < pool.size(); ++i) {
            const auto obj = pool.at(i);
            string type_str;
            if (obj->get_type()) {
                type_str = obj->get_type()->to_string();
            } else if (is<ObjNull>(obj))
                type_str = "<null>";
            else if (is<ObjBool>(obj))
                type_str = "<basic.bool>";
            else if (is<ObjChar>(obj))
                type_str = "<basic.char>";
            else if (is<ObjInt>(obj))
                type_str = "<basic.int>";
            else if (is<ObjFloat>(obj))
                type_str = "<basic.float>";
            else if (is<ObjString>(obj))
                type_str = "<basic.string>";
            else if (is<ObjArray>(obj))
                type_str = "<basic.Array>";
            else
                throw Unreachable();
            std::cout << std::format(" {: >{}d}: {} {}\n", i, max, type_str, obj->to_string());
        }
    }
}    // namespace spade
