#pragma once

#include "utils/error.hpp"
#include "utils/error_printer.hpp"
#include "parser/ast.hpp"
#include "info.hpp"
#include "scope.hpp"

namespace spade
{
    /**
     * @class Analyzer
     * @brief The Analyzer class is responsible for analyzing the abstract syntax tree (AST) of the program.
     * 
     * This class performs various tasks such as name resolution, type checking, context resolution, and
     * function call analysis. It also provides mechanisms for handling scopes, resolving assignments, 
     * and checking for ambiguities in function definitions. The Analyzer class is a visitor for different 
     * AST nodes and processes them accordingly.
     * 
     * @note This class is final and cannot be inherited.
     */
    class Analyzer final : public ast::VisitorBase {
        // Internal modules
        enum class Internal {
            SPADE,
            SPADE_ANY,
            SPADE_ENUM,
            SPADE_ANNOTATION,
            SPADE_THROWABLE,
            SPADE_INT,
            SPADE_FLOAT,
            SPADE_BOOL,
            SPADE_STRING,
            SPADE_VOID,
            SPADE_SLICE,
        };
        std::unordered_map<Internal, std::shared_ptr<scope::Scope>> internals;

        std::unordered_map<ast::Module *, ScopeInfo> module_scopes;
        std::vector<std::shared_ptr<scope::Function>> function_scopes;

        bool basic_mode = false;
        enum class Mode { DECLARATION, DEFINITION };
        Mode mode = Mode::DECLARATION;

        scope::Scope *cur_scope = null;
        scope::Scope *get_parent_scope() const;
        scope::Scope *get_current_scope() const;
        scope::Function *get_current_function() const;

        template<typename T>
            requires std::derived_from<T, scope::Scope>
        T *get_internal(Internal kind) {
            scope::Scope *scope;
            if (basic_mode) {
                switch (kind) {
                    case Internal::SPADE:
                        scope = get_current_scope()->get_enclosing_module();
                        break;
                    case Internal::SPADE_ANY:
                        scope = get_current_scope()->get_enclosing_module();
                        scope = &*scope->get_variable("any");
                        break;
                    case Internal::SPADE_ENUM:
                        scope = get_current_scope()->get_enclosing_module();
                        scope = &*scope->get_variable("Enum");
                        break;
                    case Internal::SPADE_ANNOTATION:
                        scope = get_current_scope()->get_enclosing_module();
                        scope = &*scope->get_variable("Annotation");
                        break;
                    case Internal::SPADE_THROWABLE:
                        scope = get_current_scope()->get_enclosing_module();
                        scope = &*scope->get_variable("Throwable");
                        break;
                    case Internal::SPADE_INT:
                        scope = get_current_scope()->get_enclosing_module();
                        scope = &*scope->get_variable("int");
                        break;
                    case Internal::SPADE_FLOAT:
                        scope = get_current_scope()->get_enclosing_module();
                        scope = &*scope->get_variable("float");
                        break;
                    case Internal::SPADE_BOOL:
                        scope = get_current_scope()->get_enclosing_module();
                        scope = &*scope->get_variable("bool");
                        break;
                    case Internal::SPADE_STRING:
                        scope = get_current_scope()->get_enclosing_module();
                        scope = &*scope->get_variable("string");
                        break;
                    case Internal::SPADE_VOID:
                        scope = get_current_scope()->get_enclosing_module();
                        scope = &*scope->get_variable("void");
                        break;
                    case Internal::SPADE_SLICE:
                        scope = get_current_scope()->get_enclosing_module();
                        scope = &*scope->get_variable("Slice");
                        break;
                }
            } else
                scope = &*internals[kind];
            if constexpr (std::same_as<T, scope::Scope>)
                return scope;
            else
                return cast<T>(scope);
        }

        scope::Scope *get_internal(Internal kind) {
            return get_internal<scope::Scope>(kind);
        }

        /**
         * Loads and sets up internal spade modules
         */
        void load_internal_modules();

        /// Performs name resolution
        ExprInfo resolve_name(const string &name, const ast::AstNode &node);

        void resolve_context(const scope::Scope *from_scope, const scope::Scope *to_scope, const ast::AstNode &node,
                             ErrorGroup<AnalyzerError> &errors) const;

        /**
         * Performs context resolution for scope in relation with the current scope
         * @param scope the requested scope
         * @param node the source ast node used for error messages
         */
        void resolve_context(const scope::Scope *scope, const ast::AstNode &node) const;
        void resolve_context(const std::shared_ptr<scope::Scope> scope, const ast::AstNode &node) const;

