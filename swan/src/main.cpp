#include "callable/table.hpp"
#include "ee/debugger.hpp"
#include "ee/vm.hpp"
#include "ee/thread.hpp"
#include "spinfo/opcode.hpp"
#include <memory>
#include <nite.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <string>

using namespace spade;
using namespace nite;

class PrettyDebugger : public Debugger {
    State &state;
    Position call_stack_pane_pivot;
    Position code_pivot;

  public:
    PrettyDebugger() : state(GetState()) {}

    void init(const SpadeVM *) {
        Initialize(state);
    }

    void update(const SpadeVM *vm) {
        if (ShouldWindowClose(state))
            return;

        bool loop = true;
        const auto &th_state = Thread::current()->get_state();

        // clang-format off
        while (loop && !ShouldWindowClose(state)) {
            Event event;
            while (PollEvent(state, event)) {
                HandleEvent(event, [&](const KeyEvent &ev) {
                    if (!ev.key_down)
                        return;
                    if (ev.key_code == KeyCode::ESCAPE && ev.modifiers == 0)
                        CloseWindow(state);
                    if (ev.key_code == KeyCode::ENTER && ev.modifiers == 0)
                        loop = false;
                });
            }

            BeginDrawing(state);
            BeginGridPane(state, {
                .size = GetPaneSize(state),
                .col_sizes = {50, 50},
                .row_sizes = {100},
            });

            BeginGridCell(state, 0, 0); {
                Code(th_state.get_frame());
                FillBackground(state, Color::from_hex(0x201640));
            } EndPane(state);

            BeginGridCell(state, 1, 0); {
                BeginGridPane(state, {
                    .size = GetPaneSize(state),
                    .col_sizes = {100},
                    .row_sizes = {50, 50},
                });

                BeginGridCell(state, 0, 0); {
                    CallStack(th_state);
                    FillBackground(state, Color::from_hex(0x3b4261));
                } EndPane(state);
                BeginGridCell(state, 0, 1); {
                    BeginBorder(state, BOX_BORDER_LIGHT);
                    Text(state, {
                        .text = " Output ",
                        .pos = {2, 0},
                    });
                    EndBorder(state);

                    TextBox(state, {
                        .text = vm->get_output(),
                        .size = GetPaneSize(state),
                    });
                } EndPane(state);

                EndPane(state);
            } EndPane(state);
            
            EndPane(state);
            EndDrawing(state);
        }
        // clang-format on
    }

    void cleanup(const SpadeVM *) {
        Cleanup();
    }

