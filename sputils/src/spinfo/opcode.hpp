#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include "spimp/error.hpp"

// OPCODE(...) -> OPCODE(name, param_count, take, alternate)
// Here:
//  [Opcode]    name            -> name of the opcode
//  [uint8_t]   param_count     -> number of increments done on `ip` to decode the parameter (-1 if variadic)
//  [bool]      take            -> whether the parameter represents an index to the constant pool
//  [Opcode]    alternate       -> an alternate opcode (if there is no alternate, `alternate` is same as `name`)
#define LIST_OF_OPCODES                                                                                                                              \
    /* no operation */                                                                                                                               \
    OPCODE(NOP, 0, false, NOP)                                                                                                                       \
    /* ----------------------------------------------------- */                                                                                      \
    /* stack op */                                                                                                                                   \
    /* ----------------------------------------------------- */                                                                                      \
    /* load constant 'null' */                                                                                                                       \
    OPCODE(CONST_NULL, 0, true, CONST_NULL)                                                                                                          \
    /* load constant 'true' */                                                                                                                       \
    OPCODE(CONST_TRUE, 0, true, CONST_TRUE)                                                                                                          \
    /* load constant 'false' */                                                                                                                      \
    OPCODE(CONST_FALSE, 0, true, CONST_FALSE)                                                                                                        \
    /* load constant */                                                                                                                              \
    OPCODE(CONST, 1, true, CONSTL)                                                                                                                   \
    /* load long constant */                                                                                                                         \
    OPCODE(CONSTL, 2, true, CONST)                                                                                                                   \
    /* pop */                                                                                                                                        \
    OPCODE(POP, 0, false, POP)                                                                                                                       \
    /* pop n items from the top of stack */                                                                                                          \
    OPCODE(NPOP, 1, false, NPOP)                                                                                                                     \
    /* duplicate top of stack */                                                                                                                     \
    OPCODE(DUP, 0, false, DUP)                                                                                                                       \
    /* dup top of tack n times */                                                                                                                    \
    OPCODE(NDUP, 1, false, NDUP)                                                                                                                     \
    /* ----------------------------------------------------- */                                                                                      \
    /* load store op */                                                                                                                              \
    /* ----------------------------------------------------- */                                                                                      \
    /* load global */                                                                                                                                \
    OPCODE(GLOAD, 2, true, GFLOAD)                                                                                                                   \
    /* load global fast */                                                                                                                           \
    OPCODE(GFLOAD, 1, true, GLOAD)                                                                                                                   \
    /* store global */                                                                                                                               \
    OPCODE(GSTORE, 2, true, GFSTORE)                                                                                                                 \
    /* store global fast */                                                                                                                          \
    OPCODE(GFSTORE, 1, true, GSTORE)                                                                                                                 \
    /* pop store global */                                                                                                                           \
    OPCODE(PGSTORE, 2, true, PGFSTORE)                                                                                                               \
    /* pop store global fast */                                                                                                                      \
    OPCODE(PGFSTORE, 1, true, PGSTORE)                                                                                                               \
    /* ----------------------------------------------------- */                                                                                      \
    /* load local */                                                                                                                                 \
    OPCODE(LLOAD, 2, false, LFLOAD)                                                                                                                  \
    /* load local fast */                                                                                                                            \
    OPCODE(LFLOAD, 1, false, LLOAD)                                                                                                                  \
    /* store local */                                                                                                                                \
    OPCODE(LSTORE, 2, false, LFSTORE)                                                                                                                \
    /* store local fast */                                                                                                                           \
    OPCODE(LFSTORE, 1, false, LSTORE)                                                                                                                \
    /* pop store local */                                                                                                                            \
    OPCODE(PLSTORE, 2, false, PLFSTORE)                                                                                                              \
    /* pop store local fast */                                                                                                                       \
    OPCODE(PLFSTORE, 1, false, PLSTORE)                                                                                                              \
    /* ----------------------------------------------------- */                                                                                      \
    /* load arg */                                                                                                                                   \
    OPCODE(ALOAD, 1, false, ALOAD)                                                                                                                   \
    /* store arg */                                                                                                                                  \
    OPCODE(ASTORE, 1, false, ASTORE)                                                                                                                 \
    /* pop store arg */                                                                                                                              \
    OPCODE(PASTORE, 1, false, PASTORE)                                                                                                               \
    /* ----------------------------------------------------- */                                                                                      \
    /* load member */                                                                                                                                \
    OPCODE(MLOAD, 2, true, MFLOAD)                                                                                                                   \
    /* load member fast */                                                                                                                           \
    OPCODE(MFLOAD, 1, true, MLOAD)                                                                                                                   \
    /* store member */                                                                                                                               \
    OPCODE(MSTORE, 2, true, MFSTORE)                                                                                                                 \
    /* store member fast */                                                                                                                          \
    OPCODE(MFSTORE, 1, true, MSTORE)                                                                                                                 \
    /* pop store member */                                                                                                                           \
    OPCODE(PMSTORE, 2, true, PMFSTORE)                                                                                                               \
    /* pop store member fast */                                                                                                                      \
    OPCODE(PMFSTORE, 1, true, PMSTORE)                                                                                                               \
    /* ----------------------------------------------------- */                                                                                      \
    /* array op */                                                                                                                                   \
    /* ----------------------------------------------------- */                                                                                      \
    /* pack array */                                                                                                                                 \
    OPCODE(ARRPACK, 0, false, ARRPACK)                                                                                                               \
    /* unpack array */                                                                                                                               \
    OPCODE(ARRUNPACK, 0, false, ARRUNPACK)                                                                                                           \
    /* build array */                                                                                                                                \
    OPCODE(ARRBUILD, 2, false, ARRFBUILD)                                                                                                            \
    /* build array fast */                                                                                                                           \
    OPCODE(ARRFBUILD, 1, false, ARRBUILD)                                                                                                            \
    /* load array index */                                                                                                                           \
    OPCODE(ILOAD, 0, false, ILOAD)                                                                                                                   \
    /* store array index */                                                                                                                          \
    OPCODE(ISTORE, 0, false, ISTORE)                                                                                                                 \
    /* pop store array index */                                                                                                                      \
    OPCODE(PISTORE, 0, false, PISTORE)                                                                                                               \
    /* array length */                                                                                                                               \
    OPCODE(ARRLEN, 0, false, ARRLEN)                                                                                                                 \
    /* ----------------------------------------------------- */                                                                                      \
    /* call op */                                                                                                                                    \
    /* ----------------------------------------------------- */                                                                                      \
    /* invoke */                                                                                                                                     \
    OPCODE(INVOKE, 1, false, INVOKE)                                                                                                                 \
    /* invoke virtual */                                                                                                                             \
    OPCODE(VINVOKE, 2, true, VFINVOKE)                                                                                                               \
    /* invoke super class method */                                                                                                                  \
    OPCODE(SPINVOKE, 2, true, SPFINVOKE)                                                                                                             \
    /* invoke local */                                                                                                                               \
    OPCODE(LINVOKE, 2, false, LFINVOKE)                                                                                                              \
    /* invoke global */                                                                                                                              \
    OPCODE(GINVOKE, 2, true, GFINVOKE)                                                                                                               \
    /* invoke arg */                                                                                                                                 \
    OPCODE(AINVOKE, 1, false, AINVOKE)                                                                                                               \
    /* invoke virtual fast */                                                                                                                        \
    OPCODE(VFINVOKE, 1, true, VINVOKE)                                                                                                               \
    /* invoke super class method fast */                                                                                                             \
    OPCODE(SPFINVOKE, 1, true, SPINVOKE)                                                                                                             \
    /* invoke local fast */                                                                                                                          \
    OPCODE(LFINVOKE, 1, false, LINVOKE)                                                                                                              \
    /* invoke global fast */                                                                                                                         \
    OPCODE(GFINVOKE, 1, true, GINVOKE)                                                                                                               \
    /* sub call */                                                                                                                                   \
    OPCODE(CALLSUB, 0, false, CALLSUB)                                                                                                               \
    /* sub return */                                                                                                                                 \
    OPCODE(RETSUB, 0, false, RETSUB)                                                                                                                 \
    /* ----------------------------------------------------- */                                                                                      \
    /* jump op */                                                                                                                                    \
    /* ----------------------------------------------------- */                                                                                      \
    /* relative jump forward or backward */                                                                                                          \
    OPCODE(JMP, 2, false, JMP)                                                                                                                       \
    /* pop jump if true */                                                                                                                           \
    OPCODE(JT, 2, false, JT)                                                                                                                         \
    /* pop jump if false */                                                                                                                          \
    OPCODE(JF, 2, false, JF)                                                                                                                         \
    /* pop jump if less than */                                                                                                                      \
    OPCODE(JLT, 2, false, JLT)                                                                                                                       \
    /* pop jump if less than or equal */                                                                                                             \
    OPCODE(JLE, 2, false, JLE)                                                                                                                       \
    /* pop jump if equal */                                                                                                                          \
    OPCODE(JEQ, 2, false, JEQ)                                                                                                                       \
    /* pop jump if not equal */                                                                                                                      \
    OPCODE(JNE, 2, false, JNE)                                                                                                                       \
    /* pop jump if greater than or equal */                                                                                                          \
    OPCODE(JGE, 2, false, JGE)                                                                                                                       \
    /* pop jump if greater than */                                                                                                                   \
    OPCODE(JGT, 2, false, JGT)                                                                                                                       \
    /* ----------------------------------------------------- */                                                                                      \
    /* primitive op */                                                                                                                               \
    /* ----------------------------------------------------- */                                                                                      \
    /* not */                                                                                                                                        \
    OPCODE(NOT, 0, false, NOT)                                                                                                                       \
    /* invert */                                                                                                                                     \
    OPCODE(INV, 0, false, INV)                                                                                                                       \
    /* negate */                                                                                                                                     \
    OPCODE(NEG, 0, false, NEG)                                                                                                                       \
    /* get type */                                                                                                                                   \
    OPCODE(GETTYPE, 0, false, GETTYPE)                                                                                                               \
    /* safe cast */                                                                                                                                  \
    OPCODE(SCAST, 0, false, SCAST)                                                                                                                   \
    /* checked cast */                                                                                                                               \
    OPCODE(CCAST, 0, false, CCAST)                                                                                                                   \
    /* concat */                                                                                                                                     \
    OPCODE(CONCAT, 0, false, CONCAT)                                                                                                                 \
    /* power */                                                                                                                                      \
    OPCODE(POW, 0, false, POW)                                                                                                                       \
    /* multiply */                                                                                                                                   \
    OPCODE(MUL, 0, false, MUL)                                                                                                                       \
    /* division */                                                                                                                                   \
    OPCODE(DIV, 0, false, DIV)                                                                                                                       \
    /* remainder */                                                                                                                                  \
    OPCODE(REM, 0, false, REM)                                                                                                                       \
    /* addition */                                                                                                                                   \
    OPCODE(ADD, 0, false, ADD)                                                                                                                       \
    /* subtraction */                                                                                                                                \
    OPCODE(SUB, 0, false, SUB)                                                                                                                       \
    /* shift left */                                                                                                                                 \
    OPCODE(SHL, 0, false, SHL)                                                                                                                       \
    /* shift right */                                                                                                                                \
    OPCODE(SHR, 0, false, SHR)                                                                                                                       \
    /* unsigned shift right */                                                                                                                       \
    OPCODE(USHR, 0, false, USHR)                                                                                                                     \
    /* rotate bits left */                                                                                                                           \
    OPCODE(ROL, 0, false, ROL)                                                                                                                       \
    /* rotate bits right */                                                                                                                          \
    OPCODE(ROR, 0, false, ROR)                                                                                                                       \
    /* bitwise and */                                                                                                                                \
    OPCODE(AND, 0, false, AND)                                                                                                                       \
    /* bitwise or */                                                                                                                                 \
    OPCODE(OR, 0, false, OR)                                                                                                                         \
    /* bitwise xor */                                                                                                                                \
    OPCODE(XOR, 0, false, XOR)                                                                                                                       \
    /* less than */                                                                                                                                  \
    OPCODE(LT, 0, false, LT)                                                                                                                         \
    /* less than or equal */                                                                                                                         \
    OPCODE(LE, 0, false, LE)                                                                                                                         \
    /* equal */                                                                                                                                      \
    OPCODE(EQ, 0, false, EQ)                                                                                                                         \
    /* not equal */                                                                                                                                  \
    OPCODE(NE, 0, false, NE)                                                                                                                         \
    /* greater than or equal */                                                                                                                      \
    OPCODE(GE, 0, false, GE)                                                                                                                         \
    /* greater than */                                                                                                                               \
    OPCODE(GT, 0, false, GT)                                                                                                                         \
    /* is */                                                                                                                                         \
    OPCODE(IS, 0, false, IS)                                                                                                                         \
    /* is not */                                                                                                                                     \
    OPCODE(NIS, 0, false, NIS)                                                                                                                       \
    /* is null */                                                                                                                                    \
    OPCODE(ISNULL, 0, false, ISNULL)                                                                                                                 \
    /* is not null */                                                                                                                                \
    OPCODE(NISNULL, 0, false, NISNULL)                                                                                                               \
    /* ----------------------------------------------------- */                                                                                      \
    /* cast op */                                                                                                                                    \
    /* ----------------------------------------------------- */                                                                                      \
    /* int to uint */                                                                                                                                \
    OPCODE(I2U, 0, false, I2F)                                                                                                                       \
    /* uint to int */                                                                                                                                \
    OPCODE(U2I, 0, false, F2I)                                                                                                                       \
    /* uint to float */                                                                                                                                \
    OPCODE(U2F, 0, false, F2I)                                                                                                                       \
    /* int to float */                                                                                                                               \
    OPCODE(I2F, 0, false, I2F)                                                                                                                       \
    /* float to int */                                                                                                                               \
    OPCODE(F2I, 0, false, F2I)                                                                                                                       \
    /* int to bool */                                                                                                                                \
    OPCODE(I2B, 0, false, I2B)                                                                                                                       \
    /* bool to int */                                                                                                                                \
    OPCODE(B2I, 0, false, B2I)                                                                                                                       \
    /* object to bool (truth value of the object) */                                                                                                 \
    OPCODE(O2B, 0, false, O2B)                                                                                                                       \
    /* object to string (vm specific string representation) */                                                                                       \
    OPCODE(O2S, 0, false, O2S)                                                                                                                       \
    /* ----------------------------------------------------- */                                                                                      \
    /* thread safety op */                                                                                                                           \
    /* ----------------------------------------------------- */                                                                                      \
    /* enter monitor */                                                                                                                              \
    OPCODE(ENTERMONITOR, 0, false, ENTERMONITOR)                                                                                                     \
    /* exit monitor */                                                                                                                               \
    OPCODE(EXITMONITOR, 0, false, EXITMONITOR)                                                                                                       \
    /* ----------------------------------------------------- */                                                                                      \
    /* miscellaneous op */                                                                                                                           \
    /* ----------------------------------------------------- */                                                                                      \
    /* perform match */                                                                                                                              \
    OPCODE(MTPERF, 2, false, MTFPERF)                                                                                                                \
    /* perform match fast */                                                                                                                         \
    OPCODE(MTFPERF, 1, false, MTPERF)                                                                                                                \
    /* load closure */                                                                                                                               \
    OPCODE(CLOSURELOAD, -1, false, CLOSURELOAD)                                                                                                      \
    /* load object */                                                                                                                                \
    OPCODE(OBJLOAD, 0, false, OBJLOAD)                                                                                                               \
    /* throw */                                                                                                                                      \
    OPCODE(THROW, 0, false, THROW)                                                                                                                   \
    /* ret */                                                                                                                                        \
    OPCODE(RET, 0, false, RET)                                                                                                                       \
    /* return void */                                                                                                                                \
    OPCODE(VRET, 0, false, VRET)                                                                                                                     \
    /* ----------------------------------------------------- */                                                                                      \
    /* debug op */                                                                                                                                   \
    /* ----------------------------------------------------- */                                                                                      \
    /* print to console output */                                                                                                                    \
    OPCODE(PRINTLN, 0, false, PRINTLN)

