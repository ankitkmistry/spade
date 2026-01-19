#pragma once

#include "elpops/elpdef.hpp"
#include "spinfo/opcode.hpp"
#include "utils/common.hpp"
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <unordered_map>
#include <variant>

namespace spadec
{
    /*
     *   raw             = 0x 00000000 00000000
     *                        |      | |      |
     *                        +------+ +------+
     *                           |         |
     *   accessor        |-------+         |
     *   modifier        |-----------------+
     *
     *   modifier        = 0x  0  0  0  0  0  0  0  0
     *   =================                 |  |  |  |
     *   override        |-----------------+  |  |  |
     *   final           |--------------------+  |  |
     *   abstract        |-----------------------+  |
     *   static          |--------------------------+
     *
     *   accessor        = 0x  0  0  0  0  0  0  0  0
     *   =================              |  |  |  |  |
     *   public          |--------------+  |  |  |  |
     *   protected       |-----------------+  |  |  |
     *   package-private |--------------------+  |  |
     *   internal        |-----------------------+  |
     *   private         |--------------------------+
     */
    struct Flags {
        uint16_t raw;

#define STATIC_MASK         (0b0000'0000'0000'0001)
#define ABSTRACT_MASK       (0b0000'0000'0000'0010)
#define FINAL_MASK          (0b0000'0000'0000'0100)
#define OVERRIDE_MASK       (0b0000'0000'0000'1000)
#define PRIVATE_MASK        (0b0000'0001'0000'0000)
#define INTERNAL_MASK       (0b0000'0010'0000'0000)
#define MODULE_PRIVATE_MASK (0b0000'0100'0000'0000)
#define PROTECTED_MASK      (0b0000'1000'0000'0000)
#define PUBLIC_MASK         (0b0001'0000'0000'0000)

        constexpr Flags(uint16_t raw = 0) : raw(raw) {}

        constexpr Flags(const Flags &) = default;
        constexpr Flags(Flags &&) = default;
        constexpr Flags &operator=(const Flags &) = default;
        constexpr Flags &operator=(Flags &&) = default;
        constexpr ~Flags() = default;

        constexpr uint16_t get_raw() const {
            return raw;
        }

        constexpr Flags &set_static(bool b = true) {
            raw = b ? raw | STATIC_MASK : raw & ~STATIC_MASK;
            return *this;
        }

        constexpr bool is_static() const {
            return raw & STATIC_MASK;
        }

        constexpr Flags &set_abstract(bool b = true) {
            raw = b ? raw | ABSTRACT_MASK : raw & ~ABSTRACT_MASK;
            return *this;
        }

        constexpr bool is_abstract() const {
            return raw & ABSTRACT_MASK;
        }

        constexpr Flags &set_final(bool b = true) {
            raw = b ? raw | FINAL_MASK : raw & ~FINAL_MASK;
            return *this;
        }

        constexpr bool is_final() const {
            return raw & FINAL_MASK;
        }

        constexpr Flags &set_override(bool b = true) {
            raw = b ? raw | OVERRIDE_MASK : raw & ~OVERRIDE_MASK;
            return *this;
        }

        constexpr bool is_override() const {
            return raw & OVERRIDE_MASK;
        }

        constexpr Flags &set_private(bool b = true) {
            raw = b ? raw | PRIVATE_MASK : raw & ~PRIVATE_MASK;
            return *this;
        }

        constexpr bool is_private() const {
            return raw & PRIVATE_MASK;
        }

        constexpr Flags &set_internal(bool b = true) {
            raw = b ? raw | INTERNAL_MASK : raw & ~INTERNAL_MASK;
            return *this;
        }

        constexpr bool is_internal() const {
            return raw & INTERNAL_MASK;
        }

        constexpr Flags &set_module_private(bool b = true) {
            raw = b ? raw | MODULE_PRIVATE_MASK : raw & ~MODULE_PRIVATE_MASK;
            return *this;
        }

        constexpr bool is_module_private() const {
            return raw & MODULE_PRIVATE_MASK;
        }

        constexpr Flags &set_protected(bool b = true) {
            raw = b ? raw | PROTECTED_MASK : raw & ~PROTECTED_MASK;
            return *this;
        }

        constexpr bool is_protected() const {
            return raw & PROTECTED_MASK;
        }

        constexpr Flags &set_public(bool b = true) {
            raw = b ? raw | PUBLIC_MASK : raw & ~PUBLIC_MASK;
            return *this;
        }

