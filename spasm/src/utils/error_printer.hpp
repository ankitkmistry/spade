#pragma once

#include <iostream>
#include "error.hpp"

namespace spasm
{
    class ErrorPrinter {
      public:
        void print(ErrorType type, const AssemblerError &err) const;

        template<typename T>
        void print(const ErrorGroup<T> err_grp) const {
            size_t i = 0;
            for (const auto &[type, err]: err_grp.get_errors()) {
                print(type, err);
                if (type == ErrorType::NOTE && i < err_grp.get_errors().size() - 1)
                    std::cout << "\n";
                i++;
            }
        }
    };
}    // namespace spasm
