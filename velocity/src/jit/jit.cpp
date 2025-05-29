#include "jit.hpp"

#ifndef DISABLE_JIT

#include <cstddef>
#include <cstring>
#include <memory>
#include <ostream>
#include <variant>

#include "utils/constants.hpp"

#ifdef COMPILER_MSVC
#    pragma warning(push)
#    pragma warning(disable : 4624)
#    pragma warning(disable : 4244)
#endif

#ifdef COMPILER_GCC
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wall"
#endif

#ifdef COMPILER_CLANG
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wall"
#endif

#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include "llvm/Transforms/Utils/Mem2Reg.h"

#ifdef COMPILER_MSVC
#    pragma warning(pop)
#endif

#ifdef COMPILER_GCC
#    pragma GCC diagnostic pop
#endif

#ifdef COMPILER_CLANG
#    pragma clang diagnostic pop
#endif

#include "objects/inbuilt_types.hpp"
#include "objects/obj.hpp"
#include "utils/common.hpp"
#include "spinfo/opcode.hpp"
#include "callable/frame_template.hpp"
#include "objects/int.hpp"
#include "objects/float.hpp"
#include "ee/vm.hpp"

#if defined(OS_WINDOWS)
#    define JIT_API_EXPORT extern "C" __declspec(dllexport)
#else
#    define JIT_API_EXPORT extern "C"
#endif

        JIT_API_EXPORT uint8_t
        obj_truth(int8_t *pointer) {
    const auto obj = (spade::Obj *) pointer;
    return obj->truth() ? 1 : 0;
}

JIT_API_EXPORT int8_t *obj_to_string(int8_t *pointer) {
    const auto obj = (spade::Obj *) pointer;
    return (int8_t *) spade::halloc<spade::ObjString>(obj->to_string());
}

JIT_API_EXPORT int32_t obj_cmp(int8_t *p1, int8_t *p2) {
    const auto obj1 = (spade::ObjComparable *) p1;
    const auto obj2 = (spade::ObjComparable *) p2;
    return obj1->compare(obj2);
}

JIT_API_EXPORT void print_bool(int64_t b) {
    if (b == 0)
        std::printf("false\n");
    else
        std::printf("true\n");
}

namespace spade
{
    class JitCompiler;

    class Comparator {
        llvm::Value *value;
        JitCompiler &compiler;

      public:
        Comparator(llvm::Value *value, JitCompiler &compiler) : value(value), compiler(compiler) {}

        llvm::Value *operator<(const Comparator &c);
        llvm::Value *operator<=(const Comparator &c);
        llvm::Value *operator==(const Comparator &c);
        llvm::Value *operator!=(const Comparator &c);
        llvm::Value *operator>=(const Comparator &c);
        llvm::Value *operator>(const Comparator &c);
    };

    class Block;

    class Instruction {
        uint8 *pos = null;
        Opcode opcode = Opcode::NOP;
        std::variant<uint16, Block *> value;

      public:
        Instruction(uint8 *pos, Opcode opcode, uint16 param) : pos(pos), opcode(opcode), value(param) {}

        Instruction(uint8 *pos, Opcode opcode, Block *jump) : pos(pos), opcode(opcode), value(jump) {}

        Instruction(const Instruction &) = default;
        Instruction(Instruction &&) = default;
        Instruction &operator=(const Instruction &) = default;
        Instruction &operator=(Instruction &&) = default;
        ~Instruction() = default;

        std::pair<uint8 *, uint8 *> range() const {
            return {pos, pos + OpcodeInfo::params_count(opcode)};
        }

        bool contains(uint8 *ip) const {
            return pos <= ip && ip < pos + OpcodeInfo::params_count(opcode);
        }

        uint8 *get_pos() const {
            return pos;
        }

        Opcode get_opcode() const {
            return opcode;
        }

        std::optional<Block *> get_jump() const {
            if (const auto p = std::get_if<Block *>(&value))
                return *p;
            return std::nullopt;
        }

        void set_jump(Block *block) {
            value = block;
        }

        std::optional<uint16> get_param() const {
            if (const auto p = std::get_if<uint16>(&value))
                return *p;
            return std::nullopt;
        }
    };

    class Block {
        vector<Instruction> instructions;

      public:
        Block(const vector<Instruction> &instructions) : instructions(instructions) {}

        Block() = default;
        Block(const Block &) = default;
        Block(Block &&) = default;
        Block &operator=(const Block &) = default;
        Block &operator=(Block &&) = default;
        ~Block() = default;

        std::pair<uint8 *, uint8 *> range() const {
            if (instructions.empty())
                return {null, null};
            return {instructions.front().range().first, instructions.back().range().second};
        }

        bool contains(uint8 *ip) const {
            const auto pair = range();
            return pair.first <= ip && ip < pair.second;
        }

        const vector<Instruction> &get_instructions() const {
            return instructions;
        }

        vector<Instruction> &get_instructions() {
            return instructions;
        }

        void add_instruction(const Instruction &instr) {
            instructions.push_back(instr);
        }

        bool empty() {
            return instructions.empty();
        }
    };

    class JitCompiler {
        friend class Comparator;
        // Code specific
        string fn_name;
        FrameTemplate frame;
        vector<Block> blocks;
        vector<Obj *> conpool;
        vector<llvm::Value *> stack;
        vector<llvm::AllocaInst *> locals;

        // LLVM specific
        std::unique_ptr<llvm::LLVMContext> l_context;
        std::unique_ptr<llvm::Module> l_module;
        std::unique_ptr<llvm::IRBuilder<>> l_builder;

