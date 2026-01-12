#include "retriever.hpp"
#include <iostream>

#include <callable/method.hpp>
#include <ee/debugger.hpp>
#include <ee/thread.hpp>
#include <ee/vm.hpp>
#include <memory/basic/basic_manager.hpp>
#include <nite.hpp>
#include <spinfo/opcode.hpp>
#include <spdlog/details/log_msg.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace spade;
using namespace nite;

class PrettyDebugger;

template<typename Mutex>
class DebuggerSink : public spdlog::sinks::base_sink<Mutex> {
    PrettyDebugger *debugger;

  protected:
    void sink_it_(const spdlog::details::log_msg &msg) override;

    void flush_() override {}

  public:
    DebuggerSink(PrettyDebugger *debugger) : debugger(debugger) {}
};

using DebuggerSink_mt = DebuggerSink<std::mutex>;
using DebuggerSink_st = DebuggerSink<spdlog::details::null_mutex>;

class PrettyDebugger : public Debugger {
    std::vector<std::string> console;
    State &state;
    Position call_stack_pane_pivot;
    Position code_pivot;
    TextInputState command_line;

  public:
    PrettyDebugger() : state(GetState()) {
        console.push_back("");
    }

    void init(const SpadeVM *) {
        Initialize(state);
    }

    void update(const SpadeVM *vm) {
        if (ShouldWindowClose(state))
            return;

        bool loop = true;
        const auto &th_state = Thread::current()->get_state();
        const auto frame = th_state.get_frame();

        // clang-format off
        while (loop && !ShouldWindowClose(state)) {
            Event event;
            while (PollEvent(state, event)) {
                HandleEvent(event, [&](const KeyEvent &ev) {
                    if (!ev.key_down)
                        return;
                    if (ev.key_code == KeyCode::F4 && ev.modifiers == 0)
                        CloseWindow(state);
                });
            }

            BeginDrawing(state);
            BeginGridPane(state, {
                .pos = {0, 0},
                .size = GetPaneSize(state),
                .col_sizes = {27, 27, 46},
                .row_sizes = {50, 50},
            });

            BeginGridCell(state, 0, 0); {
                BeginBorder(state, BOX_BORDER_LIGHT);
                Text(state, {
                    .text = " Call Stack ",
                    .pos = {2, 0},
                });
                FillBackground(state, Color::from_hex(0x201640));
                EndBorder(state);
                FillBackground(state, Color::from_hex(0x201640));
                CallStack(th_state);
            } EndPane(state);

            BeginGridCell(state, 1, 0); {
                BeginBorder(state, BOX_BORDER_LIGHT);
                Text(state, {
                    .text = " Code ",
                    .pos = {2, 0},
                });
                FillBackground(state, Color::from_hex(0x3b4261));
                EndBorder(state);
                FillBackground(state, Color::from_hex(0x3b4261));
                Code(frame);
            } EndPane(state);

            BeginGridCell(state, 0, 1); {
                BeginBorder(state, BOX_BORDER_LIGHT);
                Text(state, {
                    .text = " Args ",
                    .pos = {2, 0},
                });
                FillBackground(state, Color::from_hex(0x3b4261));
                EndBorder(state);
                FillBackground(state, Color::from_hex(0x3b4261));
                VarTable(frame->get_args());
            } EndPane(state);

            BeginGridCell(state, 1, 1); {
                BeginBorder(state, BOX_BORDER_LIGHT);
                Text(state, {
                    .text = " Locals ",
                    .pos = {2, 0},
                });
                FillBackground(state, Color::from_hex(0x201640));
                EndBorder(state);
                FillBackground(state, Color::from_hex(0x201640));
                VarTable(frame->get_locals());
            } EndPane(state);

            BeginGridCell(state, 2, 0); {
                BeginGridPane(state, {
                    .pos = {0, 0},
                    .size = GetPaneSize(state),
                    .col_sizes = {50, 50},
                    .row_sizes = {100},
                });
                BeginGridCell(state, 0, 0); {
                    // Operand stack
                    Text(state, {
                        .text = "> Operand Stack",
                        .pos = {0, 0},
                    });
                    OperandStack(frame);
                } EndPane(state);
                BeginGridCell(state, 1, 0); {
                    BeginBorder(state, BOX_BORDER_LIGHT);
                    Text(state, {
                        .text = " Output ",
                        .pos = {2, 0},
                    });
                    FillBackground(state, Color::from_hex(0x201640));
                    EndBorder(state);
                    // The output
                    TextBox(state, {
                        .text = vm->get_output(),
                        .pos = {0, 0},
                        .size = GetPaneSize(state),
                        .style = {.bg = Color::from_hex(0x201640), .fg = COLOR_WHITE},
                    });
                } EndPane(state);
                EndPane(state);
                // TODO: show exception info (this is part of command)
            } EndPane(state);

            BeginGridCell(state, 2, 1); {
                BeginBorder(state, BOX_BORDER_LIGHT);
                Text(state, {
                    .text = " Debug Console ",
                    .pos = {2, 0},
                });
                FillBackground(state, Color::from_hex(0x3b4261));
                // Command prompt
                Text(state, {
                    .text = "> ",
                    .pos = {0, GetPaneSize(state).height - 1},
                });
                TextField(state, command_line, {
                    .pos = {2, GetPaneSize(state).height - 1},
                    .width = GetPaneSize(state).width - 2,
                    .on_enter = [&](TextFieldInfo &) {
                        auto command = command_line.delete_all();
                        loop = false;

                        println(command);
                        if (command == "q") CloseWindow(state);
                        if (command == "clear") console = {""};
                    },
                });
                EndBorder(state);
                // The output
                TextBox(state, {
                    .text = GetVisibleConsoleText(),
                    // .text = "",
                    .pos = {0, 0},
                    .size = GetPaneSize(state),
                    .style = {.bg = Color::from_hex(0x3b4261), .fg = COLOR_WHITE},
                    .wrap = false,
                });
            } EndPane(state);

            EndPane(state);
            EndDrawing(state);
        }
        // clang-format on
    }

