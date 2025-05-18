#pragma once

#include "parser/ast.hpp"
#include "symbol_path.hpp"
#include "utils/common.hpp"

namespace spade
{
    namespace scope
    {
        class Scope;
        class Compound;
        class Module;
        class Init;
        class Function;
        class FunctionSet;
    }    // namespace scope

    struct TypeInfo {
        /// scope of the type
        scope::Compound *type = null;
        /// type args of the type
        std::vector<TypeInfo> type_args;
        /// flag if the type is nullable
        bool b_nullable = false;

        TypeInfo() = default;

        TypeInfo(const TypeInfo &other) : type(other.type), type_args(other.type_args), b_nullable(other.b_nullable) {}

        TypeInfo(TypeInfo &&other) noexcept = default;

        TypeInfo &operator=(const TypeInfo &other) {
            type = other.type;
            if (!other.type_args.empty())
                type_args = other.type_args;
            else
                type_args.clear();
            b_nullable = other.b_nullable;
            return *this;
        }

        TypeInfo &operator=(TypeInfo &&other) noexcept {
            type = std::move(other.type);
            if (!other.type_args.empty())
                type_args = std::move(other.type_args);
            else
                type_args.clear();
            b_nullable = std::move(other.b_nullable);
            return *this;
        }

        ~TypeInfo() {
            reset();
        }

        bool operator==(const TypeInfo &other) const {
            return type == other.type && b_nullable == other.b_nullable && type_args == other.type_args;
        }

        bool operator!=(const TypeInfo &other) const {
            return !(*this == other);
        }

        void reset() {
            if (type)
                type = null;
            if (!type_args.empty())
                type_args.clear();
            b_nullable = false;
        }

        bool is_type_literal() const {
            return type == null && type_args.empty();
        }

        string to_string(bool decorated = true) const;
    };

    struct ValueInfo {
        /// flag if the value is a lvalue
        bool b_lvalue = false;
        /// flag if the value is const
        bool b_const = false;
        /// flag if value is null
        bool b_null = false;
        /// flag if value is self
        bool b_self = false;

        void reset() {
            b_lvalue = false;
            b_const = false;
            b_null = false;
            b_self = false;
        }
    };

    class FunctionInfo {
      public:
        bool b_nullable = false;

      private:
        std::unordered_map<SymbolPath, scope::Function *> functions;

      public:
        FunctionInfo() = default;

        FunctionInfo(const FunctionInfo &other) = default;
        FunctionInfo(FunctionInfo &&other) = default;
        FunctionInfo(const scope::FunctionSet *fun_set);
        FunctionInfo &operator=(const FunctionInfo &other) = default;
        FunctionInfo &operator=(FunctionInfo &&other) = default;
        FunctionInfo &operator=(const scope::FunctionSet *fun_set);
        ~FunctionInfo() = default;

        scope::Function *operator[](const SymbolPath &path) const {
            return get(path);
        }

        scope::Function *get(const SymbolPath &path) const {
            return functions.contains(path) ? functions.at(path) : null;
        }

        scope::Function *get_or(const SymbolPath &path, scope::Function *or_else) const {
            return functions.contains(path) ? functions.at(path) : or_else;
        }

        bool empty() const {
            return functions.empty();
        }

        size_t size() const {
            return functions.size();
        }

        void add(const SymbolPath &path, scope::Function *function, bool override = true);

        void extend(const FunctionInfo &other, bool override = true);

        void clear() {
            functions.clear();
        }

        bool remove(const SymbolPath &path) {
            return functions.erase(path) > 0;
        }

        bool remove_if(std::function<bool(const std::pair<const SymbolPath &, const scope::Function *> &)> pred) {
            return std::erase_if(functions, pred) > 0;
        }

        const std::unordered_map<SymbolPath, scope::Function *> &get_functions() const {
            return functions;
        }

        std::unordered_map<SymbolPath, scope::FunctionSet *> get_function_sets() const;

        string to_string(bool decorated = true) const;
    };