        constexpr bool is_public() const {
            return raw & PUBLIC_MASK;
        }

#undef STATIC_MASK
#undef ABSTRACT_MASK
#undef FINAL_MASK
#undef OVERRIDE_MASK
#undef PRIVATE_MASK
#undef INTERNAL_MASK
#undef MODULE_PRIVATE_MASK
#undef PROTECTED_MASK
#undef PUBLIC_MASK
    };

    class ModuleEmitter;

    ///
    /// BCODE       -> corresponding bytecode in hexadecimal
    /// LINFO       -> line number is registered for each and every byte in the bytecode
    /// RLE-LINFO   -> The raw line info is then compressed by run length encoding
    ///
    /// --------------------+-------+-------------------
    /// Readable bytecode   | BCODE | LINFO => RLE-LINFO
    /// --------------------+-------+-------------------
    ///   const       1     | 04 00 | 01 01 => 2 x 01
    ///   plfstore    0     | 15 00 | 02 02 => 2 x 02
    ///   lfload      1     | 11 01 | 03 03 => 2 x 03
    ///   lfload      0     | 11 00 | 04 04 => 2 x 04
    ///   add               | 49    | 05    => 1 x 05
    ///   dup               | 08    | 06    => 1 x 06
    ///   plfstore    0     | 15 00 | 07 07 => 2 x 07
    ///   plfstore    1     | 15 01 | 08 08 => 2 x 05
    /// --------------------+-------+-------------------
    ///
    /// l0 = 1
    /// l0 = l1 = l1 + l0
    ///
    /// Each general emit function is of the form:
    ///     emit_OP(params, line)
    ///

    class Label {
        string name;
        size_t pos;

      public:
        Label(const string &name) : name(name), pos(0) {}

        bool operator==(const Label &other) const {
            return name == other.name;
        }

        bool operator!=(const Label &other) const {
            return name != other.name;
        }

        string get_name() const {
            return name;
        }

        size_t get_pos() const {
            return pos;
        }

        void set_pos(size_t pos) {
            this->pos = pos;
        }
    };
}    // namespace spadec

template<>
class std::hash<spadec::Label> {
  public:
    size_t operator()(const spadec::Label &label) const {
        return std::hash<std::string>()(label.get_name());
    }
};

namespace spadec
{
    class CodeEmitter {
        static constexpr const auto uint8_max = std::numeric_limits<uint8_t>::max();
        static constexpr const auto uint16_max = std::numeric_limits<uint16_t>::max();

        ModuleEmitter *module;
        vector<uint8_t> code;
        vector<uint32_t> lines;
        std::unordered_map<Label, vector<size_t>> patches;
        size_t label_counter = 0;

      public:
        Label new_label() {
            Label label(std::format("$label{}", label_counter));
            label_counter++;
            patches.emplace(label, vector<size_t>());
            return label;
        }

        void bind_label(Label &label) {
            label.set_pos(code.size());
        }

        void emit(MethodInfo &info);

        // Stack ops
        void emit_nop(uint32_t line) {
            emit_opcode(Opcode::NOP, line);
        }

        void emit_const(const CpInfo &cp, uint32_t line);

        void emit_pop(uint32_t line) {
            emit_opcode(Opcode::POP, line);
        }

        void emit_npop(uint8_t times, uint32_t line) {
            emit_opcode(Opcode::NPOP, line);
            emit_byte(times, line);
        }

        void emit_dup(uint32_t line) {
            emit_opcode(Opcode::DUP, line);
        }

        void emit_ndup(uint8_t times, uint32_t line) {
            emit_opcode(Opcode::NDUP, line);
            emit_byte(times, line);
        }

        // Global ops
        void emit_gload(const string &sign, uint32_t line) {
            emit_inst(Opcode::GLOAD, sign, line);
        }

        void emit_gstore(const string &sign, uint32_t line) {
            emit_inst(Opcode::GSTORE, sign, line);
        }

        void emit_pgstore(const string &sign, uint32_t line) {
            emit_inst(Opcode::PGSTORE, sign, line);
        }

        // Local ops
        void emit_lload(uint16_t index, uint32_t line) {
            emit_inst(Opcode::LLOAD, index, line);
        }

        void emit_lstore(uint16_t index, uint32_t line) {
            emit_inst(Opcode::LSTORE, index, line);
        }

        void emit_plstore(uint16_t index, uint32_t line) {
            emit_inst(Opcode::PLSTORE, index, line);
        }

