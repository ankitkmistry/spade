#include <cstddef>
#include <cstring>
#include <memory>

#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"

#include "jit.hpp"
#include "spinfo/opcode.hpp"
#include "utils/common.hpp"
#include "callable/frame_template.hpp"
#include "objects/int.hpp"
#include "objects/float.hpp"
#include "ee/vm.hpp"

namespace spade
{
    class JitCompiler {
        // Code specific
        uint8 *code;
        uint8 *ip;
        uint32 count;
        vector<Obj *> conpool;
        vector<llvm::Value *> stack;

        // LLVM specific
        std::unique_ptr<llvm::LLVMContext> l_context;
        std::unique_ptr<llvm::Module> l_module;
        std::unique_ptr<llvm::IRBuilder<>> l_builder;
        llvm::PointerType *l_pvoid_t;

      public:
        JitCompiler(const uint8 *code, uint32 count, const vector<Obj *> &conpool) {
            this->code = new uint8[count];
            std::memcpy(this->code, code, count);
            this->ip = this->code;
            this->count = count;
            this->conpool = conpool;

            l_context = std::make_unique<llvm::LLVMContext>();
            l_module = std::make_unique<llvm::Module>("spadejit", *l_context);
            l_builder = std::make_unique<llvm::IRBuilder<>>(*l_context);
            l_pvoid_t = llvm::PointerType::get(llvm::IntegerType::get(*l_context, 8), 0);
        }

        JitCompiler(const JitCompiler &other) {
            code = new uint8[other.count];
            std::memcpy(code, other.code, other.count);
            count = other.count;
            ip = code + (other.ip - other.code);
            conpool = other.conpool;
        }

        JitCompiler &operator=(const JitCompiler &other) {
            if (this != &other) {
                delete[] code;
                code = new uint8[other.count];
                std::memcpy(code, other.code, other.count);
                count = other.count;
                ip = code + (other.ip - other.code);
                conpool = other.conpool;
            }
            return *this;
        }

        JitCompiler(JitCompiler &&other) noexcept {
            code = other.code;
            ip = other.ip;
            count = other.count;
            other.code = null;
            other.ip = null;
            other.count = 0;
            conpool = std::move(other.conpool);
        }

        JitCompiler &operator=(JitCompiler &&other) noexcept {
            if (this != &other) {
                delete[] code;
                code = other.code;
                ip = other.ip;
                count = other.count;
                other.code = null;
                other.ip = null;
                other.count = 0;
                conpool = std::move(other.conpool);
            }
            return *this;
        }

        ~JitCompiler() {
            delete[] code;
        }

        void print_llvm() {
            l_module->print(llvm::errs(), null);
        }

        void print_code() {
            ip = code;
            while (!is_at_end()) {
                std::cout << get_pc() << ": ";
                Opcode opcode = static_cast<Opcode>(read_byte());
                uint16 param;
                switch (OpcodeInfo::params_count(opcode)) {
                    case 2:
                        param = read_short();
                        if (OpcodeInfo::take_from_const_pool(opcode))
                            std::cout << OpcodeInfo::to_string(opcode) << " " << param << " (" << conpool[param]->to_string() << ")" << "\n";
                        else
                            std::cout << OpcodeInfo::to_string(opcode) << " " << param << "\n";
                        break;
                    case 1:
                        param = read_byte();
                        if (OpcodeInfo::take_from_const_pool(opcode))
                            std::cout << OpcodeInfo::to_string(opcode) << " " << param << " (" << conpool[param]->to_string() << ")" << "\n";
                        else
                            std::cout << OpcodeInfo::to_string(opcode) << " " << param << "\n";
                        break;
                    case 0:
                        std::cout << OpcodeInfo::to_string(opcode) << "\n";
                        break;
                    default:
                        std::cout << "nop\n";
                        break;
                }
            }
            ip = code;
        }