    struct ExprInfo {
        enum class Type {
            NORMAL,
            STATIC,
            MODULE,
            FUNCTION_SET,
        } tag = Type::NORMAL;

        union {
            TypeInfo type_info;
            scope::Module *module;
        };

        // Placed this outside the union due to some union related runtime errors.
        // The cause of the error is that FunctionInfo is a object which has a STL container
        // and during construction of the object everything is initialized to zero which corrupts
        // the state of the STL container. This is why it is necessary to put functions outside the union
        FunctionInfo functions;

        ValueInfo value_info;

        ExprInfo() : type_info() {}

        ExprInfo(const ExprInfo &other) : tag(other.tag), value_info(other.value_info) {
            switch (tag) {
                case Type::NORMAL:
                case Type::STATIC:
                    type_info = other.type_info;
                    break;
                case Type::MODULE:
                    module = other.module;
                    break;
                case Type::FUNCTION_SET:
                    functions = other.functions;
                    break;
            }
        }

        ExprInfo(ExprInfo &&other) noexcept : tag(other.tag), value_info(other.value_info) {
            switch (tag) {
                case Type::NORMAL:
                case Type::STATIC:
                    type_info = std::move(other.type_info);
                    break;
                case Type::MODULE:
                    module = std::move(other.module);
                    break;
                case Type::FUNCTION_SET:
                    functions = std::move(other.functions);
                    break;
            }
        }

        ExprInfo &operator=(const ExprInfo &other) {
            if (this != &other) {
                reset();
                tag = other.tag;
                value_info = other.value_info;
                switch (tag) {
                    case Type::NORMAL:
                    case Type::STATIC:
                        type_info = other.type_info;
                        break;
                    case Type::MODULE:
                        module = other.module;
                        break;
                    case Type::FUNCTION_SET:
                        functions = other.functions;
                        break;
                }
            }
            return *this;
        }

        ExprInfo &operator=(ExprInfo &&other) noexcept {
            if (this != &other) {
                reset();
                tag = other.tag;
                value_info = other.value_info;
                switch (tag) {
                    case Type::NORMAL:
                    case Type::STATIC:
                        type_info = std::move(other.type_info);
                        break;
                    case Type::MODULE:
                        module = std::move(other.module);
                        break;
                    case Type::FUNCTION_SET:
                        functions = std::move(other.functions);
                        break;
                }
            }
            return *this;
        }

        ~ExprInfo() {
            reset();
        }

        void reset() {
            switch (tag) {
                case Type::NORMAL:
                    type_info.reset();
                    break;
                case Type::STATIC:
                    type_info.reset();
                    break;
                case Type::MODULE:
                    module = null;
                    break;
                case Type::FUNCTION_SET:
                    functions.clear();
                    break;
            }
            tag = Type::NORMAL;
            value_info.reset();
        }

        bool is_null() const {
            return tag == Type::NORMAL && value_info.b_null && type_info.b_nullable;
        }

        string to_string(bool decorated = true) const;
    };

    struct ParamInfo {
        bool b_const = false;
        bool b_variadic = false;
        bool b_default = false;
        bool b_kwd_only = false;
        string name;
        TypeInfo type_info;
        ast::decl::Param *node = null;

        void reset() {
            b_const = false;
            b_variadic = false;
            name.clear();
            type_info.reset();
        }

        string to_string(bool decorated = true) const;
    };

    struct ArgumentInfo {
        bool b_kwd = false;
        string name;
        ExprInfo expr_info;
        ast::AstNode *node = null;

        void reset() {
            b_kwd = false;
            name.clear();
            expr_info.reset();
        }

        string to_string(bool decorated = true) const;
    };

    struct IndexerInfo {
        ExprInfo caller_info;
        std::vector<ArgumentInfo> arg_infos;
        ast::AstNode *node = null;

        operator bool() const {
            return node != null;
        }

        void reset() {
            caller_info.reset();
            arg_infos.clear();
            node = null;
        }
    };
}    // namespace spade