        // Arg ops
        void emit_aload(uint8_t index, uint32_t line) {
            emit_opcode(Opcode::ALOAD, line);
            emit_byte(index, line);
        }

        void emit_astore(uint8_t index, uint32_t line) {
            emit_opcode(Opcode::ASTORE, line);
            emit_byte(index, line);
        }

        void emit_pastore(uint8_t index, uint32_t line) {
            emit_opcode(Opcode::PASTORE, line);
            emit_byte(index, line);
        }

        // Member ops
        void emit_mload(const string &name, uint32_t line) {
            emit_inst(Opcode::MLOAD, name, line);
        }

        void emit_mstore(const string &name, uint32_t line) {
            emit_inst(Opcode::MSTORE, name, line);
        }

        void emit_pmstore(const string &name, uint32_t line) {
            emit_inst(Opcode::PMSTORE, name, line);
        }

        // Superclass ops
        void emit_spload(const string &sign, uint32_t line) {
            emit_inst(Opcode::SPLOAD, sign, line);
        }

        // Array ops
        void emit_arrpack(uint8_t count, uint32_t line) {
            emit_opcode(Opcode::ARRUNPACK, line);
            emit_byte(count, line);
        }

        void emit_arrunpack(uint32_t line) {
            emit_opcode(Opcode::ARRUNPACK, line);
        }

        void emit_arrbuild(uint16_t size, uint32_t line) {
            emit_inst(Opcode::ARRBUILD, size, line);
        }

        void emit_iload(uint32_t line) {
            emit_opcode(Opcode::ILOAD, line);
        }

        void emit_istore(uint32_t line) {
            emit_opcode(Opcode::ISTORE, line);
        }

        void emit_pistore(uint32_t line) {
            emit_opcode(Opcode::PISTORE, line);
        }

        void emit_arrlen(uint32_t line) {
            emit_opcode(Opcode::ARRLEN, line);
        }

        // Invoke ops
        void emit_invoke(uint8_t arg_count, uint32_t line) {
            emit_opcode(Opcode::INVOKE, line);
            emit_byte(arg_count, line);
        }

        void emit_vinvoke(const string &sign, uint32_t line) {
            emit_inst(Opcode::VINVOKE, sign, line);
        }

        void emit_spinvoke(const string &sign, uint32_t line) {
            emit_inst(Opcode::SPINVOKE, sign, line);
        }

        void emit_linvoke(uint16_t index, uint32_t line) {
            emit_inst(Opcode::LINVOKE, index, line);
        }

        void emit_ginvoke(const string &sign, uint32_t line) {
            emit_inst(Opcode::GINVOKE, sign, line);
        }

        void emit_ainvoke(uint8_t index, uint32_t line) {
            emit_opcode(Opcode::AINVOKE, line);
            emit_byte(index, line);
        }

        void emit_callsub(const Label &dest, uint32_t line) {
            emit_opcode(Opcode::CALLSUB, line);
            emit_label(dest, line);
        }

        void emit_retsub(uint32_t line) {
            emit_opcode(Opcode::RETSUB, line);
        }

        // Jump ops
        void emit_jmp(const Label &dest, uint32_t line) {
            emit_opcode(Opcode::JMP, line);
            emit_label(dest, line);
        }

        void emit_jt(const Label &dest, uint32_t line) {
            emit_opcode(Opcode::JT, line);
            emit_label(dest, line);
        }

        void emit_jf(const Label &dest, uint32_t line) {
            emit_opcode(Opcode::JF, line);
            emit_label(dest, line);
        }

        void emit_jlt(const Label &dest, uint32_t line) {
            emit_opcode(Opcode::JLT, line);
            emit_label(dest, line);
        }

        void emit_jle(const Label &dest, uint32_t line) {
            emit_opcode(Opcode::JLE, line);
            emit_label(dest, line);
        }

        void emit_jeq(const Label &dest, uint32_t line) {
            emit_opcode(Opcode::JEQ, line);
            emit_label(dest, line);
        }

        void emit_jne(const Label &dest, uint32_t line) {
            emit_opcode(Opcode::JNE, line);
            emit_label(dest, line);
        }

        void emit_jge(const Label &dest, uint32_t line) {
            emit_opcode(Opcode::JGE, line);
            emit_label(dest, line);
        }

        void emit_jgt(const Label &dest, uint32_t line) {
            emit_opcode(Opcode::JGT, line);
            emit_label(dest, line);
        }

