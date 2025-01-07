#include "opcode.hpp"

struct Info {
    string name;
    int params;
    bool take;
};

static const Info OPCODE_TABLE[] = {
        {"nop",          0,  false},

        {"const",        1,  true },
        {"constl",       2,  true },
        {"pop",          0,  false},
        {"npop",         1,  false},
        {"dup",          0,  false},
        {"ndup",         1,  false},

        {"gload",        2,  true },
        {"gfload",       1,  true },
        {"gstore",       2,  true },
        {"gfstore",      1,  true },
        {"pgstore",      2,  true },
        {"pgfstore",     1,  true },
        {"lload",        2,  false},
        {"lfload",       1,  false},
        {"lstore",       2,  false},
        {"lfstore",      1,  false},
        {"plstore",      2,  false},
        {"plfstore",     1,  false},
        {"aload",        1,  false},
        {"astore",       1,  false},
        {"pastore",      1,  false},
        {"tload",        2,  true },
        {"tfload",       1,  true },
        {"tstore",       2,  true },
        {"tfstore",      1,  true },
        {"ptstore",      2,  true },
        {"ptfstore",     1,  true },
        {"mload",        2,  true },
        {"mfload",       1,  true },
        {"mstore",       2,  true },
        {"mfstore",      1,  true },
        {"pmstore",      2,  true },
        {"pmfstore",     1,  true },
        {"sload",        2,  true },
        {"sfload",       1,  true },
        {"sstore",       2,  true },
        {"sfstore",      1,  true },
        {"psstore",      2,  true },
        {"psfstore",     1,  true },
        {"spload",       2,  true },
        {"spfload",      1,  true },
        {"bload",        2,  false},
        {"bfload",       1,  false},

        {"arrpack",      0,  false},
        {"arrunpack",    0,  false},
        {"arrbuild",     2,  false},
        {"arrfbuild",    1,  false},
        {"iload",        0,  false},
        {"istore",       0,  false},
        {"pistore",      0,  false},
        {"arrlen",       0,  false},

        {"invoke",       1,  false},
        {"vinvoke",      2,  true },
        {"sinvoke",      2,  true },
        {"spinvoke",     2,  true },
        {"linvoke",      2,  false},
        {"ginvoke",      2,  true },
        {"ainvoke",      1,  false},
        {"vfinvoke",     1,  true },
        {"sfinvoke",     1,  true },
        {"spinvoke",     1,  true },
        {"lfinvoke",     1,  false},
        {"gfinvoke",     1,  true },

        {"callsub",      0,  false},
        {"retsub",       0,  false},

        {"jfw",          2,  false},
        {"jbw",          2,  false},
        {"jt",           2,  false},
        {"jf",           2,  false},
        {"jlt",          2,  false},
        {"jle",          2,  false},
        {"jeq",          2,  false},
        {"jne",          2,  false},
        {"jge",          2,  false},
        {"jgt",          2,  false},

        {"not",          0,  false},
        {"inv",          0,  false},
        {"neg",          0,  false},
        {"gettype",      0,  false},
        {"scast",        0,  false},
        {"ccast",        0,  false},
        {"pow",          0,  false},
        {"mul",          0,  false},
        {"div",          0,  false},
        {"rem",          0,  false},
        {"add",          0,  false},
        {"sub",          0,  false},
        {"shl",          0,  false},
        {"shr",          0,  false},
        {"ushr",         0,  false},
        {"and",          0,  false},
        {"or",           0,  false},
        {"xor",          0,  false},
        {"lt",           0,  false},
        {"le",           0,  false},
        {"eq",           0,  false},
        {"ne",           0,  false},
        {"ge",           0,  false},
        {"gt",           0,  false},
        {"is",           0,  false},
        {"nis",          0,  false},
        {"isnull",       0,  false},
        {"nisnull",      0,  false},

        {"i2f",          0,  false},
        {"f2i",          0,  false},
        {"i2b",          0,  false},
        {"b2i",          0,  false},
        {"o2b",          0,  false},
        {"o2s",          0,  false},

        {"entermonitor", 0,  false},
        {"exitmonitor",  0,  false},

        {"mtperf",       2,  false},
        {"mtfperf",      1,  false},
        {"closureload",  -1, false},
        {"reifiedload",  1,  false},
        {"objload",      0,  false},

        {"throw",        0,  false},
        {"ret",          0,  false},
        {"vret",         0,  false},

        {"println",      0,  false},
};

static_assert(static_cast<size_t>(Opcode::NUM_OPCODES) == std::size(OPCODE_TABLE), "update opcode table");

string OpcodeInfo::toString(Opcode opcode) { return OPCODE_TABLE[static_cast<int>(opcode)].name; }

uint8_t OpcodeInfo::getParams(Opcode opcode) { return OPCODE_TABLE[static_cast<int>(opcode)].params; }

bool OpcodeInfo::takeFromConstPool(Opcode opcode) { return OPCODE_TABLE[static_cast<int>(opcode)].take; }

Opcode OpcodeInfo::fromString(const string &str) {
    static std::map<string, Opcode> opcodes = {};
    // Populate the table if empty
    if (opcodes.empty()) {
        for (int i = 0; i < static_cast<int>(Opcode::NUM_OPCODES); ++i) {
            opcodes[OPCODE_TABLE[i].name] = static_cast<Opcode>(i);
        }
    }
    try {
        return opcodes.at(str);
    } catch (std::out_of_range &) {
        return Opcode::NOP;
    }
}
