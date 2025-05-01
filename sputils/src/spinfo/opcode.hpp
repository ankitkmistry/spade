#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <utility>
#include "spimp/error.hpp"

#define LIST_OF_OPCODES                                                                                                                              \
    /* no operation */                                                                                                                               \
    OPCODE(NOP, 0, false)                                                                                                                            \
    /* ----------------------------------------------------- */                                                                                      \
    /* stack op */                                                                                                                                   \
    /* ----------------------------------------------------- */                                                                                      \
    /* load constant */                                                                                                                              \
    OPCODE(CONST, 1, true)                                                                                                                           \
    /* load long constant */                                                                                                                         \
    OPCODE(CONSTL, 2, true)                                                                                                                          \
    /* pop */                                                                                                                                        \
    OPCODE(POP, 0, false)                                                                                                                            \
    /* pop n items from the top of stack */                                                                                                          \
    OPCODE(NPOP, 1, false)                                                                                                                           \
    /* duplicate top of stack */                                                                                                                     \
    OPCODE(DUP, 0, false)                                                                                                                            \
    /* dup top of tack n times */                                                                                                                    \
    OPCODE(NDUP, 1, false)                                                                                                                           \
    /* ----------------------------------------------------- */                                                                                      \
    /* load store op */                                                                                                                              \
    /* ----------------------------------------------------- */                                                                                      \
    /* load global */                                                                                                                                \
    OPCODE(GLOAD, 2, true)                                                                                                                           \
    /* load global fast */                                                                                                                           \
    OPCODE(GFLOAD, 1, true)                                                                                                                          \
    /* store global */                                                                                                                               \
    OPCODE(GSTORE, 2, true)                                                                                                                          \
    /* store global fast */                                                                                                                          \
    OPCODE(GFSTORE, 1, true)                                                                                                                         \
    /* pop store global */                                                                                                                           \
    OPCODE(PGSTORE, 2, true)                                                                                                                         \
    /* pop store global fast */                                                                                                                      \
    OPCODE(PGFSTORE, 1, true)                                                                                                                        \
    /* ----------------------------------------------------- */                                                                                      \
    /* load local */                                                                                                                                 \
    OPCODE(LLOAD, 2, false)                                                                                                                          \
    /* load local fast */                                                                                                                            \
    OPCODE(LFLOAD, 1, false)                                                                                                                         \
    /* store local */                                                                                                                                \
    OPCODE(LSTORE, 2, false)                                                                                                                         \
    /* store local fast */                                                                                                                           \
    OPCODE(LFSTORE, 1, false)                                                                                                                        \
    /* pop store local */                                                                                                                            \
    OPCODE(PLSTORE, 2, false)                                                                                                                        \
    /* pop store local fast */                                                                                                                       \
    OPCODE(PLFSTORE, 1, false)                                                                                                                       \
    /* ----------------------------------------------------- */                                                                                      \
    /* load arg */                                                                                                                                   \
    OPCODE(ALOAD, 1, false)                                                                                                                          \
    /* store arg */                                                                                                                                  \
    OPCODE(ASTORE, 1, false)                                                                                                                         \
    /* pop store arg */                                                                                                                              \
    OPCODE(PASTORE, 1, false)                                                                                                                        \
    /* ----------------------------------------------------- */                                                                                      \
    /* load typearg */                                                                                                                               \
    OPCODE(TLOAD, 2, true)                                                                                                                           \
    /* load typearg fast */                                                                                                                          \
    OPCODE(TFLOAD, 1, true)                                                                                                                          \
    /* store typearg */                                                                                                                              \
    OPCODE(TSTORE, 2, true)                                                                                                                          \
    /* store typearg fast */                                                                                                                         \
    OPCODE(TFSTORE, 1, true)                                                                                                                         \
    /* pop store typearg */                                                                                                                          \
    OPCODE(PTSTORE, 2, true)                                                                                                                         \
    /* pop store typearg fast */                                                                                                                     \
    OPCODE(PTFSTORE, 1, true)                                                                                                                        \
    /* ----------------------------------------------------- */                                                                                      \
    /* load member */                                                                                                                                \
    OPCODE(MLOAD, 2, true)                                                                                                                           \
    /* load member fast */                                                                                                                           \
    OPCODE(MFLOAD, 1, true)                                                                                                                          \
    /* store member */                                                                                                                               \
    OPCODE(MSTORE, 2, true)                                                                                                                          \
    /* store member fast */                                                                                                                          \
    OPCODE(MFSTORE, 1, true)                                                                                                                         \
    /* pop store member */                                                                                                                           \
    OPCODE(PMSTORE, 2, true)                                                                                                                         \
    /* pop store member fast */                                                                                                                      \
    OPCODE(PMFSTORE, 1, true)                                                                                                                        \
    /* ----------------------------------------------------- */                                                                                      \
    /* load static */                                                                                                                                \
    OPCODE(SLOAD, 2, true)                                                                                                                           \
    /* load static fast */                                                                                                                           \
    OPCODE(SFLOAD, 1, true)                                                                                                                          \
    /* store static */                                                                                                                               \
    OPCODE(SSTORE, 2, true)                                                                                                                          \
    /* store static fast */                                                                                                                          \
    OPCODE(SFSTORE, 1, true)                                                                                                                         \
    /* pop store static */                                                                                                                           \
    OPCODE(PSSTORE, 2, true)                                                                                                                         \
    /* pop store static fast */                                                                                                                      \
    OPCODE(PSFSTORE, 1, true)                                                                                                                        \
    /* ----------------------------------------------------- */                                                                                      \
    /* super class method load */                                                                                                                    \
    OPCODE(SPLOAD, 2, true)                                                                                                                          \
    /* super class method load fast */                                                                                                               \
    OPCODE(SPFLOAD, 1, true)                                                                                                                         \
    /* load lambda */                                                                                                                                \
    OPCODE(BLOAD, 2, false)                                                                                                                          \
    /* load lambda fast */                                                                                                                           \
    OPCODE(BFLOAD, 1, false)                                                                                                                         \
    /* ----------------------------------------------------- */                                                                                      \
    /* array op */                                                                                                                                   \
    /* ----------------------------------------------------- */                                                                                      \
    /* pack array */                                                                                                                                 \
    OPCODE(ARRPACK, 0, false)                                                                                                                        \
    /* unpack array */                                                                                                                               \
    OPCODE(ARRUNPACK, 0, false)                                                                                                                      \
    /* build array */                                                                                                                                \
    OPCODE(ARRBUILD, 2, false)                                                                                                                       \
    /* build array fast */                                                                                                                           \
    OPCODE(ARRFBUILD, 1, false)                                                                                                                      \
    /* load array index */                                                                                                                           \
    OPCODE(ILOAD, 0, false)                                                                                                                          \
    /* store array index */                                                                                                                          \
    OPCODE(ISTORE, 0, false)                                                                                                                         \
    /* pop store array index */                                                                                                                      \
    OPCODE(PISTORE, 0, false)                                                                                                                        \
    /* array length */                                                                                                                               \
    OPCODE(ARRLEN, 0, false)                                                                                                                         \
    /* ----------------------------------------------------- */                                                                                      \
    /* call op */                                                                                                                                    \
    /* ----------------------------------------------------- */                                                                                      \
    /* invoke */                                                                                                                                     \
    OPCODE(INVOKE, 1, false)                                                                                                                         \
    /* invoke virtual */                                                                                                                             \
    OPCODE(VINVOKE, 2, true)                                                                                                                         \
    /* invoke static */                                                                                                                              \
    OPCODE(SINVOKE, 2, true)                                                                                                                         \
    /* invoke super class method */                                                                                                                  \
    OPCODE(SPINVOKE, 2, true)                                                                                                                        \
    /* invoke local */                                                                                                                               \
    OPCODE(LINVOKE, 2, false)                                                                                                                        \
    /* invoke global */                                                                                                                              \
    OPCODE(GINVOKE, 2, true)                                                                                                                         \
    /* invoke arg */                                                                                                                                 \
    OPCODE(AINVOKE, 1, false)                                                                                                                        \
    /* invoke virtual fast */                                                                                                                        \
    OPCODE(VFINVOKE, 1, true)                                                                                                                        \
    /* invoke static fast */                                                                                                                         \
    OPCODE(SFINVOKE, 1, true)                                                                                                                        \
    /* invoke super class method fast */                                                                                                             \
    OPCODE(SPFINVOKE, 1, true)                                                                                                                       \
    /* invoke local fast */                                                                                                                          \
    OPCODE(LFINVOKE, 1, false)                                                                                                                       \
    /* invoke global fast */                                                                                                                         \
    OPCODE(GFINVOKE, 1, true)                                                                                                                        \
    /* sub call */                                                                                                                                   \
    OPCODE(CALLSUB, 0, false)                                                                                                                        \
    /* sub return */                                                                                                                                 \
    OPCODE(RETSUB, 0, false)                                                                                                                         \
    /* ----------------------------------------------------- */                                                                                      \
    /* jump op */                                                                                                                                    \
    /* ----------------------------------------------------- */                                                                                      \
    /* jump forward */                                                                                                                               \
    OPCODE(JFW, 2, false)                                                                                                                            \
    /* jump backward */                                                                                                                              \
    OPCODE(JBW, 2, false)                                                                                                                            \
    /* pop jump if true */                                                                                                                           \
    OPCODE(JT, 2, false)                                                                                                                             \
    /* pop jump if false */                                                                                                                          \
    OPCODE(JF, 2, false)                                                                                                                             \
    /* pop jump if less than */                                                                                                                      \
    OPCODE(JLT, 2, false)                                                                                                                            \
    /* pop jump if less than or equal */                                                                                                             \
    OPCODE(JLE, 2, false)                                                                                                                            \
    /* pop jump if equal */                                                                                                                          \
    OPCODE(JEQ, 2, false)                                                                                                                            \
    /* pop jump if not equal */                                                                                                                      \
    OPCODE(JNE, 2, false)                                                                                                                            \
    /* pop jump if greater than or equal */                                                                                                          \
    OPCODE(JGE, 2, false)                                                                                                                            \
    /* pop jump if greater than */                                                                                                                   \
    OPCODE(JGT, 2, false)                                                                                                                            \
    /* ----------------------------------------------------- */                                                                                      \
    /* primitive op */                                                                                                                               \
    /* ----------------------------------------------------- */                                                                                      \
    /* not */                                                                                                                                        \
    OPCODE(NOT, 0, false)                                                                                                                            \
    /* invert */                                                                                                                                     \
    OPCODE(INV, 0, false)                                                                                                                            \
    /* negate */                                                                                                                                     \
    OPCODE(NEG, 0, false)                                                                                                                            \
    /* get type */                                                                                                                                   \
    OPCODE(GETTYPE, 0, false)                                                                                                                        \
    /* safe cast */                                                                                                                                  \
    OPCODE(SCAST, 0, false)                                                                                                                          \
    /* checked cast */                                                                                                                               \
    OPCODE(CCAST, 0, false)                                                                                                                          \
    /* power */                                                                                                                                      \
    OPCODE(POW, 0, false)                                                                                                                            \
    /* multiply */                                                                                                                                   \
    OPCODE(MUL, 0, false)                                                                                                                            \
    /* division */                                                                                                                                   \
    OPCODE(DIV, 0, false)                                                                                                                            \
    /* remainder */                                                                                                                                  \
    OPCODE(REM, 0, false)                                                                                                                            \
    /* addition */                                                                                                                                   \
    OPCODE(ADD, 0, false)                                                                                                                            \
    /* subtraction */                                                                                                                                \
    OPCODE(SUB, 0, false)                                                                                                                            \
    /* shift left */                                                                                                                                 \
    OPCODE(SHL, 0, false)                                                                                                                            \
    /* shift right */                                                                                                                                \
    OPCODE(SHR, 0, false)                                                                                                                            \
    /* unsigned shift right */                                                                                                                       \
    OPCODE(USHR, 0, false)                                                                                                                           \
    /* bitwise and */                                                                                                                                \
    OPCODE(AND, 0, false)                                                                                                                            \
    /* bitwise or */                                                                                                                                 \
    OPCODE(OR, 0, false)                                                                                                                             \
    /* bitwise xor */                                                                                                                                \
    OPCODE(XOR, 0, false)                                                                                                                            \
    /* less than */                                                                                                                                  \
    OPCODE(LT, 0, false)                                                                                                                             \
    /* less than or equal */                                                                                                                         \
    OPCODE(LE, 0, false)                                                                                                                             \
    /* equal */                                                                                                                                      \
    OPCODE(EQ, 0, false)                                                                                                                             \
    /* not equal */                                                                                                                                  \
    OPCODE(NE, 0, false)                                                                                                                             \
    /* greater than or equal */                                                                                                                      \
    OPCODE(GE, 0, false)                                                                                                                             \
    /* greater than */                                                                                                                               \
    OPCODE(GT, 0, false)                                                                                                                             \
    /* is */                                                                                                                                         \
    OPCODE(IS, 0, false)                                                                                                                             \
    /* is not */                                                                                                                                     \
    OPCODE(NIS, 0, false)                                                                                                                            \
    /* is null */                                                                                                                                    \
    OPCODE(ISNULL, 0, false)                                                                                                                         \
    /* is not null */                                                                                                                                \
    OPCODE(NISNULL, 0, false)                                                                                                                        \
    /* ----------------------------------------------------- */                                                                                      \
    /* cast op */                                                                                                                                    \
    /* ----------------------------------------------------- */                                                                                      \
    /* int to float */                                                                                                                               \
    OPCODE(I2F, 0, false)                                                                                                                            \
    /* float to int */                                                                                                                               \
    OPCODE(F2I, 0, false)                                                                                                                            \
    /* int to bool */                                                                                                                                \
    OPCODE(I2B, 0, false)                                                                                                                            \
    /* bool to int */                                                                                                                                \
    OPCODE(B2I, 0, false)                                                                                                                            \
    /* object to bool (truth value of the object) */                                                                                                 \
    OPCODE(O2B, 0, false)                                                                                                                            \
    /* object to string (vm specific string representation) */                                                                                       \
    OPCODE(O2S, 0, false)                                                                                                                            \
    /* ----------------------------------------------------- */                                                                                      \
    /* thread safety op */                                                                                                                           \
    /* ----------------------------------------------------- */                                                                                      \
    /* enter monitor */                                                                                                                              \
    OPCODE(ENTERMONITOR, 0, false)                                                                                                                   \
    /* exit monitor */                                                                                                                               \
    OPCODE(EXITMONITOR, 0, false)                                                                                                                    \
    /* ----------------------------------------------------- */                                                                                      \
    /* miscellaneous op */                                                                                                                           \
    /* ----------------------------------------------------- */                                                                                      \
    /* perform match */                                                                                                                              \
    OPCODE(MTPERF, 2, false)                                                                                                                         \
    /* perform match fast */                                                                                                                         \
    OPCODE(MTFPERF, 1, false)                                                                                                                        \
    /* load closure */                                                                                                                               \
    OPCODE(CLOSURELOAD, -1, false)                                                                                                                   \
    /* load reified object */                                                                                                                        \
    OPCODE(REIFIEDLOAD, 1, false)                                                                                                                    \
    /* load object */                                                                                                                                \
    OPCODE(OBJLOAD, 0, false)                                                                                                                        \
    /* throw */                                                                                                                                      \
    OPCODE(THROW, 0, false)                                                                                                                          \
    /* ret */                                                                                                                                        \
    OPCODE(RET, 0, false)                                                                                                                            \
    /* return void */                                                                                                                                \
    OPCODE(VRET, 0, false)                                                                                                                           \
    /* ----------------------------------------------------- */                                                                                      \
    /* debug op */                                                                                                                                   \
    /* ----------------------------------------------------- */                                                                                      \
    /* print to console output */                                                                                                                    \
    OPCODE(PRINTLN, 0, false)

namespace spade
{
    enum class Opcode {
#define OPCODE(name, params, take) name,
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
        static constexpr const size_t OPCODE_COUNT =
#define OPCODE(...) 1 +
                LIST_OF_OPCODES 0;
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

        static constexpr uint8_t get_params(Opcode opcode) {
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
#define OPCODE(name, params, take)                                                                                                                   \
    case Opcode::name:                                                                                                                               \
        return take;
                LIST_OF_OPCODES
                default:
                    throw Unreachable();
#undef OPCODE
            }
        }

        static Opcode from_string(const std::string &str) {
            static const std::unordered_map<std::string, Opcode> names{
#define OPCODE(name, ...) {static_cast<const char *>(to_lower(#name)), Opcode::name},
                    LIST_OF_OPCODES
#undef OPCODE
            };
            auto itr = names.find(str);
            return itr != names.end() ? itr->second : Opcode::NOP;
        }

        static std::array<Opcode, OPCODE_COUNT> all_opcodes() {
            return std::array<Opcode, OPCODE_COUNT>{
#define OPCODE(name, ...) Opcode::name,
                    LIST_OF_OPCODES
#undef OPCODE
            };
        }
    };
}    // namespace spade