        // Primitive ops
        void emit_not(uint32_t line) {
            emit_opcode(Opcode::NOT, line);
        }

        void emit_inv(uint32_t line) {
            emit_opcode(Opcode::INV, line);
        }

        void emit_neg(uint32_t line) {
            emit_opcode(Opcode::NEG, line);
        }

        void emit_gettype(uint32_t line) {
            emit_opcode(Opcode::GETTYPE, line);
        }

        void emit_scast(uint32_t line) {
            emit_opcode(Opcode::SCAST, line);
        }

        void emit_ccast(uint32_t line) {
            emit_opcode(Opcode::CCAST, line);
        }

        void emit_concat(uint32_t line) {
            emit_opcode(Opcode::CONCAT, line);
        }

        void emit_pow(uint32_t line) {
            emit_opcode(Opcode::POW, line);
        }

        void emit_mul(uint32_t line) {
            emit_opcode(Opcode::MUL, line);
        }

        void emit_div(uint32_t line) {
            emit_opcode(Opcode::DIV, line);
        }

        void emit_rem(uint32_t line) {
            emit_opcode(Opcode::REM, line);
        }

        void emit_add(uint32_t line) {
            emit_opcode(Opcode::ADD, line);
        }

        void emit_sub(uint32_t line) {
            emit_opcode(Opcode::SUB, line);
        }

        void emit_shl(uint32_t line) {
            emit_opcode(Opcode::SHL, line);
        }

        void emit_shr(uint32_t line) {
            emit_opcode(Opcode::SHR, line);
        }

        void emit_ushr(uint32_t line) {
            emit_opcode(Opcode::USHR, line);
        }

        void emit_rol(uint32_t line) {
            emit_opcode(Opcode::ROL, line);
        }

        void emit_ror(uint32_t line) {
            emit_opcode(Opcode::ROR, line);
        }

        void emit_and(uint32_t line) {
            emit_opcode(Opcode::AND, line);
        }

        void emit_or(uint32_t line) {
            emit_opcode(Opcode::OR, line);
        }

        void emit_xor(uint32_t line) {
            emit_opcode(Opcode::XOR, line);
        }

        void emit_lt(uint32_t line) {
            emit_opcode(Opcode::LT, line);
        }

        void emit_le(uint32_t line) {
            emit_opcode(Opcode::LE, line);
        }

        void emit_eq(uint32_t line) {
            emit_opcode(Opcode::EQ, line);
        }

        void emit_ne(uint32_t line) {
            emit_opcode(Opcode::NE, line);
        }

        void emit_ge(uint32_t line) {
            emit_opcode(Opcode::GE, line);
        }

        void emit_gt(uint32_t line) {
            emit_opcode(Opcode::GT, line);
        }

        void emit_is(uint32_t line) {
            emit_opcode(Opcode::IS, line);
        }

        void emit_nis(uint32_t line) {
            emit_opcode(Opcode::NIS, line);
        }

        void emit_isnull(uint32_t line) {
            emit_opcode(Opcode::ISNULL, line);
        }

        void emit_nisnull(uint32_t line) {
            emit_opcode(Opcode::NISNULL, line);
        }

        // Cast ops
        void emit_i2f(uint32_t line) {
            emit_opcode(Opcode::I2F, line);
        }

        void emit_f2i(uint32_t line) {
            emit_opcode(Opcode::F2I, line);
        }

        void emit_i2b(uint32_t line) {
            emit_opcode(Opcode::I2B, line);
        }

        void emit_b2i(uint32_t line) {
            emit_opcode(Opcode::B2I, line);
        }

        void emit_o2b(uint32_t line) {
            emit_opcode(Opcode::O2B, line);
        }

        void emit_o2s(uint32_t line) {
            emit_opcode(Opcode::O2S, line);
        }

        // Thread safety ops
        void emit_entermonitor(uint32_t line) {
            emit_opcode(Opcode::ENTERMONITOR, line);
        }

        void emit_exitmonitor(uint32_t line) {
            emit_opcode(Opcode::EXITMONITOR, line);
        }

        // Misc. ops
        void emit_mtperf(uint16_t index, uint32_t line) {
            emit_inst(Opcode::MTPERF, index, line);
        }

        void emit_closureload(uint32_t line);    // TODO: implement closures

        void emit_objload(uint32_t line) {
            emit_opcode(Opcode::OBJLOAD, line);
        }

        void emit_throw(uint32_t line) {
            emit_opcode(Opcode::THROW, line);
        }