        /// Performs cast checking
        void check_cast(scope::Compound *from, scope::Compound *to, const ast::AstNode &node, bool safe);

        /**
         * Performs type resolution for assignments.
         * It checks if the type of the expression is compatible with the type of the variable.
         * @param type type to assign
         * @param expr expression to be assigned
         * @param node the source ast node used for error messages
         * @return the correct type info that is assigned
         */
        TypeInfo resolve_assign(const TypeInfo &type_info, const ExprInfo &expr_info, const ast::AstNode &node);

        /**
         * Performs type resolution for assignments.
         * It checks if the type of the expression is compatible with the type of the variable.
         * If the current scope is a variable, it automatically sets the type info and evaluation state of the variable
         *
         * @param type type to assign
         * @param expr expression to be assigned
         * @param node the source ast node used for error messages
         * @return the correct type info that is assigned
         */
        TypeInfo resolve_assign(std::shared_ptr<ast::Type> type, std::shared_ptr<ast::Expression> expr, const ast::AstNode &node);

        /**
         * This function checks whether @p function can meet the requirements provided by
         * @p{arg_infos}. If this function returns true then @p errors are not changed
         * @param function the function to be checked
         * @param arg_infos the arguments to use
         * @param node the source ast node used for error messages
         * @param errors the place where errors are to be reported
         * @return true if function can take @p arg_infos
         * @return false if function cannot take @p arg_infos
         */
        bool check_fun_call(scope::Function *function, const std::vector<ArgInfo> &arg_infos, const ast::AstNode &node,
                            ErrorGroup<AnalyzerError> &errors);

        /**
         * This function takes in @p arg_infos and selects the best viable function
         * from the function set and returns the ExprInfo of its return value
         * @param fun_set the function set to analyze
         * @param arg_infos the argument infos of the function call
         * @param node the source ast node used for error messages
         * @return ExprInfo the return value expr info of the function
         */
        ExprInfo resolve_call(const FunctionInfo &funs, const std::vector<ArgInfo> &arg_infos, const ast::AstNode &node);

        /**
         * This function resolves the indexer if there was any indexer call because indexers are late resolved
         * so as to correctly detect which version of the indexer has to be called (get_item or set_item)
         * The spade::Analyzer::indexer_info field serves the purpose for this function. After resolution
         * the value in spade::Analyzer::indexer_info is reset. The resultant value of the indexer is saved in @p result
         * @param[out] result the ExprInfo where the result is saved
         * @param[in] get determines which version of the indexer to be called
         * @param[in] node the source ast node used for error messages
         */
        void resolve_indexer(ExprInfo &result, bool get, const ast::AstNode &node);

        /// Performs variable type inference resolution
        ExprInfo get_var_expr_info(std::shared_ptr<scope::Variable> var_scope, const ast::AstNode &node);
        /// Declares a variable in the current block if it is a function
        std::shared_ptr<scope::Variable> declare_variable(ast::decl::Variable &node);

        /**
         * Checks whether @p fun1 and @p fun2 are ambiguous or not
         * @param fun1 the first function
         * @param fun2 the second function
         * @param errors the place where errors are to be reported
         */
        void check_funs(const scope::Function *fun1, const scope::Function *fun2, ErrorGroup<AnalyzerError> &errors) const;

        /**
         * Checks whether all the functions in @p fun_set are well formed 
         * i.e. none of them are ambiguous
         * @param fun_set the set of functions
         */
        void check_fun_set(const std::shared_ptr<scope::FunctionSet> &fun_set);

        static bool check_fun_exactly_same(const scope::Function *fun1, const scope::Function *fun2);

        void check_compatible_supers(const std::shared_ptr<scope::Compound> &klass, const std::vector<scope::Compound *> &supers,
                                     const std::vector<std::shared_ptr<ast::decl::Parent>> &nodes) const;

        ExprInfo get_member(const ExprInfo &caller_info, const string &member_name, bool safe, const ast::AstNode &node,
                            ErrorGroup<AnalyzerError> &errors);
        ExprInfo get_member(const ExprInfo &caller_info, const string &member_name, const ast::AstNode &node, ErrorGroup<AnalyzerError> &errors);
        ExprInfo get_member(const ExprInfo &caller_info, const string &member_name, bool safe, const ast::AstNode &node);
        ExprInfo get_member(const ExprInfo &caller_info, const string &member_name, const ast::AstNode &node);

        ErrorPrinter printer;

        inline AnalyzerError error(const string &msg) const {
            return AnalyzerError(msg, get_current_scope()->get_enclosing_module()->get_module_node()->get_file_path(),
                                 static_cast<ast::AstNode *>(null));
        }