        std::unique_ptr<llvm::FunctionPassManager> l_fpm;
        std::unique_ptr<llvm::LoopAnalysisManager> l_lam;
        std::unique_ptr<llvm::FunctionAnalysisManager> l_fam;
        std::unique_ptr<llvm::CGSCCAnalysisManager> l_cgam;
        std::unique_ptr<llvm::ModuleAnalysisManager> l_mam;
        std::unique_ptr<llvm::PassInstrumentationCallbacks> l_pic;
        std::unique_ptr<llvm::StandardInstrumentations> l_si;

        llvm::Function *fn;
        bool has_return_type = false;

        // Inbuilt types and functions
        llvm::PointerType *l_ptr_t;
        llvm::Function *fn_pow;
        llvm::Function *fn_printf;
        llvm::Function *fn_print_bool;
        llvm::Function *fn_obj_truth;
        llvm::Function *fn_obj_cmp;
        llvm::Function *fn_obj_to_string;

      public:
        size_t find_ip_in_blocks(const vector<Block> &blocks, uint8 *ip, size_t start, size_t end) {
            if (end - start == 0)
                return -1;
            if (end - start == 1)
                if (blocks[start].contains(ip))
                    return start;
            if (end - start == 2) {
                if (blocks[start].contains(ip))
                    return start;
                else if (blocks[start + 1].contains(ip))
                    return start + 1;
            }

            auto start_ip = blocks[start].range().first;
            size_t mid = (start + end) / 2;
            auto mid_ip = blocks[mid].range().first;
            auto end_ip = blocks[end - 1].range().second;

            if (start_ip <= ip && ip < mid_ip)
                return find_ip_in_blocks(blocks, ip, start, mid);
            if (mid_ip <= ip && ip < end_ip)
                return find_ip_in_blocks(blocks, ip, mid, end);
            return -1;
        }

        size_t find_ip_in_blocks(const vector<Block> &blocks, uint8 *ip) {
            return find_ip_in_blocks(blocks, ip, 0, blocks.size());
        }

        void init_instructions() {
            // [from_ip] -> [to_block_idx]
            std::unordered_map<uint8 *, size_t> jump_tos;
            {
                uint8 *ip = frame.get_code();
                vector<Block> blocks;
                Block cur_block;
                std::unordered_map<uint8 *, int16> split_points;
                while (ip < frame.get_code() + frame.get_code_count()) {
                    uint8 *pos = ip;
                    const Opcode opcode = static_cast<Opcode>(*ip++);
                    uint16 param;
                    switch (OpcodeInfo::params_count(opcode)) {
                        case 2:
                            param = (ip += 2, ip[-2] << 8 | ip[-1]);
                            break;
                        case 1:
                            param = *ip++;
                            break;
                        default:
                            param = 0;
                            break;
                    }

                    Instruction ins(pos, opcode, param);
                    cur_block.add_instruction(ins);
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
                            int16 offset = static_cast<int16>(param);
                            split_points[ip + offset] = offset;
                            blocks.push_back(cur_block);
                            cur_block = {};
                            break;
                        }
                        case Opcode::RET:
                            has_return_type = true;
                            blocks.push_back(cur_block);
                            cur_block = {};
                            break;
                        case Opcode::VRET:
                            has_return_type = false;
                            blocks.push_back(cur_block);
                            cur_block = {};
                            break;
                        default:
                            break;
                    }
                }