        void compile() {
            while (!is_at_end()) {
                Opcode opcode = static_cast<Opcode>(read_byte());
                switch (opcode) {
                    case Opcode::NOP:
                        break;
                    case Opcode::CONST_NULL:
                        push(load_null());
                        break;
                    case Opcode::CONST_TRUE:
                        push(load_true());
                        break;
                    case Opcode::CONST_FALSE:
                        push(load_false());
                        break;
                    case Opcode::CONST:
                        push(load_const(read_short()));
                        break;
                    case Opcode::CONSTL:
                        push(load_const(read_byte()));
                        break;
                    case Opcode::POP:
                        pop();
                        break;
                    case Opcode::NPOP:
                        pop_n(read_byte());
                        break;
                    case Opcode::DUP:
                        push(top());
                        break;
                    case Opcode::NDUP:
                        push_n(top(), read_byte());
                        break;
                    case Opcode::GLOAD:
                        break;
                    case Opcode::GFLOAD:
                        break;
                    case Opcode::GSTORE:
                        break;
                    case Opcode::GFSTORE:
                        break;
                    case Opcode::PGSTORE:
                        break;
                    case Opcode::PGFSTORE:
                        break;
                    case Opcode::LLOAD:
                        break;
                    case Opcode::LFLOAD:
                        break;
                    case Opcode::LSTORE:
                        break;
                    case Opcode::LFSTORE:
                        break;
                    case Opcode::PLSTORE:
                        break;
                    case Opcode::PLFSTORE:
                        break;
                    case Opcode::ALOAD:
                        break;
                    case Opcode::ASTORE:
                        break;
                    case Opcode::PASTORE:
                        break;
                    case Opcode::TLOAD:
                        break;
                    case Opcode::TFLOAD:
                        break;
                    case Opcode::TSTORE:
                        break;
                    case Opcode::TFSTORE:
                        break;
                    case Opcode::PTSTORE:
                        break;
                    case Opcode::PTFSTORE:
                        break;
                    case Opcode::MLOAD:
                        break;
                    case Opcode::MFLOAD:
                        break;
                    case Opcode::MSTORE:
                        break;
                    case Opcode::MFSTORE:
                        break;
                    case Opcode::PMSTORE:
                        break;
                    case Opcode::PMFSTORE:
                        break;
                    case Opcode::SPLOAD:
                        break;
                    case Opcode::SPFLOAD:
                        break;
                    case Opcode::ARRPACK:
                        break;
                    case Opcode::ARRUNPACK:
                        break;
                    case Opcode::ARRBUILD:
                        break;
                    case Opcode::ARRFBUILD:
                        break;
                    case Opcode::ILOAD:
                        break;
                    case Opcode::ISTORE:
                        break;
                    case Opcode::PISTORE:
                        break;
                    case Opcode::ARRLEN:
                        break;
                    case Opcode::INVOKE:
                        break;
                    case Opcode::VINVOKE:
                        break;
                    case Opcode::SPINVOKE:
                        break;
                    case Opcode::LINVOKE:
                        break;
                    case Opcode::GINVOKE:
                        break;
                    case Opcode::AINVOKE:
                        break;
                    case Opcode::VFINVOKE:
                        break;
                    case Opcode::SPFINVOKE:
                        break;
                    case Opcode::LFINVOKE:
                        break;
                    case Opcode::GFINVOKE:
                        break;
                    case Opcode::CALLSUB:
                        break;
                    case Opcode::RETSUB:
                        break;
                    case Opcode::JMP:
                        break;
                    case Opcode::JT:
                        break;
                    case Opcode::JF:
                        break;
                    case Opcode::JLT:
                        break;
                    case Opcode::JLE:
                        break;
                    case Opcode::JEQ:
                        break;
                    case Opcode::JNE:
                        break;
                    case Opcode::JGE:
                        break;
                    case Opcode::JGT:
                        break;
                    case Opcode::NOT: {
                        const auto value = pop();
                        if (value->getType()->isIntegerTy(64))
                            l_builder->CreateNot(value, "res_not");
                        break;
                    }
                    case Opcode::INV: {
                        const auto value = pop();
                        if (value->getType()->isIntegerTy(64))
                            l_builder->CreateXor(value, load_int(-1), "res_inv");
                        break;
                    }
                    case Opcode::NEG:
                        break;
                    case Opcode::GETTYPE:
                        break;
                    case Opcode::SCAST:
                        break;
                    case Opcode::CCAST:
                        break;
                    case Opcode::CONCAT:
                        break;
                    case Opcode::POW:
                        break;
                    case Opcode::MUL: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            l_builder->CreateMul(a, b, "res_mul", false, true);
                        else if (a->getType()->isDoubleTy() && b->getType()->isDoubleTy())
                            l_builder->CreateFMul(a, b, "res_mul");
                        break;
                    }
                    case Opcode::DIV: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            l_builder->CreateSDiv(a, b, "res_div", true);
                        else if (a->getType()->isDoubleTy() && b->getType()->isDoubleTy())
                            l_builder->CreateFDiv(a, b, "res_div");
                        break;
                    }
                    case Opcode::REM: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            l_builder->CreateSRem(a, b, "res_mod");
                        break;
                    }
                    case Opcode::ADD: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            l_builder->CreateAdd(a, b, "res_add", false, true);
                        else if (a->getType()->isDoubleTy() && b->getType()->isDoubleTy())
                            l_builder->CreateFAdd(a, b, "res_add");
                        break;
                    }
                    case Opcode::SUB: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            l_builder->CreateSub(a, b, "res_sub", false, true);
                        else if (a->getType()->isDoubleTy() && b->getType()->isDoubleTy())
                            l_builder->CreateFSub(a, b, "res_sub");
                        break;
                    }
                    case Opcode::SHL: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            l_builder->CreateShl(a, b, "res_shl", true, true);
                        break;
                    }
                    case Opcode::SHR: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            l_builder->CreateAShr(a, b, "res_shl");
                        break;
                    }
                    case Opcode::USHR: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            l_builder->CreateLShr(a, b, "res_shl");
                        break;
                    }
                    case Opcode::AND: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            l_builder->CreateAnd(a, b, "res_and");
                        break;
                    }
                    case Opcode::OR: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            l_builder->CreateOr(a, b, "res_or");
                        break;
                    }
                    case Opcode::XOR: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            l_builder->CreateXor(a, b, "res_xor");
                        break;
                    }
                    case Opcode::LT:
                        break;
                    case Opcode::LE:
                        break;
                    case Opcode::EQ:
                        break;
                    case Opcode::NE:
                        break;
                    case Opcode::GE:
                        break;
                    case Opcode::GT:
                        break;
                    case Opcode::IS: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isPointerTy() && b->getType()->isPointerTy()) {
                            const auto address_a = l_builder->CreatePtrToInt(a, llvm::Type::getInt64Ty(*l_context));
                            const auto address_b = l_builder->CreatePtrToInt(b, llvm::Type::getInt64Ty(*l_context));
                            const auto cmp = l_builder->CreateICmp(llvm::CmpInst::Predicate::ICMP_EQ, a, b, "res_is");
                        }
                        break;
                    }
                    case Opcode::NIS: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isPointerTy() && b->getType()->isPointerTy()) {
                            const auto address_a = l_builder->CreatePtrToInt(a, llvm::Type::getInt64Ty(*l_context));
                            const auto address_b = l_builder->CreatePtrToInt(b, llvm::Type::getInt64Ty(*l_context));
                            const auto cmp = l_builder->CreateICmp(llvm::CmpInst::Predicate::ICMP_NE, a, b, "res_nis");
                        }
                        break;
                    }
                    case Opcode::ISNULL: {
                        const auto value = pop();
                        if (value->getType()->isPointerTy()) {
                            const auto address = l_builder->CreatePtrToInt(value, llvm::Type::getInt64Ty(*l_context));
                            const auto cmp = l_builder->CreateICmp(llvm::CmpInst::Predicate::ICMP_EQ, value, load_int(0), "res_isnull");
                        }
                        break;
                    }
                    case Opcode::NISNULL: {
                        const auto value = pop();
                        if (value->getType()->isPointerTy()) {
                            const auto address = l_builder->CreatePtrToInt(value, llvm::Type::getInt64Ty(*l_context));
                            const auto cmp = l_builder->CreateICmp(llvm::CmpInst::Predicate::ICMP_NE, value, load_int(0), "res_nisnull");
                        }
                        break;
                    }
                    case Opcode::I2F:
                        l_builder->CreateSIToFP(pop(), llvm::Type::getDoubleTy(*l_context));
                        break;
                    case Opcode::F2I:
                        l_builder->CreateFPToSI(pop(), llvm::Type::getInt64Ty(*l_context));
                        break;
                    case Opcode::I2B:
                        break;
                    case Opcode::B2I:
                        break;
                    case Opcode::O2B:
                        break;
                    case Opcode::O2S:
                        break;
                    case Opcode::ENTERMONITOR:
                        break;
                    case Opcode::EXITMONITOR:
                        break;
                    case Opcode::MTPERF:
                        break;
                    case Opcode::MTFPERF:
                        break;
                    case Opcode::CLOSURELOAD:
                        break;
                    case Opcode::REIFIEDLOAD:
                        break;
                    case Opcode::OBJLOAD:
                        break;
                    case Opcode::THROW:
                        break;
                    case Opcode::RET:
                        break;
                    case Opcode::VRET:
                        break;
                    case Opcode::PRINTLN:
                        break;
                }
            }
        }

      private:
        llvm::Value *top() {
            return stack.back();
        }

        void push(llvm::Value *value) {
            stack.push_back(value);
        }

        void push_n(llvm::Value *value, size_t n) {
            for (size_t i = 0; i < n; i++) stack.push_back(value);
        }

        llvm::Value *pop() {
            const auto value = top();
            stack.pop_back();
            return value;
        }

        void pop_n(size_t n) {
            for (size_t i = 0; i < n; i++) stack.pop_back();
        }

        llvm::Value *load_null() {
            return llvm::ConstantPointerNull::get(l_pvoid_t);
        }

        llvm::Value *load_false() {
            return llvm::ConstantInt::get(*l_context, llvm::APInt(1, 0));
        }

        llvm::Value *load_true() {
            return llvm::ConstantInt::get(*l_context, llvm::APInt(1, 1));
        }

        llvm::Value *load_int(int64 value) {
            return llvm::ConstantInt::get(*l_context, llvm::APInt(64, value, true, true));
        }

        llvm::Value *load_const(cpidx index) {
            const auto &obj = conpool[index];
            if (is<ObjBool>(obj)) {
                return llvm::ConstantInt::get(*l_context, llvm::APInt(1, cast<ObjBool>(obj)->truth() ? 1 : 0));
            } else if (is<ObjInt>(obj)) {
                return llvm::ConstantInt::get(*l_context, llvm::APInt(64, cast<ObjInt>(obj)->value(), true, true));
            } else if (is<ObjFloat>(obj)) {
                return llvm::ConstantFP::get(*l_context, llvm::APFloat(cast<ObjFloat>(obj)->value()));
            } else {
                throw std::runtime_error("not yet implemented");
            }
        }

        uint8 read_byte() {
            if (ip - code >= count)
                throw std::runtime_error("code overflowed");
            return *ip++;
        }

        uint16 read_short() {
            if (ip - code >= count)
                throw std::runtime_error("code overflowed");
            return (ip += 2, ip[-2] << 8 | ip[-1]);
        }

        uint32 get_pc() const {
            return ip - code;
        }

        bool is_at_end() const {
            return ip - code >= count;
        }
    };

    void jit_test(ObjMethod *method) {
        const auto &frame = method->get_frame_template();
        const auto module = cast<ObjModule>(method->get_info().manager->get_vm()->get_symbol(method->get_sign().get_parent_module().to_string()));
        JitCompiler compiler(frame.get_code(), frame.get_code_count(), module->get_constant_pool());
        compiler.compile();
        std::cout << method->to_string() << std::endl;
        std::cout << "---bytecode----------------------------------" << std::endl;
        compiler.print_code();
        std::cout << "---llvm--------------------------------------" << std::endl;
        compiler.print_llvm();
        std::cout << "---------------------------------------------" << std::endl;
    }
}    // namespace spade