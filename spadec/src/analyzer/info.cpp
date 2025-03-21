#include "info.hpp"
#include "scope.hpp"

namespace spade
{
    string TypeInfo::to_string() const {
        return type->to_string();
    }

    string ExprInfo::to_string() const {
        switch (tag) {
            case Type::NORMAL:
            case Type::STATIC:
                return type_info.to_string();
            case Type::MODULE:
                return module->to_string();
            case Type::INIT:
                return init->to_string();
            case Type::FUNCTION:
                return function->to_string();
        }
    }
}    // namespace spade