                vector<Block> result;
                for (const auto &block: blocks) {
                    cur_block = {};
                    for (const auto &ins: block.get_instructions()) {
                        if (const auto it = split_points.find(ins.get_pos()); it != split_points.end()) {
                            if (!cur_block.empty()) {
                                result.push_back(cur_block);
                                cur_block = {};
                            }
                        }
                        cur_block.add_instruction(ins);
                    }
                    result.push_back(cur_block);
                }
                for (const auto &block: result) {
                    for (const auto &ins: block.get_instructions()) {
                        if (const auto it = split_points.find(ins.get_pos()); it != split_points.end()) {
                            // 3 -> one increment for opcode, 2 increments for param
                            // final_ip = from_ip + 3 + offset
                            // from_ip = final_ip - offset - 3
                            const auto ip = ins.get_pos() - it->second - 3;
                            jump_tos[ip] = find_ip_in_blocks(result, ip);
                        }
                    }
                }
                this->blocks = result;
            }
            for (auto &block: blocks) {
                for (auto &ins: block.get_instructions()) {
                    switch (ins.get_opcode()) {
                        case Opcode::JMP:
                        case Opcode::JT:
                        case Opcode::JF:
                        case Opcode::JLT:
                        case Opcode::JLE:
                        case Opcode::JEQ:
                        case Opcode::JNE:
                        case Opcode::JGE:
                        case Opcode::JGT:
                            ins.set_jump(&blocks[jump_tos[ins.get_pos()]]);
                            break;
                        default:
                            break;
                    }
                }
            }
        }

        void init() {
            fn_name = frame.get_method()->get_sign().get_elements().back().get_name();
            conpool = cast<ObjModule>(SpadeVM::current()->get_symbol(frame.get_method()->get_sign().get_parent_module().to_string()))
                              ->get_constant_pool();
            init_instructions();

            l_context = std::make_unique<llvm::LLVMContext>();
            l_module = std::make_unique<llvm::Module>("spadejit", *l_context);

            // Create a new builder for the module
            l_builder = std::make_unique<llvm::IRBuilder<>>(*l_context);

            // Create new pass and analysis managers
            l_fpm = std::make_unique<llvm::FunctionPassManager>();
            l_lam = std::make_unique<llvm::LoopAnalysisManager>();
            l_fam = std::make_unique<llvm::FunctionAnalysisManager>();
            l_cgam = std::make_unique<llvm::CGSCCAnalysisManager>();
            l_mam = std::make_unique<llvm::ModuleAnalysisManager>();
            l_pic = std::make_unique<llvm::PassInstrumentationCallbacks>();
            l_si = std::make_unique<llvm::StandardInstrumentations>(*l_context, /*DebugLogging*/ true);

            l_si->registerCallbacks(*l_pic, l_mam.get());

            // Add transform passes.
            // Promote allocas to registers.
            l_fpm->addPass(llvm::PromotePass());
            // Do simple "peephole" optimizations and bit-twiddling optzns.
            l_fpm->addPass(llvm::InstCombinePass());
            // Reassociate expressions.
            l_fpm->addPass(llvm::ReassociatePass());
            // Eliminate Common SubExpressions.
            l_fpm->addPass(llvm::GVNPass());
            // Simplify the control flow graph (deleting unreachable blocks, etc).
            l_fpm->addPass(llvm::SimplifyCFGPass());

            // Register analysis passes used in these transform passes.
            llvm::PassBuilder l_pb;
            l_pb.registerModuleAnalyses(*l_mam);
            l_pb.registerFunctionAnalyses(*l_fam);
            l_pb.crossRegisterProxies(*l_lam, *l_fam, *l_cgam, *l_mam);

            // l_ptr_t : i8*
            l_ptr_t = llvm::PointerType::get(llvm::Type::getInt8Ty(*l_context), 0);
            {
                // Declare i32 printf(i8 *, ...)
                const vector<llvm::Type *> params{l_ptr_t};
                llvm::FunctionType *const t_printf = llvm::FunctionType::get(llvm::Type::getInt32Ty(*l_context), params, true);
                fn_printf = llvm::Function::Create(t_printf, llvm::Function::ExternalLinkage, "printf", *l_module);
            }
            {
                // Declare double pow(double, double)
                const vector<llvm::Type *> params(2, llvm::Type::getDoubleTy(*l_context));
                llvm::FunctionType *const t_pow = llvm::FunctionType::get(llvm::Type::getDoubleTy(*l_context), params, false);
                fn_pow = llvm::Function::Create(t_pow, llvm::Function::ExternalLinkage, "pow", *l_module);
            }
            {
                // Declare void print_bool(i64)
                const vector<llvm::Type *> params{llvm::Type::getInt64Ty(*l_context)};
                llvm::FunctionType *const t_print_bool = llvm::FunctionType::get(llvm::Type::getVoidTy(*l_context), params, false);
                fn_print_bool = llvm::Function::Create(t_print_bool, llvm::Function::ExternalLinkage, "print_bool", *l_module);
            }
            {
                // Declare u8 obj_truth(i8 *)
                const vector<llvm::Type *> params{l_ptr_t};
                llvm::FunctionType *const t_obj_truth = llvm::FunctionType::get(llvm::Type::getInt8Ty(*l_context), params, false);
                fn_obj_truth = llvm::Function::Create(t_obj_truth, llvm::Function::ExternalLinkage, "obj_truth", *l_module);
            }
            {
                // Declare i8 obj_cmp(i8 *, i8 *)
                const vector<llvm::Type *> params(2, l_ptr_t);
                llvm::FunctionType *const t_obj_cmp = llvm::FunctionType::get(llvm::Type::getInt8Ty(*l_context), params, false);
                fn_obj_cmp = llvm::Function::Create(t_obj_cmp, llvm::Function::ExternalLinkage, "obj_cmp", *l_module);
            }
            {
                // Declare i8 *obj_to_string(i8 *pointer)
                const vector<llvm::Type *> params{l_ptr_t};
                llvm::FunctionType *const t_obj_to_string = llvm::FunctionType::get(l_ptr_t, params, false);
                fn_obj_to_string = llvm::Function::Create(t_obj_to_string, llvm::Function::ExternalLinkage, "obj_to_string", *l_module);
            }
        }

        JitCompiler(const FrameTemplate &frame) : frame(frame) {
            init();
        }

        JitCompiler(const JitCompiler &other) = delete;
        JitCompiler &operator=(const JitCompiler &other) = delete;
        JitCompiler(JitCompiler &&other) noexcept = default;
        JitCompiler &operator=(JitCompiler &&other) noexcept = default;
        ~JitCompiler() = default;

        void print_llvm() {
            l_module->print(llvm::errs(), null);
        }

        void print_code() {
            uint8 *ip = frame.get_code();
            const auto read_byte = [&] { return *ip++; };
            const auto read_short = [&] { return (ip += 2, ip[-2] << 8 | ip[-1]); };
            while (ip < frame.get_code() + frame.get_code_count()) {
                std::cout << ip - frame.get_code() << ": ";
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
            std::cout << std::flush;
        }

        void generate(Block &block) {
            block_map[&block] = l_builder->GetInsertBlock();
            // Start generation
            for (auto &instr: block.get_instructions()) {
                const Opcode opcode = instr.get_opcode();
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
                        push(load_const(*instr.get_param()));
                        break;
                    case Opcode::CONSTL:
                        push(load_const(*instr.get_param()));
                        break;
                    case Opcode::POP:
                        pop();
                        break;
                    case Opcode::NPOP:
                        pop_n(*instr.get_param());
                        break;
                    case Opcode::DUP:
                        push(top());
                        break;
                    case Opcode::NDUP:
                        push_n(top(), *instr.get_param());
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
                    case Opcode::LFLOAD: {
                        const auto index = *instr.get_param();
                        const auto local = locals[index];
                        push(l_builder->CreateLoad(local->getAllocatedType(), local, frame.get_locals().get_local(index).get_name()));
                        break;
                    }
                    case Opcode::LSTORE:
                    case Opcode::LFSTORE: {
                        const auto index = *instr.get_param();
                        const auto local = locals[index];
                        const auto value = top();
                        if (local->getAllocatedType() == value->getType())
                            l_builder->CreateStore(value, local);
                        break;
                    }
                    case Opcode::PLSTORE:
                    case Opcode::PLFSTORE: {
                        const auto index = *instr.get_param();
                        const auto local = locals[index];
                        const auto value = pop();
                        if (local->getAllocatedType() == value->getType())
                            l_builder->CreateStore(value, local);
                        break;
                    }
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
                    case Opcode::JT:
                    case Opcode::JF:
                    case Opcode::JLT:
                    case Opcode::JLE:
                    case Opcode::JEQ:
                    case Opcode::JNE:
                    case Opcode::JGE:
                    case Opcode::JGT:
                        return;
                    case Opcode::NOT: {
                        const auto value = pop();
                        if (value->getType()->isIntegerTy(1))
                            push(l_builder->CreateNot(value, "res_not"));
                        break;
                    }
                    case Opcode::INV: {
                        const auto value = pop();
                        if (value->getType()->isIntegerTy(64))
                            push(l_builder->CreateXor(value, load_int64(-1), "res_inv"));
                        break;
                    }
                    case Opcode::NEG: {
                        const auto value = pop();
                        if (value->getType()->isIntegerTy(64))
                            push(l_builder->CreateSub(load_int64(0), value, "res_neg"));
                        else if (value->getType()->isDoubleTy())
                            push(l_builder->CreateFNeg(value, "res_neg"));
                        break;
                    }
                    case Opcode::GETTYPE:
                        break;
                    case Opcode::SCAST:
                        break;
                    case Opcode::CCAST:
                        break;
                    case Opcode::CONCAT:
                        break;
                    case Opcode::POW: {
                        break;
                    }
                    case Opcode::MUL: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            push(l_builder->CreateMul(a, b, "res_mul"));
                        else if (a->getType()->isDoubleTy() && b->getType()->isDoubleTy())
                            push(l_builder->CreateFMul(a, b, "res_mul"));
                        break;
                    }
                    case Opcode::DIV: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            push(l_builder->CreateSDiv(a, b, "res_div"));
                        else if (a->getType()->isDoubleTy() && b->getType()->isDoubleTy())
                            push(l_builder->CreateFDiv(a, b, "res_div"));
                        break;
                    }
                    case Opcode::REM: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            push(l_builder->CreateSRem(a, b, "res_mod"));
                        break;
                    }
                    case Opcode::ADD: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            push(l_builder->CreateAdd(a, b, "res_add"));
                        else if (a->getType()->isDoubleTy() && b->getType()->isDoubleTy())
                            push(l_builder->CreateFAdd(a, b, "res_add"));
                        break;
                    }
                    case Opcode::SUB: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            push(l_builder->CreateSub(a, b, "res_sub"));
                        else if (a->getType()->isDoubleTy() && b->getType()->isDoubleTy())
                            push(l_builder->CreateFSub(a, b, "res_sub"));
                        break;
                    }
                    case Opcode::SHL: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            push(l_builder->CreateShl(a, b, "res_shl"));
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
                            push(l_builder->CreateLShr(a, b, "res_shl"));
                        break;
                    }
                    case Opcode::AND: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            push(l_builder->CreateAnd(a, b, "res_and"));
                        break;
                    }
                    case Opcode::OR: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            push(l_builder->CreateOr(a, b, "res_or"));
                        break;
                    }
                    case Opcode::XOR: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
                            push(l_builder->CreateXor(a, b, "res_xor"));
                        break;
                    }
                    case Opcode::LT: {
                        const auto b = pop();
                        const auto a = pop();
                        push(Comparator(a, *this) < Comparator(b, *this));
                        break;
                    }
                    case Opcode::LE: {
                        const auto b = pop();
                        const auto a = pop();
                        push(Comparator(a, *this) <= Comparator(b, *this));
                        break;
                    }
                    case Opcode::EQ: {
                        const auto b = pop();
                        const auto a = pop();
                        push(Comparator(a, *this) == Comparator(b, *this));
                        break;
                    }
                    case Opcode::NE: {
                        const auto b = pop();
                        const auto a = pop();
                        push(Comparator(a, *this) != Comparator(b, *this));
                        break;
                    }
                    case Opcode::GE: {
                        const auto b = pop();
                        const auto a = pop();
                        push(Comparator(a, *this) >= Comparator(b, *this));
                        break;
                    }
                    case Opcode::GT: {
                        const auto b = pop();
                        const auto a = pop();
                        push(Comparator(a, *this) > Comparator(b, *this));
                        break;
                    }
                    case Opcode::IS: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isPointerTy() && b->getType()->isPointerTy()) {
                            const auto address_a = l_builder->CreatePtrToInt(a, llvm::Type::getInt64Ty(*l_context));
                            const auto address_b = l_builder->CreatePtrToInt(b, llvm::Type::getInt64Ty(*l_context));
                            const auto cmp = l_builder->CreateICmpEQ(a, b, "res_is");
                            push(cmp);
                        }
                        break;
                    }
                    case Opcode::NIS: {
                        const auto b = pop();
                        const auto a = pop();
                        if (a->getType()->isPointerTy() && b->getType()->isPointerTy()) {
                            const auto address_a = l_builder->CreatePtrToInt(a, llvm::Type::getInt64Ty(*l_context));
                            const auto address_b = l_builder->CreatePtrToInt(b, llvm::Type::getInt64Ty(*l_context));
                            const auto cmp = l_builder->CreateICmpNE(a, b, "res_nis");
                            push(cmp);
                        }
                        break;
                    }
                    case Opcode::ISNULL: {
                        const auto value = pop();
                        if (value->getType()->isPointerTy()) {
                            const auto address = l_builder->CreatePtrToInt(value, llvm::Type::getInt64Ty(*l_context));
                            const auto cmp = l_builder->CreateICmpEQ(value, load_int64(0), "res_isnull");
                            push(cmp);
                        }
                        break;
                    }
                    case Opcode::NISNULL: {
                        const auto value = pop();
                        if (value->getType()->isPointerTy()) {
                            const auto address = l_builder->CreatePtrToInt(value, llvm::Type::getInt64Ty(*l_context));
                            const auto cmp = l_builder->CreateICmpNE(value, load_int64(0), "res_nisnull");
                            push(cmp);
                        }
                        break;
                    }
                    case Opcode::I2F:
                        push(l_builder->CreateSIToFP(pop(), llvm::Type::getDoubleTy(*l_context)));
                        break;
                    case Opcode::F2I:
                        push(l_builder->CreateFPToSI(pop(), llvm::Type::getInt64Ty(*l_context)));
                        break;
                    case Opcode::I2B: {
                        const auto value = pop();
                        if (value->getType()->isIntegerTy(64)) {
                            const auto cmp = l_builder->CreateICmpNE(value, load_int64(0), "res_i2b");
                            push(cmp);
                        }
                        break;
                    }
                    case Opcode::B2I: {
                        const auto value = pop();
                        if (value->getType()->isIntegerTy(1)) {
                            const auto result = l_builder->CreateZExt(value, llvm::Type::getInt64Ty(*l_context), "res_b2i");
                            push(result);
                        }
                        break;
                    }
                    case Opcode::O2B: {
                        const auto value = pop();
                        if (value->getType()->isPointerTy()) {
                            llvm::Value *res_o2b_call = l_builder->CreateCall(fn_obj_truth, {value}, "res_o2b_call");
                            push(l_builder->CreateICmpEQ(res_o2b_call, load_int8(1), "res_o2b"));
                        }
                        break;
                    }
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
                    case Opcode::VRET:
                        return;
                    case Opcode::PRINTLN: {
                        auto value = pop();
                        llvm::Value *fmt_str;
                        if (value->getType()->isIntegerTy(1)) {
                            value = l_builder->CreateZExt(value, llvm::Type::getInt64Ty(*l_context));
                            l_builder->CreateCall(fn_print_bool, {value}, "res_println");
                        } else if (value->getType()->isIntegerTy(8)) {
                            fmt_str = l_builder->CreateGlobalStringPtr("%c\\n");
                            l_builder->CreateCall(fn_printf, {fmt_str, value}, "res_println");
                        } else if (value->getType()->isIntegerTy(64)) {
                            fmt_str = l_builder->CreateGlobalStringPtr("%ld\\n");
                            l_builder->CreateCall(fn_printf, {fmt_str, value}, "res_println");
                        } else if (value->getType()->isDoubleTy()) {
                            fmt_str = l_builder->CreateGlobalStringPtr("%g\\n");
                            l_builder->CreateCall(fn_printf, {fmt_str, value}, "res_println");
                        }
                        break;
                    }
                }
            }
        }

        std::unordered_map<Block *, llvm::BasicBlock *> block_map;

        llvm::BasicBlock *get_llvm_block(Block *block) {
            if (const auto it = block_map.find(block); it != block_map.end())
                return it->second;
            return null;
        }

        llvm::BasicBlock *patch_block_end(size_t &cur_idx) {
            auto &block = blocks[cur_idx++];
            const auto next_block = cur_idx == blocks.size() ? null : &blocks[cur_idx];
            const auto ins = block.get_instructions().back();
            if (!next_block) {
                if (l_builder->GetInsertBlock()->getName() == "start")
                    l_builder->GetInsertBlock()->setName("fun");
                else
                    l_builder->GetInsertBlock()->setName("end");
            }
            switch (ins.get_opcode()) {
#define COND_JUMP_CODE()                                                                                                                             \
    do {                                                                                                                                             \
        Block *then_block = *ins.get_jump();                                                                                                         \
        Block *else_block = next_block;                                                                                                              \
                                                                                                                                                     \
        llvm::BasicBlock *then_branch = get_llvm_block(then_block);                                                                                  \
                                                                                                                                                     \
        if (else_block) {                                                                                                                            \
            if (then_branch) {                                                                                                                       \
                llvm::BasicBlock *else_branch = llvm::BasicBlock::Create(*l_context, "else", fn);                                                    \
                l_builder->CreateCondBr(cond, then_branch, else_branch);                                                                             \
                return else_branch;                                                                                                                  \
            } else {                                                                                                                                 \
                if (then_block == else_block) {                                                                                                      \
                    llvm::BasicBlock *bb_next = llvm::BasicBlock::Create(*l_context, "then_else");                                                   \
                    l_builder->CreateBr(bb_next);                                                                                                    \
                    fn->insert(fn->end(), bb_next);                                                                                                  \
                    return bb_next;                                                                                                                  \
                } else {                                                                                                                             \
                    llvm::BasicBlock *else_branch = llvm::BasicBlock::Create(*l_context, "else", fn);                                                \
                    then_branch = llvm::BasicBlock::Create(*l_context, "then");                                                                      \
                    l_builder->CreateCondBr(cond, then_branch, else_branch);                                                                         \
                                                                                                                                                     \
                    while (else_branch != null && &blocks[cur_idx] != then_block) {                                                                  \
                        l_builder->SetInsertPoint(else_branch);                                                                                      \
                        generate(blocks[cur_idx]);                                                                                                   \
                        else_branch = patch_block_end(cur_idx);                                                                                      \
                    }                                                                                                                                \
                                                                                                                                                     \
                    fn->insert(fn->end(), then_branch);                                                                                              \
                    l_builder->SetInsertPoint(then_branch);                                                                                          \
                    return then_branch;                                                                                                              \
                }                                                                                                                                    \
            }                                                                                                                                        \
        }                                                                                                                                            \
        if (has_return_type)                                                                                                                         \
            l_builder->CreateRet(load_null());                                                                                                       \
        else                                                                                                                                         \
            l_builder->CreateRetVoid();                                                                                                              \
        return null;                                                                                                                                 \
    } while (false)
                case Opcode::JMP: {
                    Block *dest_block = *ins.get_jump();
                    llvm::BasicBlock *dest_branch = get_llvm_block(dest_block);
                    if (dest_branch) {
                        l_builder->CreateBr(dest_branch);
                        // if (next_block) {
                        //     llvm::BasicBlock *bb_next = llvm::BasicBlock::Create(*l_context, "block");
                        //     fn->insert(fn->end(), bb_next);
                        //     return bb_next;
                        // } else
                        return null;
                    } else {
                        dest_branch = llvm::BasicBlock::Create(*l_context, "dest");
                        l_builder->CreateBr(dest_branch);

                        if (next_block != dest_block) {
                            llvm::BasicBlock *next_branch = llvm::BasicBlock::Create(*l_context, "block", fn);
                            while (next_branch != null && &blocks[cur_idx] != dest_block) {
                                l_builder->SetInsertPoint(next_branch);
                                generate(blocks[cur_idx]);
                                next_branch = patch_block_end(cur_idx);
                            }
                        }

                        fn->insert(fn->end(), dest_branch);
                        l_builder->SetInsertPoint(dest_branch);
                        return dest_branch;
                    }
                }
                case Opcode::JT: {
                    const auto cond = pop();
                    if (cond->getType()->isIntegerTy(1))
                        COND_JUMP_CODE();
                }
                case Opcode::JF: {
                    const auto value = pop();
                    const auto cond = l_builder->CreateNot(value, "res_jf_not");
                    if (cond->getType()->isIntegerTy(1))
                        COND_JUMP_CODE();
                }
                case Opcode::JLT: {
                    const auto b = pop();
                    const auto a = pop();
                    const auto cond = Comparator(a, *this) < Comparator(b, *this);
                    COND_JUMP_CODE();
                }
                case Opcode::JLE: {
                    const auto b = pop();
                    const auto a = pop();
                    const auto cond = Comparator(a, *this) <= Comparator(b, *this);
                    COND_JUMP_CODE();
                }
                case Opcode::JEQ: {
                    const auto b = pop();
                    const auto a = pop();
                    const auto cond = Comparator(a, *this) == Comparator(b, *this);
                    COND_JUMP_CODE();
                }
                case Opcode::JNE: {
                    const auto b = pop();
                    const auto a = pop();
                    const auto cond = Comparator(a, *this) != Comparator(b, *this);
                    COND_JUMP_CODE();
                }
                case Opcode::JGE: {
                    const auto b = pop();
                    const auto a = pop();
                    const auto cond = Comparator(a, *this) >= Comparator(b, *this);
                    COND_JUMP_CODE();
                }
                case Opcode::JGT: {
                    const auto b = pop();
                    const auto a = pop();
                    const auto cond = Comparator(a, *this) > Comparator(b, *this);
                    COND_JUMP_CODE();
                }
#undef COND_JUMP_CODE
                case Opcode::RET:
                case Opcode::VRET:
                    l_builder->CreateRetVoid();
                    return null;
                default:
                    if (next_block) {
                        llvm::BasicBlock *bb_next = llvm::BasicBlock::Create(*l_context, "block");
                        l_builder->CreateBr(bb_next);
                        fn->insert(fn->end(), bb_next);
                        return bb_next;
                    } else {
                        if (has_return_type)
                            l_builder->CreateRet(load_null());
                        else
                            l_builder->CreateRetVoid();
                        return null;
                    }
            }
        }

        void generate() {
            size_t i = 0;
            Block &block = blocks[i];
            llvm::BasicBlock *bb = llvm::BasicBlock::Create(*l_context, "prologue", fn);

            l_builder->SetInsertPoint(bb);

            for (size_t local_idx = 0; local_idx < frame.get_locals().get_closure_start(); local_idx++) {
                const auto &local = frame.get_locals().get_local(local_idx);
                const auto &name = local.get_name();
                const auto value = local.get_value();
                llvm::Type *type;
                if (is<ObjNull>(value))
                    type = l_ptr_t;
                else if (is<ObjBool>(value))
                    type = llvm::Type::getInt1Ty(*l_context);
                else if (is<ObjChar>(value))
                    type = llvm::Type::getInt8Ty(*l_context);
                else if (is<ObjInt>(value))
                    type = llvm::Type::getInt64Ty(*l_context);
                else if (is<ObjFloat>(value))
                    type = llvm::Type::getDoubleTy(*l_context);
                else
                    type = l_ptr_t;
                locals.push_back(l_builder->CreateAlloca(type, null, name));
            }
            bb = llvm::BasicBlock::Create(*l_context, "start", fn);
            l_builder->CreateBr(bb);
            l_builder->SetInsertPoint(bb);
            generate(block);
            bb = patch_block_end(i);

            for (; i < blocks.size();) {
                if (!bb)
                    break;
                block = blocks[i];
                l_builder->SetInsertPoint(bb);
                generate(block);
                bb = patch_block_end(i);
            }
        }

        void compile() {
            // Make function type void()
            llvm::FunctionType *fn_type;
            if (has_return_type)
                fn_type = llvm::FunctionType::get(l_ptr_t, false);
            else
                fn_type = llvm::FunctionType::get(llvm::Type::getVoidTy(*l_context), false);
            fn = llvm::Function::Create(fn_type, llvm::Function::ExternalLinkage, fn_name, *l_module);
            // Start generation
            generate();
            llvm::verifyFunction(*fn, &llvm::errs());
            // l_fpm->run(*fn, *l_fam);
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
            return llvm::ConstantPointerNull::get(l_ptr_t);
        }

        llvm::Value *load_false() {
            return llvm::ConstantInt::get(*l_context, llvm::APInt(1, 0));
        }

        llvm::Value *load_true() {
            return llvm::ConstantInt::get(*l_context, llvm::APInt(1, 1));
        }

        llvm::Value *load_int8(int8 value) {
            return llvm::ConstantInt::get(*l_context, llvm::APInt(8, value));
        }

        llvm::Value *load_int32(int32 value) {
            return llvm::ConstantInt::get(*l_context, llvm::APInt(32, value));
        }

        llvm::Value *load_int64(int64 value) {
            return llvm::ConstantInt::get(*l_context, llvm::APInt(64, value, true, true));
        }

        llvm::Value *load_double(double value) {
            return llvm::ConstantFP::get(*l_context, llvm::APFloat(value));
        }

        llvm::Value *load_const(cpidx index) {
            const auto obj = conpool[index];
            if (is<ObjNull>(obj)) {
                return load_null();
            } else if (is<ObjBool>(obj)) {
                return obj->truth() ? load_true() : load_false();
            } else if (is<ObjChar>(obj)) {
                return llvm::ConstantInt::get(*l_context, llvm::APInt(8, obj->to_string()[0]));
            } else if (is<ObjInt>(obj)) {
                return load_int64(cast<ObjInt>(obj)->value());
            } else if (is<ObjFloat>(obj)) {
                return load_double(cast<ObjFloat>(obj)->value());
            } else {
                const auto address = load_int64((int64) (intptr) obj);
                return l_builder->CreateIntToPtr(address, l_ptr_t, "res_ptr");
            }
        }

        llvm::Function *get_fn() const {
            return fn;
        }
    };

    llvm::Value *Comparator::operator<(const Comparator &c) {
        auto &l_builder = *compiler.l_builder;
        const auto fn_obj_cmp = compiler.fn_obj_cmp;
        const auto a = value;
        const auto b = c.value;
        if (a->getType()->isIntegerTy(1) && b->getType()->isIntegerTy(1))
            return l_builder.CreateICmpULT(a, b, "res_lt");
        else if (a->getType()->isIntegerTy(8) && b->getType()->isIntegerTy(8))
            return l_builder.CreateICmpULT(a, b, "res_lt");
        else if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
            return l_builder.CreateICmpSLT(a, b, "res_lt");
        else if (a->getType()->isDoubleTy() && b->getType()->isDoubleTy())
            return l_builder.CreateFCmpOLT(a, b, "res_lt");
        else if (a->getType()->isPointerTy() && b->getType()->isPointerTy()) {
            llvm::Value *res_cmp = l_builder.CreateCall(fn_obj_cmp, {a, b}, "res_cmp");
            return l_builder.CreateICmpSLT(res_cmp, compiler.load_int32(0), "res_lt");
        }
        return null;
    }

    llvm::Value *Comparator::operator<=(const Comparator &c) {
        auto &l_builder = *compiler.l_builder;
        const auto fn_obj_cmp = compiler.fn_obj_cmp;
        const auto a = value;
        const auto b = c.value;
        if (a->getType()->isIntegerTy(1) && b->getType()->isIntegerTy(1))
            return l_builder.CreateICmpULE(a, b, "res_le");
        else if (a->getType()->isIntegerTy(8) && b->getType()->isIntegerTy(8))
            return l_builder.CreateICmpULE(a, b, "res_le");
        else if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
            return l_builder.CreateICmpSLE(a, b, "res_le");
        else if (a->getType()->isDoubleTy() && b->getType()->isDoubleTy())
            return l_builder.CreateFCmpOLE(a, b, "res_le");
        else if (a->getType()->isPointerTy() && b->getType()->isPointerTy()) {
            llvm::Value *res_cmp = l_builder.CreateCall(fn_obj_cmp, {a, b}, "res_cmp");
            return l_builder.CreateICmpSLE(res_cmp, compiler.load_int32(0), "res_le");
        }
        return null;
    }

    llvm::Value *Comparator::operator==(const Comparator &c) {
        auto &l_builder = *compiler.l_builder;
        const auto fn_obj_cmp = compiler.fn_obj_cmp;
        const auto a = value;
        const auto b = c.value;
        if (a->getType()->isIntegerTy(1) && b->getType()->isIntegerTy(1))
            return l_builder.CreateICmpEQ(a, b, "res_eq");
        else if (a->getType()->isIntegerTy(8) && b->getType()->isIntegerTy(8))
            return l_builder.CreateICmpEQ(a, b, "res_eq");
        else if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
            return l_builder.CreateICmpEQ(a, b, "res_eq");
        else if (a->getType()->isDoubleTy() && b->getType()->isDoubleTy())
            return l_builder.CreateFCmpOEQ(a, b, "res_eq");
        else if (a->getType()->isPointerTy() && b->getType()->isPointerTy()) {
            llvm::Value *res_cmp = l_builder.CreateCall(fn_obj_cmp, {a, b}, "res_cmp");
            return l_builder.CreateICmpEQ(res_cmp, compiler.load_int32(0), "res_eq");
        }
        return null;
    }

    llvm::Value *Comparator::operator!=(const Comparator &c) {
        auto &l_builder = *compiler.l_builder;
        const auto fn_obj_cmp = compiler.fn_obj_cmp;
        const auto a = value;
        const auto b = c.value;
        if (a->getType()->isIntegerTy(1) && b->getType()->isIntegerTy(1))
            return l_builder.CreateICmpNE(a, b, "res_ne");
        else if (a->getType()->isIntegerTy(8) && b->getType()->isIntegerTy(8))
            return l_builder.CreateICmpNE(a, b, "res_ne");
        else if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
            return l_builder.CreateICmpNE(a, b, "res_ne");
        else if (a->getType()->isDoubleTy() && b->getType()->isDoubleTy())
            return l_builder.CreateFCmpONE(a, b, "res_ne");
        else if (a->getType()->isPointerTy() && b->getType()->isPointerTy()) {
            llvm::Value *res_cmp = l_builder.CreateCall(fn_obj_cmp, {a, b}, "res_cmp");
            return l_builder.CreateICmpNE(res_cmp, compiler.load_int32(0), "res_ne");
        }
        return null;
    }

    llvm::Value *Comparator::operator>=(const Comparator &c) {
        auto &l_builder = *compiler.l_builder;
        const auto fn_obj_cmp = compiler.fn_obj_cmp;
        const auto a = value;
        const auto b = c.value;
        if (a->getType()->isIntegerTy(1) && b->getType()->isIntegerTy(1))
            return l_builder.CreateICmpUGE(a, b, "res_ge");
        else if (a->getType()->isIntegerTy(8) && b->getType()->isIntegerTy(8))
            return l_builder.CreateICmpUGE(a, b, "res_ge");
        else if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
            return l_builder.CreateICmpSGE(a, b, "res_ge");
        else if (a->getType()->isDoubleTy() && b->getType()->isDoubleTy())
            return l_builder.CreateFCmpOGE(a, b, "res_ge");
        else if (a->getType()->isPointerTy() && b->getType()->isPointerTy()) {
            llvm::Value *res_cmp = l_builder.CreateCall(fn_obj_cmp, {a, b}, "res_cmp");
            return l_builder.CreateICmpSGE(res_cmp, compiler.load_int32(0), "res_ge");
        }
        return null;
    }

    llvm::Value *Comparator::operator>(const Comparator &c) {
        auto &l_builder = *compiler.l_builder;
        const auto fn_obj_cmp = compiler.fn_obj_cmp;
        const auto a = value;
        const auto b = c.value;
        if (a->getType()->isIntegerTy(1) && b->getType()->isIntegerTy(1))
            return l_builder.CreateICmpUGT(a, b, "res_gt");
        else if (a->getType()->isIntegerTy(8) && b->getType()->isIntegerTy(8))
            return l_builder.CreateICmpUGT(a, b, "res_gt");
        else if (a->getType()->isIntegerTy(64) && b->getType()->isIntegerTy(64))
            return l_builder.CreateICmpSGT(a, b, "res_gt");
        else if (a->getType()->isDoubleTy() && b->getType()->isDoubleTy())
            return l_builder.CreateFCmpOGT(a, b, "res_gt");
        else if (a->getType()->isPointerTy() && b->getType()->isPointerTy()) {
            llvm::Value *res_cmp = l_builder.CreateCall(fn_obj_cmp, {a, b}, "res_cmp");
            return l_builder.CreateICmpSGT(res_cmp, compiler.load_int32(0), "res_gt");
        }
        return null;
    }

    void jit_test(ObjMethod *method) {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();
        // llvm::ExitOnError exit_on_error;

        JitCompiler compiler(method->get_frame_template());
        compiler.compile();
        std::cout << method->to_string() << std::endl;
        std::cout << "---bytecode----------------------------------" << std::endl;
        compiler.print_code();
        std::cout << "---llvm--------------------------------------" << std::endl;
        compiler.print_llvm();
        std::cout << "---------------------------------------------" << std::endl;
    }
}    // namespace spade

#endif