        void emit_ret(uint32_t line) {
            emit_opcode(Opcode::RET, line);
        }

        void emit_vret(uint32_t line) {
            emit_opcode(Opcode::VRET, line);
        }

        void emit_println(uint32_t line) {
            emit_opcode(Opcode::PRINTLN, line);
        }

      private:
        void emit_inst(Opcode opcode, string param, uint32_t line);
        void emit_inst(Opcode opcode, uint16_t param, uint32_t line);
        void emit_label(const Label &label, uint32_t line);
        void emit_opcode(Opcode opcode, uint32_t line);
        void emit_byte(uint8_t u8, uint32_t line);
        void emit_short(uint16_t u16, uint32_t line);

        void patch_labels();
    };

    class MethodEmitter {
        MethodInfo info;

        ModuleEmitter *module;
        mutable CodeEmitter code_emitter;

      public:
        enum class Kind { FUNCTION, METHOD, CONSTRUCTOR };

        MethodEmitter(ModuleEmitter &module, const string &name, Kind kind, Flags modifiers, uint8_t args_count, uint16_t locals_count);

        MethodInfo emit() const {
            MethodInfo method = info;
            code_emitter.emit(method);
            // TODO: method.stack_max
            // TODO: method.exception_table
            // TODO: method.matches
            // TODO: method.meta
            return method;
        }
    };

    class ClassEmitter {
        ClassInfo info;

        ModuleEmitter *module;
        vector<std::unique_ptr<MethodEmitter>> methods;

      public:
        enum class Kind { CLASS, INTERFACE, ANNOTATION, ENUM };

        ClassEmitter(ModuleEmitter &module, const string &name, Kind kind, Flags modifiers, vector<string> supers);

        void add_field(const string &name, bool is_const, Flags modifiers);

        ClassInfo emit() const {
            ClassInfo klass = info;
            klass.methods_count = methods.size();
            for (const auto &method: methods) {
                klass.methods.push_back(method->emit());
            }
            return klass;
        }
    };

    class ModuleEmitter {
        ModuleInfo info;

        vector<std::unique_ptr<ClassEmitter>> classes;
        vector<CpInfo> conpool;

      public:
        ModuleEmitter(const string &name, bool is_executable, const fs::path &path);

        void add_global(const string &name, bool is_const, Flags modifiers);

        ClassEmitter &new_class(ModuleEmitter &module, const string &name, ClassEmitter::Kind kind, Flags modifiers, vector<string> supers) {
            return *classes.emplace_back(std::make_unique<ClassEmitter>(module, name, kind, modifiers, supers));
        }

        cpidx get_constant(const string &str) {
            return get_constant(CpInfo::from_string(str));
        }

        cpidx get_constant(const vector<CpInfo> &array) {
            return get_constant(CpInfo::from_array(array));
        }

        cpidx get_constant(const CpInfo &cp) {
            for (cpidx i = 0; i < conpool.size(); i++)
                if (cp == conpool[i])
                    return i;

            cpidx index = conpool.size();
            conpool.push_back(cp);
            return index;
        }

        ModuleInfo emit() const {
            ModuleInfo module = info;
            module.classes_count = classes.size();
            for (const auto &klass: classes) {
                module.classes.push_back(klass->emit());
            }
            module.constant_pool_count = conpool.size();
            for (const auto &cp: conpool) {
                module.constant_pool.push_back(cp);
            }
            return module;
        }
    };

    class ElpEmitter {
        ElpInfo info;

        vector<std::unique_ptr<ModuleEmitter>> modules;

      public:
        explicit ElpEmitter(bool is_executable) {
            info.magic = is_executable ? 0xC0FFEDE : 0xDEADCAFE;
            info.major_version = 0;
            info.minor_version = 0;
        }

        void set_entry(const string &entry) {
            info.entry = _UTF8(entry);
        }

        void add_import(const fs::path &path) {
            info.imports_count++;
            info.imports.push_back(_UTF8(path.string()));
        }

        ModuleEmitter &new_module(bool is_executable, const fs::path &path, const string &name) {
            return *modules.emplace_back(std::make_unique<ModuleEmitter>(name, is_executable, path));
        }

        ElpInfo emit() const {
            ElpInfo elp = info;
            elp.modules_count = modules.size();
            for (const auto &module: modules) {
                elp.modules.push_back(module->emit());
            }
            return elp;
        }
    };
}    // namespace spadec
