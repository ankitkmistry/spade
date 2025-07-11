#pragma once

#include <variant>

#include "utils/common.hpp"
#include "parser/ast.hpp"
#include "symbol_path.hpp"

namespace spadec
{
    namespace scope
    {
        class Scope;
        class Compound;
        class Module;
        class Variable;
        class Init;
        class Function;
        class FunctionSet;
        class Block;
    }    // namespace scope

    class TypeInfo;

    struct BasicType {
        /// scope of the type
        scope::Compound *type = null;
        /// type args of the type
        std::vector<TypeInfo> type_args;
        /// flag if the type is nullable
        bool b_nullable = false;

        bool is_type_literal() const {
            return type == null && type_args.empty();
        }

        bool weak_equals(const BasicType &other) const;

        bool operator==(const BasicType &other) const;

        bool operator!=(const BasicType &other) const {
            return !(*this == other);
        }

        string to_string(bool decorated) const;
    };

    struct ParamInfo;

    class FunctionType {
        std::unique_ptr<TypeInfo> m_ret_type;
        std::vector<ParamInfo> m_pos_only_params;
        std::vector<ParamInfo> m_pos_kwd_params;
        std::vector<ParamInfo> m_kwd_only_params;

      public:
        /// type args of the type
        // TODO: To be implemented in future
        // std::vector<TypeInfo> type_args;
        /// flag if the type is nullable
        bool b_nullable = false;

        FunctionType();
        FunctionType(const FunctionType &other);
        FunctionType(FunctionType &&);
        FunctionType &operator=(const FunctionType &other);
        FunctionType &operator=(FunctionType &&);
        ~FunctionType();

        bool is_variadic() const;
        bool is_default() const;
        size_t min_param_count() const;
        size_t param_count() const;

        TypeInfo &return_type() {
            return *m_ret_type;
        }

        const TypeInfo &return_type() const {
            return *m_ret_type;
        }

        std::vector<ParamInfo> &pos_only_params() {
            return m_pos_only_params;
        }

        const std::vector<ParamInfo> &pos_only_params() const {
            return m_pos_only_params;
        }

        std::vector<ParamInfo> &pos_kwd_params() {
            return m_pos_kwd_params;
        }

        const std::vector<ParamInfo> &pos_kwd_params() const {
            return m_pos_kwd_params;
        }

        std::vector<ParamInfo> &kwd_only_params() {
            return m_kwd_only_params;
        }

        const std::vector<ParamInfo> &kwd_only_params() const {
            return m_kwd_only_params;
        }

        bool weak_equals(const FunctionType &other) const;

        bool operator==(const FunctionType &other) const;

        bool operator!=(const FunctionType &other) const {
            return !(*this == other);
        }

        string to_string(bool decorated = true) const;
    };

    bool operator==(const FunctionType &fn_type, scope::Function *function);

    inline bool operator!=(const FunctionType &fn_type, scope::Function *function) {
        return !(fn_type == function);
    }

    inline bool operator==(scope::Function *function, const FunctionType &fn_type) {
        return fn_type == function;
    }

    inline bool operator!=(scope::Function *function, const FunctionType &fn_type) {
        return fn_type != function;
    }

    class TypeInfo {
      public:
        enum class Kind {
            BASIC,
            FUNCTION,
        } tag = Kind::BASIC;

      private:
        std::variant<BasicType, FunctionType> variant;

      public:
        bool &nullable() {
            if (const auto value = std::get_if<BasicType>(&variant))
                return value->b_nullable;
            if (const auto value = std::get_if<FunctionType>(&variant))
                return value->b_nullable;
            throw Unreachable();
        }

        bool nullable() const {
            if (const auto value = std::get_if<BasicType>(&variant))
                return value->b_nullable;
            if (const auto value = std::get_if<FunctionType>(&variant))
                return value->b_nullable;
            throw Unreachable();
        }

        BasicType &basic() {
            if (const auto value = std::get_if<BasicType>(&variant))
                return *value;
            return (tag = Kind::BASIC, std::get<BasicType>(variant = BasicType()));
        }

        const BasicType &basic() const {
            if (const auto value = std::get_if<BasicType>(&variant))
                return *value;
            throw std::bad_variant_access();
        }

        FunctionType &function() {
            if (const auto value = std::get_if<FunctionType>(&variant))
                return *value;
            return (tag = Kind::FUNCTION, std::get<FunctionType>(variant = FunctionType()));
        }

        const FunctionType &function() const {
            if (const auto value = std::get_if<FunctionType>(&variant))
                return *value;
            throw std::bad_variant_access();
        }

        void increase_usage();

        void reset() {
            tag = Kind::BASIC;
            variant = {};
        }

        bool weak_equals(const TypeInfo &other) const {
            if (tag != other.tag)
                return false;
            switch (tag) {
            case Kind::BASIC:
                return basic().weak_equals(other.basic());
            case Kind::FUNCTION:
                return function().weak_equals(other.function());
            }
        }

