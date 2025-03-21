#pragma once

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
    }    // namespace scope

    struct TypeInfo {
        scope::Compound *type;
        std::vector<TypeInfo> type_args;
        bool b_nullable = false;

        TypeInfo() = default;
        TypeInfo(const TypeInfo &other) = default;
        TypeInfo(TypeInfo &&other) noexcept = default;
        TypeInfo &operator=(const TypeInfo &other) = default;
        TypeInfo &operator=(TypeInfo &&other) noexcept = default;

        ~TypeInfo() {
            reset();
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
    };

    struct ExprInfo {
        enum class Type {
            NORMAL,
            STATIC,
            MODULE,
            INIT,
            FUNCTION,
        } tag = Type::NORMAL;

        union {
            TypeInfo type_info;
            std::shared_ptr<scope::Module> module;
            std::shared_ptr<scope::Init> init;
            std::shared_ptr<scope::Function> function;
        };

        ExprInfo() : type_info() {}

        ExprInfo(const ExprInfo &other) : tag(other.tag) {
            switch (tag) {
                case Type::NORMAL:
                case Type::STATIC:
                    type_info = other.type_info;
                    break;
                case Type::MODULE:
                    module = other.module;
                    break;
                case Type::INIT:
                    init = other.init;
                    break;
                case Type::FUNCTION:
                    function = other.function;
                    break;
            }
        }

        ExprInfo(ExprInfo &&other) noexcept : tag(other.tag) {
            switch (tag) {
                case Type::NORMAL:
                case Type::STATIC:
                    type_info = std::move(other.type_info);
                    break;
                case Type::MODULE:
                    module = std::move(other.module);
                    break;
                case Type::INIT:
                    init = std::move(other.init);
                    break;
                case Type::FUNCTION:
                    function = std::move(other.function);
                    break;
            }
        }

        ExprInfo &operator=(const ExprInfo &other) {
            if (this != &other) {
                reset();
                tag = other.tag;
                switch (tag) {
                    case Type::NORMAL:
                    case Type::STATIC:
                        type_info = other.type_info;
                        break;
                    case Type::MODULE:
                        module = other.module;
                        break;
                    case Type::INIT:
                        init = other.init;
                        break;
                    case Type::FUNCTION:
                        function = other.function;
                        break;
                }
            }
            return *this;
        }

        ExprInfo &operator=(ExprInfo &&other) noexcept {
            if (this != &other) {
                reset();
                tag = other.tag;
                switch (tag) {
                    case Type::NORMAL:
                    case Type::STATIC:
                        type_info = std::move(other.type_info);
                        break;
                    case Type::MODULE:
                        module = std::move(other.module);
                        break;
                    case Type::INIT:
                        init = std::move(other.init);
                        break;
                    case Type::FUNCTION:
                        function = std::move(other.function);
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
                    module.reset();
                    break;
                case Type::INIT:
                    init.reset();
                    break;
                case Type::FUNCTION:
                    function.reset();
                    break;
            }
            tag = Type::NORMAL;
        }
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