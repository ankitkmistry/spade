#pragma once

#include "scope.hpp"

namespace spade
{
    struct TypeInfo {
        std::shared_ptr<scope::Compound> type;
        std::vector<TypeInfo> type_args;
        std::shared_ptr<Token> nullable;

        void reset() {
            type = null;
            type_args.clear();
            nullable = null;
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