  private:
    void Instructions(size_t &cur_col, const uint8_t *code, const uint8_t *ip, const uint32_t code_count, const vector<Obj *> pool,
                      const LineNumberTable &line_table) const {
        if (code_count == 0)
            return;

        auto byte_line_max_len = std::to_string(code_count - 1).length();
        auto source_line_max_len = std::to_string(line_table.get_line_infos().back().sourceLine).length() + 2;
        uint64_t source_line = 0;
        uint32_t i = 0;
        const auto read_byte = [&i, code]() -> uint8_t { return code[i++]; };
        const auto read_short = [&i, code]() -> uint16_t {
            i += 2;
            return code[i - 2] << 8 | code[i - 1];
        };
        while (i < code_count) {
            uint64_t source_line_temp = line_table.get_source_line(i);
            string source_line_str;
            if (source_line != source_line_temp) {
                source_line = source_line_temp;
                source_line_str = pad_right(std::to_string(source_line) + " |", source_line_max_len);
            } else {
                source_line_str = pad_right(" |", source_line_max_len);
            }
            // Get the start of the line
            auto start = i;
            // Get the opcode
            auto opcode = static_cast<Opcode>(read_byte());
            // The parameters of the opcode, if any
            string param;
            switch (OpcodeInfo::params_count(opcode)) {
            case 1: {
                uint8_t num = read_byte();
                string valStr = OpcodeInfo::take_from_const_pool(opcode) ? std::format("({})", pool[num]->to_string()) : "";
                param = std::format("{} {}", num, valStr);
                break;
            }
            case 2: {
                uint16_t num = read_short();
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
                    num = static_cast<int8_t>(num);
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
                    for (uint8_t i = 0; i < count; i++) {
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
            string final_str = std::format(" {} {: >{}}: {} {} {}", (start == ip - code - 1 ? ">" : " "), start, byte_line_max_len, source_line_str,
                                           OpcodeInfo::to_string(opcode), param);
            // clang-format off
            Text(state, {
                .text = final_str,
                .pos = {0, cur_col++},
            });
            // clang-format on
        }
    }

    void Code(const Frame *frame) {
        size_t max_width = 1;
        vector<string> data;
        for (Obj **sp = &frame->stack[0]; sp < frame->sp; sp++) {
            const auto text = " " + (*sp)->to_string();
            data.push_back(text);
            max_width += text.size() + 3;
        }

        Size max_size = Size{.width = max_width, .height = GetPaneSize(state).height};

        // clang-format off
        BeginScrollPane(state, code_pivot, {
            .min_size = GetPaneSize(state),
            .max_size = max_size,
            .scroll_bar = SCROLL_LIGHT,
            .show_hscroll_bar = false,
        }); {
            Text(state, {
                .text = "Operand stack",
                .pos = {0, 0},
            });
            if (!data.empty())
                SimpleTable(state, {
                    .data = data,
                    .include_header_row = false,
                    .num_cols = data.size(),
                    .num_rows = 1,
                    .pos = {0, 1},
                    .show_border = true,
                    .border = TABLE_BORDER_LIGHT,
                });
            else
                Text(state, {
                    .text = "    <No stack>",
                    .pos = {0, 2},
                });

            Text(state, {
                .text = "Bytecode",
                .pos = {0, 5},
            });

            size_t cur_col = 6;
            Instructions(cur_col, &frame->code[0], frame->ip, frame->get_code_count(), frame->get_const_pool(), frame->get_lines());
        } EndPane(state);
        // clang-format on
    }

    void CallStack(const ThreadState &th_state) {
        vector<string> table{" i", " method", " args"};

        const auto call_stack = th_state.get_call_stack();
        for (auto frame = th_state.get_frame(); frame >= call_stack; frame--) {
            table.push_back(std::to_string(table.size() / 3 - 1));
            table.push_back(frame->get_method()->to_string());
            table.push_back(frame->get_args().to_string());
        }

        // clang-format off
        BeginNoPane(state);
        Size size = SimpleTable(state, {
           .data = table,
           .include_header_row = true,
           .num_cols = 3,
           .num_rows = table.size() / 3,
           .show_border = true,
           .border = TABLE_BORDER_LIGHT,
        });
        EndPane(state);

        BeginScrollPane(state, call_stack_pane_pivot, {
            .min_size = GetPaneSize(state),
            .max_size = size + Size(0, 1),
            .scroll_bar = SCROLL_LIGHT,
            .scroll_factor = 1.5,
        });
        Text(state, {
            .text = "Call Stack",
            .pos = {0, 0},
        });
        SimpleTable(state, {
           .data = table,
           .include_header_row = true,
           .num_cols = 3,
           .num_rows = table.size() / 3,
           .pos = {0, 1},
           .show_border = true,
           .border = TABLE_BORDER_LIGHT,
        });
        EndPane(state);
        // clang-format on
    }
};

int main(int argc, char **argv) {
    vector<string> args;
    for (int i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    }

    auto logger = spdlog::stdout_color_mt("console");
    logger->set_pattern("[%Y-%m-%d %H:%M:%S %z] [thread %t] [%^%l%$] %v");
    spdlog::set_default_logger(logger);

    try {
        SpadeVM vm(null, std::make_unique<PrettyDebugger>());
        vm.start("../swan/res/hello.elp", args, true);
        std::cout << vm.get_output();
        spdlog::info("VM exited with code {}", vm.get_exit_code());
        return vm.get_exit_code();
    } catch (const SpadeError &error) {
        std::cerr << "VM Error: " << error.what() << std::endl;
        return 1;
    }
}
