#pragma once

#include "utils/error.hpp"

namespace spade
{
    class ErrorPrinter {
      public:
        void print(ErrorType type, const CompilerError &err);

        template<typename T>
            requires std::derived_from<T, CompilerError>
        void print(const ErrorGroup<T> err_grp) {
            for (const auto &[type, err]: err_grp.get_errors()) {
                print(type, err);
            }
        }
    };
}    // namespace spade