    void cleanup(const SpadeVM *) {
        Cleanup();
    }

    void print(const string &str) {
        for (const char c: str) {
            if (c == '\n') {
                console.push_back("");
            } else {
                console.back() += c;
            }
        }
    }

    void println(const string &str) {
        print(str);
        print("\n");
    }

  private:
    struct Instruction {
        uint32_t start;
        string source_line_str;
        Opcode opcode;
        string param;
    };

    string GetVisibleConsoleText() {
        const auto max_lines = GetPaneSize(state).height;
        vector<string> lines;
        if (console.back().empty()) {
            lines.insert(lines.begin(), console.begin(), console.end() - 1);
        } else {
            lines = console;
        }

        string result;
        if (lines.size() < max_lines) {
            // Return all
            for (size_t i = 0; i < lines.size(); i++) {
                result += lines[i];
                if (i < lines.size() - 1)
                    result += '\n';
            }
        } else {
            // keep [size()-max_lines, size())
            for (size_t i = lines.size() - max_lines; i < lines.size(); i++) {
                result += lines[i];
                if (i < lines.size() - 1)
                    result += '\n';
            }
        }
        return result;
    }

    void Code(const Frame *frame) {
        const auto code = &frame->code[0];
        const auto ip = frame->ip;
        const auto code_count = frame->get_code_count();
        const auto &pool = frame->get_const_pool();
        const auto &line_table = frame->get_lines();

        if (code_count == 0)
            return;

        const auto byte_line_max_len = std::to_string(code_count - 1).length();
        const auto source_line_max_len = std::to_string(line_table.get_line_infos().back().sourceLine).length() + 2;
        vector<Instruction> instructions;
        size_t active_instr = 0;
        uint64_t source_line = 0;
        uint32_t i = 0;
        const auto read_byte = [&i, code]() -> uint8_t { return code[i++]; };
        const auto read_short = [&i, code]() -> uint16_t {
            i += 2;
            return code[i - 2] << 8 | code[i - 1];
        };

        while (i < code_count) {
            // Compute source line
            uint64_t source_line_tmp = line_table.get_source_line(i);
            string source_line_str;
            if (source_line != source_line_tmp) {
                // If the current source line is different from the prev one then show the line number
                source_line = source_line_tmp;
                source_line_str = pad_right(std::to_string(source_line) + " |", source_line_max_len);
            } else {
                // If the current source line is same as the prev source line then do not show the line number
                source_line_str = pad_right(" |", source_line_max_len);
            }

            // Get the start of the line
            const auto start = i;
            // Get the opcode
            const auto opcode = static_cast<Opcode>(read_byte());
            // Evaluate parameters of the opcode
            string param;
            switch (OpcodeInfo::params_count(opcode)) {
            case 1: {
                const auto num = read_byte();
                string val_str = OpcodeInfo::take_from_const_pool(opcode) ? std::format("({})", pool[num]->to_string()) : "";
                param = std::format("{} {}", num, val_str);
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
                case Opcode::JGT: {
                    const auto offset = static_cast<int16_t>(num);
                    param = std::to_string(offset);
                    break;
                }
                default:
                    string val_str = OpcodeInfo::take_from_const_pool(opcode) ? std::format("({})", pool[num]->to_string()) : "";
                    param = std::format("{} {}", num, val_str);
                    break;
                }
                break;
            }
            default:
                param = "";
                if (opcode == Opcode::CLOSURELOAD) {
                    const auto count = read_byte();
                    param.append("[");
                    for (uint8_t i = 0; i < count; i++) {
                        const auto local_idx = read_short();
                        string kind;
                        size_t to_idx;
                        switch (read_byte()) {
                        case 0:
                            kind = "arg";
                            to_idx = read_byte();
                            break;
                        case 1:
                            kind = "local";
                            to_idx = read_short();
                            break;
                        default:
                            throw Unreachable();
                        }
                        param += std::format("{}->{}({}), ", local_idx, kind, to_idx);
                    }
                    if (param.back() != '[') {
                        param.pop_back();
                        param.pop_back();
                    }
                    param.append("]");
                }
                break;
            }

            if (start == ip - code - 1)
                active_instr = instructions.size();
            instructions.emplace_back(start, source_line_str, opcode, param);
        }

