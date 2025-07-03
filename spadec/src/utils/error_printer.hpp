#pragma once

#include "utils/error.hpp"

namespace spadec
{
    class ErrorPrinter {
      public:
        void print(ErrorType type, const CompilerError &err) const;

        template<typename T>
        void print(const ErrorGroup<T> err_grp) const {
            const auto &errs = err_grp.get_errors();
            std::vector<ErrorGroup<T>> err_grps;

            bool newer = true;
            for (size_t i = 0; i < errs.size(); i++) {
                if (newer) {
                    err_grps.push_back(ErrorGroup<T>(std::pair<ErrorType, T>(errs[i])));
                    newer = false;
                } else
                    err_grps.back().extend(ErrorGroup<T>(std::pair<ErrorType, T>(errs[i])));

                if (i + 1 < errs.size()) {
                    if (errs[i + 1].first == ErrorType::ERROR || errs[i + 1].first == ErrorType::WARNING)
                        newer = true;
                }
            }

            for (size_t i = 0; const auto &err_grp: err_grps) {
                for (const auto &[type, err]: err_grp.get_errors()) {
                    print(type, err);
                }
                if (i != err_grps.size() - 1)
                    std::cout << std::endl;
                i++;
            }
        }
    };
}    // namespace spadec