        bool operator==(const TypeInfo &other) const {
            if (tag != other.tag)
                return false;
            switch (tag) {
            case Kind::BASIC:
                return basic() == other.basic();
            case Kind::FUNCTION:
                return function() == other.function();
            }
        }

        bool operator!=(const TypeInfo &other) const {
            return !(*this == other);
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
        /// The scope of the value referring to
        scope::Scope *scope = null;
        /// The scope of the value referring to
        ParamInfo *param_info = null;

        void reset() {
            b_lvalue = false;
            b_const = false;
            b_null = false;
            b_self = false;
            scope = null;
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

        bool remove_if(const std::function<bool(const std::pair<const SymbolPath &, const scope::Function *> &)> &pred) {
            return std::erase_if(functions, pred) > 0;
        }

        const std::unordered_map<SymbolPath, scope::Function *> &get_functions() const {
            return functions;
        }

        std::unordered_map<SymbolPath, scope::FunctionSet *> get_function_sets() const;

        string to_string(bool decorated = true) const;
    };

    struct ExprInfo {
        enum class Kind {
            NORMAL,
            STATIC,
            MODULE,
            FUNCTION_SET,
        } tag = Kind::NORMAL;

      private:
        std::variant<TypeInfo, scope::Module *, FunctionInfo> variant;

      public:
        ValueInfo value_info;

        TypeInfo &type_info() {
            if (const auto value = std::get_if<TypeInfo>(&variant))
                return *value;
            return (tag = Kind::NORMAL, std::get<TypeInfo>(variant = TypeInfo()));
        }

        const TypeInfo &type_info() const {
            if (const auto value = std::get_if<TypeInfo>(&variant))
                return *value;
            throw std::bad_variant_access();
        }

        scope::Module *&module() {
            if (const auto value = std::get_if<scope::Module *>(&variant))
                return *value;
            return (tag = Kind::MODULE, std::get<scope::Module *>(variant = null));
        }

        const scope::Module *const &module() const {
            if (const auto value = std::get_if<scope::Module *>(&variant))
                return *value;
            throw std::bad_variant_access();
        }

        FunctionInfo &functions() {
            if (const auto value = std::get_if<FunctionInfo>(&variant))
                return *value;
            return (tag = Kind::FUNCTION_SET, std::get<FunctionInfo>(variant = FunctionInfo()));
        }

        const FunctionInfo &functions() const {
            if (const auto value = std::get_if<FunctionInfo>(&variant))
                return *value;
            throw std::bad_variant_access();
        }

        void reset() {
            tag = Kind::NORMAL;
            variant = {};
            value_info.reset();
        }

        bool is_null() const {
            return value_info.b_null;
        }

        string to_string(bool decorated = true) const;
    };

    struct ParamInfo {
        /// flag if the param has been used
        bool b_used = false;
        /// flag if the param is const
        bool b_const = false;
        /// flag if the param is variadic
        bool b_variadic = false;
        /// flag if the param is default
        bool b_default = false;
        /// flag if the param is keyword only
        bool b_kwd_only = false;
        /// name of the param
        string name;
        /// type of the param
        TypeInfo type_info;
        /// ast node of the param
        ast::AstNode *node = null;

        void reset() {
            b_used = false;
            b_const = false;
            b_variadic = false;
            b_default = false;
            b_kwd_only = false;
            name.clear();
            type_info.reset();
            node = null;
        }

        bool operator==(const ParamInfo &other) const;

        string to_string(bool decorated = true) const;
    };

    struct ParamsInfo {
        vector<ParamInfo> pos_only;
        vector<ParamInfo> pos_kwd;
        vector<ParamInfo> kwd_only;

        void reset() {
            pos_only = pos_kwd = kwd_only = {};
        }
    };

    string params_string(const std::vector<ParamInfo> &pos_only_params, const std::vector<ParamInfo> &pos_kwd_params,
                         const std::vector<ParamInfo> &kwd_only_params);

    struct ArgumentInfo {
        bool b_kwd = false;
        string name;
        ExprInfo expr_info;
        ast::AstNode *node = null;

        void reset() {
            b_kwd = false;
            name.clear();
            expr_info.reset();
            node = null;
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

    struct ImportInfo {
        string name;
        bool b_used = false;
        scope::Scope *scope = null;
        const ast::AstNode *node = null;
    };

    struct StmtInfo {
        enum class Kind {
            VAR_USED,
            VAR_ASSIGNED,
        } kind;

        const scope::Variable *var = null;
        const ast::AstNode *node = null;
    };

    class CFNode;

    struct LoopInfo {
        std::vector<std::shared_ptr<CFNode>> continue_nodes;
        std::vector<std::shared_ptr<CFNode>> break_nodes;
    };
}    // namespace spadec