        // Now set the things in order
        for (size_t i = 0; i < instructions.size(); i++) {
            const auto &instr = instructions[i];

            if (i < GetPaneSize(state).height) {
                Color line_bg;
                if (i == active_instr)
                    line_bg = Color::from_hex(0x400296);
                else
                    line_bg = Color::from_hex(0x201640);
                // 82ff9e
                // clang-format off
                RichTextBox(state, {
                    .text = color_fmt("%(#{},#FFFFFF){} %(#{},#FFB626){: >{}}%(#{},#FFFFFF): {} %(#{},#82ff9e){}%(#{},#FFFFFF) {}", 
                                line_bg.to_string_hex(), " ",
                                line_bg.to_string_hex(), instr.start, byte_line_max_len, line_bg.to_string_hex(),
                                instr.source_line_str, 
                                line_bg.to_string_hex(), OpcodeInfo::to_string(instr.opcode), line_bg.to_string_hex(),
                                instr.param),
                    .pos = {0, i},
                    .size = {GetPaneSize(state).width, 1},
                    .style = {.bg = line_bg, .fg = COLOR_WHITE},
                });
                // clang-format on
            }
        }
    }

    void OperandStack(const Frame *frame) {
        vector<string> data;
        for (Obj **sp = &frame->stack[0]; sp < frame->sp; sp++) {
            const auto text = " " + (*sp)->to_string();
            data.push_back(text);
        }
        // clang-format off
        if (data.empty()) {
            Text(state, {
                .text = "<empty>",
                .pos = {0, 1},
            });
        } else { 
            SimpleTable(state, {
                .data = data,
                .include_header_row = false,
                .num_cols = 1,
                .num_rows = data.size(),
                .pos = {0, 1},
                .table_style = {.bg = Color::from_hex(0x3936ad), .fg = COLOR_WHITE},
                .show_border = false,
                .border = TABLE_BORDER_LIGHT,
            }); 
        }
        // clang-format on
    }

    void VarTable(const VariableTable &table) {
        vector<string> data{" index", " value"};
        for (uint8_t i = 0; i < table.count(); i++) {
            data.push_back(std::to_string(i));
            data.push_back(table.get(i)->to_string());
        }

        // clang-format off
        SimpleTable(state, {
           .data = data,
           .include_header_row = true,
           .num_cols = 2,
           .num_rows = data.size() / 2,
           .pos = {0, 0},
           .header_style = {.bg = Color::from_hex(0x345c25), .fg = COLOR_WHITE},
           .table_style = {.bg = Color::from_hex(0x104876), .fg = COLOR_WHITE},
           .show_border = false,
           .border = TABLE_BORDER_LIGHT,
        });
        // clang-format on
    }

    void CallStack(const ThreadState &th_state) {
        vector<string> table{" index", " method"};

        const auto call_stack = th_state.get_call_stack();
        for (auto frame = th_state.get_frame(); frame >= call_stack; frame--) {
            table.push_back(std::to_string(table.size() / 2 - 1));
            table.push_back(frame->get_method()->to_string());
        }

        // clang-format off
        SimpleTable(state, {
           .data = table,
           .include_header_row = true,
           .num_cols = 2,
           .num_rows = table.size() / 2,
           .pos = {0, 0},
           .header_style = {.bg = Color::from_hex(0x345c25), .fg = COLOR_WHITE},
           .table_style = {.bg = Color::from_hex(0x104876), .fg = COLOR_WHITE},
           .show_border = false,
           .border = TABLE_BORDER_LIGHT,
        });
        // clang-format on
    }
};