namespace spade
{
    enum class Opcode : uint8_t {
#define OPCODE(name, ...) name,
        LIST_OF_OPCODES
#undef OPCODE
    };

    class OpcodeInfo {
      private:
        constexpr static char to_lower_char(const char c) {
            return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
        }

        template<size_t N>
        class ConstString {
          private:
            const char s[N];

          public:
            template<size_t... Indices>
            constexpr ConstString(const char (&str)[N], std::integer_sequence<size_t, Indices...>) : s{to_lower_char(str[Indices])...} {}

            constexpr ~ConstString() = default;

            constexpr char operator[](size_t i) const {
                return s[i];
            }

            operator const char *() const {
                return s;
            }
        };

        template<size_t N>
        constexpr static ConstString<N> to_lower(const char (&str)[N]) {
            return {str, std::make_integer_sequence<size_t, N>()};
        }

      public:
        static constinit const size_t OPCODE_COUNT =
#define OPCODE(...) 1 +
                                              LIST_OF_OPCODES 0,
                                      NOP;
#undef OPCODE

        static std::string to_string(Opcode opcode) {
            switch (opcode) {
#define OPCODE(name, ...)                                                                                                                            \
    case Opcode::name:                                                                                                                               \
        return std::string(static_cast<const char *>(to_lower(#name)));
                LIST_OF_OPCODES
            default:
                throw Unreachable();
#undef OPCODE
            }
        }

        static constexpr uint8_t params_count(Opcode opcode) {
            switch (opcode) {
#define OPCODE(name, params, ...)                                                                                                                    \
    case Opcode::name:                                                                                                                               \
        return params;
                LIST_OF_OPCODES
            default:
                throw Unreachable();
#undef OPCODE
            }
        }

        static constexpr bool take_from_const_pool(Opcode opcode) {
            switch (opcode) {
#define OPCODE(name, params, take, ...)                                                                                                              \
    case Opcode::name:                                                                                                                               \
        return take;
                LIST_OF_OPCODES
            default:
                throw Unreachable();
#undef OPCODE
            }
        }

        static constexpr Opcode alternate(Opcode opcode) {
            switch (opcode) {
#define OPCODE(name, params, take, alternate)                                                                                                        \
    case Opcode::name:                                                                                                                               \
        return Opcode::alternate;
                LIST_OF_OPCODES
            default:
                throw Unreachable();
#undef OPCODE
            }
        }

        static std::optional<Opcode> from_string(const std::string &str) {
            static const std::unordered_map<std::string, Opcode> names{
#define OPCODE(name, ...) {static_cast<const char *>(to_lower(#name)), Opcode::name},
                    LIST_OF_OPCODES
#undef OPCODE
            };
            auto itr = names.find(str);
            return itr != names.end() ? std::make_optional<Opcode>(itr->second) : std::nullopt;
        }

        static constexpr std::array<Opcode, OPCODE_COUNT> all_opcodes() {
            return std::array<Opcode, OPCODE_COUNT>{
#define OPCODE(name, ...) Opcode::name,
                    LIST_OF_OPCODES
#undef OPCODE
            };
        }
    };
}    // namespace spade
