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
        enum class Internal { SPADE, SPADE_ANY, SPADE_INT, SPADE_FLOAT, SPADE_BOOL, SPADE_STRING, SPADE_VOID };
        std::unordered_map<Internal, std::shared_ptr<scope::Scope>> internals;

        std::unordered_map<ast::Module *, ScopeInfo> module_scopes;
        std::vector<std::shared_ptr<scope::Function>> function_scopes;

        enum class Mode { DECLARATION, DEFINITION };
        Mode mode = Mode::DECLARATION;

        scope::Scope *cur_scope = null;
        scope::Scope *get_parent_scope() const;
        scope::Scope *get_current_scope() const;

        /**
         * Loads and sets up internal spade modules
         */
        void load_internal_modules();

        /// Performs name resolution
        std::shared_ptr<scope::Scope> find_name(const string &name) const;

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
        TypeInfo resolve_assign(const TypeInfo *type_info, const ExprInfo *expr_info, const ast::AstNode &node);

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
        TypeInfo resolve_assign(std::shared_ptr<ast::Type> type, std::shared_ptr<ast::Expression> expr,
                                const ast::AstNode &node);

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

        /// Performs variable type inference resolution
        ExprInfo get_var_expr_info(std::shared_ptr<scope::Variable> var_scope, const ast::AstNode &node);

        /**
         * Checks whether @p fun1 and @p fun2 are ambiguous or not
         * @param fun1 the first function
         * @param fun2 the second function
         * @param errors the place where errors are to be reported
         */
        void check_funs(const std::shared_ptr<scope::Function> &fun1, const std::shared_ptr<scope::Function> &fun2,
                        ErrorGroup<AnalyzerError> &errors) const;

        /**
         * Checks whether all the functions in @p fun_set are well formed 
         * i.e. none of them are ambiguous
         * @param fun_set the set of functions
         */
        void check_fun_set(const std::shared_ptr<scope::FunctionSet> &fun_set);

        ErrorPrinter printer;

        inline AnalyzerError error(const string &msg) const {
            return AnalyzerError(msg, get_current_scope()->get_enclosing_module()->get_module_node()->get_file_path(),
                                 static_cast<ast::AstNode *>(null));
        }

        template<ast::HasLineInfo T>
        AnalyzerError error(const string &msg, T node) const {
            return AnalyzerError(msg, get_current_scope()->get_enclosing_module()->get_module_node()->get_file_path(), node);
        }

        template<ast::HasLineInfo T>
        void warning(const string &msg, T node) {
            printer.print(ErrorType::WARNING, error(msg, node));
        }

        template<ast::HasLineInfo T>
        void note(const string &msg, T node) {
            printer.print(ErrorType::NOTE, error(msg, node));
        }

        template<typename Scope_Type, typename Ast_Type>
            requires std::derived_from<Scope_Type, scope::Scope> && std::derived_from<Ast_Type, ast::AstNode>
        std::shared_ptr<Scope_Type> begin_scope(Ast_Type &node) {
            auto scope = std::make_shared<Scope_Type>(&node);
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

      private:
        std::shared_ptr<scope::Scope> _res_reference;

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
        // Expression visitor
      private:
        ExprInfo _res_expr_info;

      public:
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
        void visit(ast::expr::Index &node);
        void visit(ast::expr::Slice &node);
        void visit(ast::expr::Unary &node);
        void visit(ast::expr::Cast &node);
        void visit(ast::expr::Binary &node);
        void visit(ast::expr::ChainBinary &node);
        void visit(ast::expr::Ternary &node);
        void visit(ast::expr::Assignment &node);
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