int main1() {
    pretty::Retriever commands;
    commands.add_command("breakpoint");
    commands.add_command("breakdo");
    commands.add_command("break");
    commands.add_command("watchpoint");
    commands.add_command("thread");
    commands.add_command("frame");
    commands.add_command("print");

    std::cout << string(15, '-') << std::endl;

    auto result = commands.get_command("br");
    for (const auto &[name, _]: result) {
        std::cout << "name: " << name << std::endl;
    }

    std::cout << string(15, '-') << std::endl;

    // commands.print();
    return 0;
}

int main(int argc, char **argv) {
    vector<string> args;
    for (int i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    }

    auto debugger = std::make_unique<PrettyDebugger>();
    auto manager = std::make_unique<basic::BasicMemoryManager>();

    // auto debugger_sink = std::make_shared<DebuggerSink_mt>(&*debugger);
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    // auto logger = std::make_shared<spdlog::logger>("swan", spdlog::sinks_init_list{stdout_sink, debugger_sink});
    auto logger = std::make_shared<spdlog::logger>("swan", stdout_sink);
    // logger->set_pattern("[%Y-%m-%d %H:%M:%S %z] [thread %t] [%^%l%$] %v");
    logger->set_pattern("[%^%l%$] %v");
    spdlog::set_default_logger(logger);
    spdlog::default_logger();

    try {
        SpadeVM vm(manager.get(), std::move(debugger));
        vm.start("../swan/res/hello.elp", args, true);
        std::cout << vm.get_output();
        spdlog::info("VM exited with code {}", vm.get_exit_code());
        return vm.get_exit_code();
    } catch (const SpadeError &error) {
        std::cerr << "VM Error: " << error.what() << std::endl;
        return 1;
    }
}