        template<typename T>
        AnalyzerError error(const string &msg, LineInfoVector<T> node) const {
            return AnalyzerError(msg, get_current_scope()->get_enclosing_module()->get_module_node()->get_file_path(), node);
        }

        template<typename T>
            requires ast::HasLineInfo<const T *>
        AnalyzerError error(const string &msg, const T *node) const {
            if constexpr (std::derived_from<T, scope::Scope>)
                return AnalyzerError(msg, node->get_enclosing_module()->get_module_node()->get_file_path(), node);
            else
                return AnalyzerError(msg, get_current_scope()->get_enclosing_module()->get_module_node()->get_file_path(), node);
        }

        template<typename T>
            requires ast::HasLineInfo<const std::shared_ptr<T> &>
        AnalyzerError error(const string &msg, const std::shared_ptr<T> &node) const {
            if constexpr (std::derived_from<T, scope::Scope>)
                return AnalyzerError(msg, node->get_enclosing_module()->get_module_node()->get_file_path(), node);
            else
                return AnalyzerError(msg, get_current_scope()->get_enclosing_module()->get_module_node()->get_file_path(), node);
        }

        template<ast::HasLineInfo T>
        void warning(const string &msg, T node) const {
            printer.print(ErrorType::WARNING, error(msg, node));
        }

        template<ast::HasLineInfo T>
        void note(const string &msg, T node) const {
            printer.print(ErrorType::NOTE, error(msg, node));
        }

        inline std::shared_ptr<scope::Block> begin_block(ast::stmt::Block &node) {
            auto scope = std::make_shared<scope::Block>(&node);
            get_current_scope()->new_variable(std::format("%block{}", get_current_scope()->get_members().size()), null, scope);
            cur_scope = &*scope;
            return scope;
        }

        template<typename Scope_Type>
            requires std::derived_from<Scope_Type, scope::Scope>
        std::shared_ptr<Scope_Type> find_scope(const string &name) {
            auto scope = get_current_scope()->get_variable(name);
            cur_scope = &*scope;
            return cast<Scope_Type>(scope);
        }

        inline void end_scope() {
            cur_scope = cur_scope->get_parent();
        }

      public:
        explicit Analyzer(const std::unordered_map<ast::Module *, ScopeInfo> &module_scopes, ErrorPrinter printer)
            : module_scopes(module_scopes), printer(printer) {}

        void analyze();

      public:
        // Visitor
        void visit(ast::Reference &node);

      private:
        TypeInfo _res_type_info;

      public:
        // Type visitor
        void visit(ast::type::Reference &node);
        void visit(ast::type::Function &node);
        void visit(ast::type::TypeLiteral &node);
        void visit(ast::type::BinaryOp &node);
        void visit(ast::type::Nullable &node);
        void visit(ast::type::TypeBuilder &node);
        void visit(ast::type::TypeBuilderMember &node);

      private:
        ExprInfo _res_expr_info;

      public:
        // Expression visitor
        void visit(ast::expr::Constant &node);
        void visit(ast::expr::Super &node);
        void visit(ast::expr::Self &node);
        void visit(ast::expr::DotAccess &node);
        void visit(ast::expr::Call &node);

      private:
        ArgInfo _res_arg_info;

      public:
        void visit(ast::expr::Argument &node);
        void visit(ast::expr::Reify &node);

      private:
        IndexerInfo indexer_info;

      public:
        void visit(ast::expr::Index &node);
        void visit(ast::expr::Slice &node);
        void visit(ast::expr::Unary &node);
        void visit(ast::expr::Cast &node);
        void visit(ast::expr::Binary &node);
        void visit(ast::expr::ChainBinary &node);
        void visit(ast::expr::Ternary &node);
        void visit(ast::expr::Assignment &node);

      private:
        bool is_loop = false;

      public:
        // Statement visitor
        void visit(ast::stmt::Block &node);
        void visit(ast::stmt::If &node);
        void visit(ast::stmt::While &node);
        void visit(ast::stmt::DoWhile &node);
        void visit(ast::stmt::Throw &node);
        void visit(ast::stmt::Catch &node);
        void visit(ast::stmt::Try &node);
        void visit(ast::stmt::Continue &node);
        void visit(ast::stmt::Break &node);
        void visit(ast::stmt::Return &node);
        void visit(ast::stmt::Yield &node);
        void visit(ast::stmt::Expr &node);
        void visit(ast::stmt::Declaration &node);
        // Declaration visitor
        void visit(ast::decl::TypeParam &node);
        void visit(ast::decl::Constraint &node);

      private:
        ParamInfo _res_param_info;

