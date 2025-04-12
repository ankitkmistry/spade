#pragma once

#include "utils/error.hpp"

namespace spade
{
    class ErrorPrinter {
      public:
        void print(ErrorType type, const CompilerError &err);

        template<typename T>
        // requires std::derived_from<T, CompilerError> // ErrorGroup already requires this
        void print(const ErrorGroup<T> err_grp) {
            size_t i = 0;
            for (const auto &[type, err]: err_grp.get_errors()) {
                print(type, err);
                if (type == ErrorType::NOTE && i < err_grp.get_errors().size() - 1)
                    std::cout << "\n";
                i++;
            }
        }
    };
}    // namespace spade