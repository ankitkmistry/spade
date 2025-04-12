#pragma once

#include "parser/ast.hpp"
#include "utils/common.hpp"

namespace spade
{
    namespace scope
    {
        class Scope;
        class Compound;
        class Module;
        class Init;
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

        TypeInfo &operator=(TypeInfo &&other) noexcept = default;

        ~TypeInfo() {
            reset();
        }

        bool operator==(const TypeInfo &other) const {
            return type == other.type && b_nullable == other.b_nullable && /* b_null == other.b_null && */
                   type_args == other.type_args;
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
            scope::FunctionSet *function_set;
        };

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
                    function_set = other.function_set;
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
                    function_set = std::move(other.function_set);
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
                        function_set = other.function_set;
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
                        function_set = std::move(other.function_set);
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
                    function_set = null;
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

    struct ArgInfo {
        bool b_kwd = false;
        string name;
        ExprInfo expr_info;
        ast::expr::Argument *node = null;

        void reset() {
            b_kwd = false;
            name.clear();
            expr_info.reset();
        }

        string to_string(bool decorated = true) const;
    };

    class ScopeInfo {
        std::shared_ptr<scope::Scope> scope;
        bool original;

      public:
        explicit ScopeInfo(std::shared_ptr<scope::Scope> scope, bool original = true) : scope(scope), original(original) {}

        ScopeInfo(const ScopeInfo &other) = default;
        ScopeInfo(ScopeInfo &&other) noexcept = default;
        ScopeInfo &operator=(const ScopeInfo &other) = default;
        ScopeInfo &operator=(ScopeInfo &&other) noexcept = default;
        ~ScopeInfo() = default;

        std::shared_ptr<scope::Scope> get_scope() const {
            return scope;
        }

        bool is_original() const {
            return original;
        }
    };
}    // namespace spade