      public:
        void visit(ast::decl::Param &node);
        void visit(ast::decl::Params &node);
        void visit(ast::decl::Function &node);
        void visit(ast::decl::Variable &node);
        void visit(ast::decl::Parent &node);
        void visit(ast::decl::Enumerator &node);
        void visit(ast::decl::Compound &node);
        // Module level visitor
        void visit(ast::Import &node);
        void visit(ast::Module &node);
        void visit(ast::FolderModule &node);
    };
}    // namespace spade

// Names of functions that represent overloaded operators

// Binary ops
// `a + b` is same as `a.__add__(b)` if `a` has defined `__add__` function
#define OV_OP_POW     (std::string("__pow__"))
#define OV_OP_MUL     (std::string("__mul__"))
#define OV_OP_DIV     (std::string("__div__"))
#define OV_OP_MOD     (std::string("__mod__"))
#define OV_OP_ADD     (std::string("__add__"))
#define OV_OP_SUB     (std::string("__sub__"))
#define OV_OP_LSHIFT  (std::string("__lshift__"))
#define OV_OP_RSHIFT  (std::string("__rshift__"))
#define OV_OP_URSHIFT (std::string("__urshift__"))
#define OV_OP_AND     (std::string("__and__"))
#define OV_OP_XOR     (std::string("__xor__"))
#define OV_OP_OR      (std::string("__or__"))
// `a + b` is same as `b.__rev_add__(a)` if `a` has not defined `__add__` function
#define OV_OP_REV_POW     (std::string("__rev_pow__"))
#define OV_OP_REV_MUL     (std::string("__rev_mul__"))
#define OV_OP_REV_DIV     (std::string("__rev_div__"))
#define OV_OP_REV_MOD     (std::string("__rev_mod__"))
#define OV_OP_REV_ADD     (std::string("__rev_add__"))
#define OV_OP_REV_SUB     (std::string("__rev_sub__"))
#define OV_OP_REV_LSHIFT  (std::string("__rev_lshift__"))
#define OV_OP_REV_RSHIFT  (std::string("__rev_rshift__"))
#define OV_OP_REV_URSHIFT (std::string("__rev_urshift__"))
#define OV_OP_REV_AND     (std::string("__rev_and__"))
#define OV_OP_REV_XOR     (std::string("__rev_xor__"))
#define OV_OP_REV_OR      (std::string("__rev_or__"))
// `a += b` is same as `b.__aug_add__(a)`
#define OV_OP_AUG_POW     (std::string("__aug_pow__"))
#define OV_OP_AUG_MUL     (std::string("__aug_mul__"))
#define OV_OP_AUG_DIV     (std::string("__aug_div__"))
#define OV_OP_AUG_MOD     (std::string("__aug_mod__"))
#define OV_OP_AUG_ADD     (std::string("__aug_add__"))
#define OV_OP_AUG_SUB     (std::string("__aug_sub__"))
#define OV_OP_AUG_LSHIFT  (std::string("__aug_lshift__"))
#define OV_OP_AUG_RSHIFT  (std::string("__aug_rshift__"))
#define OV_OP_AUG_URSHIFT (std::string("__aug_urshift__"))
#define OV_OP_AUG_AND     (std::string("__aug_and__"))
#define OV_OP_AUG_XOR     (std::string("__aug_xor__"))
#define OV_OP_AUG_OR      (std::string("__aug_or__"))
// Comparison operators
#define OV_OP_LT (std::string("__lt__"))
#define OV_OP_LE (std::string("__le__"))
#define OV_OP_EQ (std::string("__eq__"))
#define OV_OP_NE (std::string("__ne__"))
#define OV_OP_GE (std::string("__ge__"))
#define OV_OP_GT (std::string("__gt__"))

// Postfix operators
// `a(arg1, arg2, ...)` is same as `a.__call__(arg1, arg2, ...)`
#define OV_OP_CALL (std::string("__call__"))
// `a[arg1, arg2, ...]` is same as `a.__get_item__(arg1, arg2, ...)`
#define OV_OP_GET_ITEM (std::string("__get_item__"))
// `a[arg1, arg2, ...] = value` is same as `a.__set_item__(arg1, arg2, ..., value)`
#define OV_OP_SET_ITEM (std::string("__set_item__"))
// `a in b` is same as `b.__contains__(a)`
#define OV_OP_CONTAINS (std::string("__contains__"))

// Unary operators
// `~a` is same as `a.__inv__()`
#define OV_OP_INV (std::string("__inv__"))
// `-a` is same as `a.__neg__()`
#define OV_OP_NEG (std::string("__neg__"))
// `+a` is same as `a.__pos__()`
#define OV_OP_POS (std::string("__pos__"))