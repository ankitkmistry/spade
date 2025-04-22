#pragma once

#include "lexer/token.hpp"
#include <concepts>

namespace spade::ast
{
    class FolderModule;
    class Module;
    class Import;
    class Declaration;

    namespace decl
    {
        class Compound;
        class Enumerator;
        class Parent;
        class Constraint;
        class TypeParam;
        class Variable;
        class Param;
        class Params;
        class Function;
    }    // namespace decl

    class Reference;
    class AstNode;
    class Expression;

    namespace expr
    {
        class Slice;
        class Argument;
        class Assignment;
        class Ternary;
        class ChainBinary;
        class Binary;
        class Cast;
        class Unary;
        class Index;
        class Reify;
        class Call;
        class DotAccess;
        class Self;
        class Super;
        class Constant;
    }    // namespace expr

    namespace type
    {
        class TypeBuilderMember;
        class TypeBuilder;
        class Nullable;
        class BinaryOp;
        class TypeLiteral;
        class Function;
        class Reference;
    }    // namespace type

    namespace stmt
    {
        class Declaration;
        class Expr;
        class Yield;
        class Return;
        class Break;
        class Continue;
        class Try;
        class Catch;
        class Throw;
        class DoWhile;
        class While;
        class If;
        class Block;
    }    // namespace stmt

    class VisitorBase {
      public:
        VisitorBase() = default;
        VisitorBase(const VisitorBase &other) = default;
        VisitorBase(VisitorBase &&other) noexcept = default;
        VisitorBase &operator=(const VisitorBase &other) = default;
        VisitorBase &operator=(VisitorBase &&other) noexcept = default;
        virtual ~VisitorBase() = default;

        // Visitor
        virtual void visit(Reference &node) = 0;
        // Type visitor
        virtual void visit(type::Reference &node) = 0;
        virtual void visit(type::Function &node) = 0;
        virtual void visit(type::TypeLiteral &node) = 0;
        virtual void visit(type::BinaryOp &node) = 0;
        virtual void visit(type::Nullable &node) = 0;
        virtual void visit(type::TypeBuilder &node) = 0;
        virtual void visit(type::TypeBuilderMember &node) = 0;
        // Expression visitor
        virtual void visit(expr::Constant &node) = 0;
        virtual void visit(expr::Super &node) = 0;
        virtual void visit(expr::Self &node) = 0;
        virtual void visit(expr::DotAccess &node) = 0;
        virtual void visit(expr::Call &node) = 0;
        virtual void visit(expr::Argument &node) = 0;
        virtual void visit(expr::Reify &node) = 0;
        virtual void visit(expr::Index &node) = 0;
        virtual void visit(expr::Slice &node) = 0;
        virtual void visit(expr::Unary &node) = 0;
        virtual void visit(expr::Cast &node) = 0;
        virtual void visit(expr::Binary &node) = 0;
        virtual void visit(expr::ChainBinary &node) = 0;
        virtual void visit(expr::Ternary &node) = 0;
        virtual void visit(expr::Assignment &node) = 0;
        // Statement visitor
        virtual void visit(stmt::Block &node) = 0;
        virtual void visit(stmt::If &node) = 0;
        virtual void visit(stmt::While &node) = 0;
        virtual void visit(stmt::DoWhile &node) = 0;
        virtual void visit(stmt::Throw &node) = 0;
        virtual void visit(stmt::Catch &node) = 0;
        virtual void visit(stmt::Try &node) = 0;
        virtual void visit(stmt::Continue &node) = 0;
        virtual void visit(stmt::Break &node) = 0;
        virtual void visit(stmt::Return &node) = 0;
        virtual void visit(stmt::Yield &node) = 0;
        virtual void visit(stmt::Expr &node) = 0;
        virtual void visit(stmt::Declaration &node) = 0;
        // Declaration visitor
        virtual void visit(decl::TypeParam &node) = 0;
        virtual void visit(decl::Constraint &node) = 0;
        virtual void visit(decl::Param &node) = 0;
        virtual void visit(decl::Params &node) = 0;
        virtual void visit(decl::Function &node) = 0;
        virtual void visit(decl::Variable &node) = 0;
        virtual void visit(decl::Parent &node) = 0;
        virtual void visit(decl::Enumerator &node) = 0;
        virtual void visit(decl::Compound &node) = 0;
        // Module level visitor
        virtual void visit(Import &node) = 0;
        virtual void visit(Module &node) = 0;
        virtual void visit(FolderModule &node) = 0;
    };

    template<typename T>
    concept HasLineInfo = requires(T t) {
        { static_cast<bool>(t) } -> std::same_as<bool>;
        { t->get_line_start() } -> std::same_as<int>;
        { t->get_line_end() } -> std::same_as<int>;
        { t->get_col_start() } -> std::same_as<int>;
        { t->get_col_end() } -> std::same_as<int>;
    };

    class AstNode {
      protected:
        int line_start;
        int line_end;
        int col_start;
        int col_end;

      public:
        AstNode(int line_start, int line_end, int col_start, int col_end)
            : line_start(line_start), line_end(line_end), col_start(col_start), col_end(col_end) {}

        template<typename T1, typename T2>
            requires HasLineInfo<T1> && HasLineInfo<T2>
        AstNode(T1 start, T2 end)
            : line_start(start ? start->get_line_start() : -1),
              line_end(end ? end->get_line_end() : -1),
              col_start(start ? start->get_col_start() : -1),
              col_end(end ? end->get_col_end() : -1) {}

        AstNode(const AstNode &other) = default;
        AstNode(AstNode &&other) noexcept = default;
        AstNode &operator=(const AstNode &other) = default;
        AstNode &operator=(AstNode &&other) noexcept = default;
        virtual ~AstNode() = default;

        virtual void accept(VisitorBase *visitor) = 0;

        int get_line_start() const {
            return line_start;
        }

        int get_line_end() const {
            return line_end;
        }

        int get_col_start() const {
            return col_start;
        }

        int get_col_end() const {
            return col_end;
        }
    };

    class Reference : public AstNode {
        std::vector<std::shared_ptr<Token>> path;

      public:
        explicit Reference(const std::vector<std::shared_ptr<Token>> &path) : AstNode(path.front(), path.back()), path(path) {}

        const std::vector<std::shared_ptr<Token>> &get_path() const {
            return path;
        }

        void accept(VisitorBase *visitor) override {
            visitor->visit(*this);
        }
    };

    class Type : public AstNode {
      public:
        template<typename T1, typename T2>
            requires HasLineInfo<T1> && HasLineInfo<T2>
        Type(T1 start, T2 end) : AstNode(start, end) {}
    };

    namespace type
    {
        class Reference final : public Type {
            std::shared_ptr<ast::Reference> reference;
            std::vector<std::shared_ptr<Type>> type_args;

          public:
            Reference(const std::shared_ptr<Token> &end, const std::shared_ptr<ast::Reference> &reference,
                      const std::vector<std::shared_ptr<Type>> &type_args)
                : Type(reference, end), reference(reference), type_args(type_args) {}

            Reference(const std::shared_ptr<ast::Reference> &reference) : Type(reference, reference), reference(reference) {}

            std::shared_ptr<ast::Reference> get_reference() const {
                return reference;
            }

            const std::vector<std::shared_ptr<Type>> &get_type_args() const {
                return type_args;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Function final : public Type {
            std::vector<std::shared_ptr<Type>> param_types;
            std::shared_ptr<Type> return_type;

          public:
            Function(const std::shared_ptr<Token> &start, const std::vector<std::shared_ptr<Type>> &param_types,
                     const std::shared_ptr<Type> &return_type)
                : Type(start, return_type), param_types(param_types), return_type(return_type) {}

            const std::vector<std::shared_ptr<Type>> &get_param_types() const {
                return param_types;
            }

            const std::shared_ptr<Type> &get_return_type() const {
                return return_type;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class TypeLiteral final : public Type {
          public:
            TypeLiteral(const std::shared_ptr<Token> &token) : Type(token, token) {}

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class BinaryOp final : public Type {
            std::shared_ptr<Type> left;
            std::shared_ptr<Token> op;
            std::shared_ptr<Type> right;

          public:
            BinaryOp(const std::shared_ptr<Type> &left, const std::shared_ptr<Token> &op, const std::shared_ptr<Type> &right)
                : Type(left, right), left(left), op(op), right(right) {}

            const std::shared_ptr<Type> &get_left() const {
                return left;
            }

            const std::shared_ptr<Token> &get_op() const {
                return op;
            }

            const std::shared_ptr<Type> &get_right() const {
                return right;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Nullable final : public Type {
            std::shared_ptr<Type> type;

          public:
            Nullable(const std::shared_ptr<Token> &end, const std::shared_ptr<Type> &type) : Type(type, end), type(type) {}

            const std::shared_ptr<Type> &get_type() const {
                return type;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class TypeBuilderMember final : public AstNode {
            std::shared_ptr<Token> name;
            std::shared_ptr<Type> type;

          public:
            TypeBuilderMember(const std::shared_ptr<Token> &name, const std::shared_ptr<Type> &type)
                : AstNode(name, type), name(name), type(type) {}

            const std::shared_ptr<Token> &get_name() const {
                return name;
            }

            const std::shared_ptr<Type> &get_type() const {
                return type;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class TypeBuilder final : public Type {
            std::vector<std::shared_ptr<TypeBuilderMember>> members;

          public:
            template<typename T1, typename T2>
                requires HasLineInfo<T1> && HasLineInfo<T2>
            explicit TypeBuilder(T1 start, T2 end, const std::vector<std::shared_ptr<TypeBuilderMember>> &members)
                : Type(start, end), members(members) {}

            const std::vector<std::shared_ptr<TypeBuilderMember>> &get_members() const {
                return members;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };
    }    // namespace type

    class Expression : public AstNode {
      public:
        template<typename T1, typename T2>
            requires HasLineInfo<T1> && HasLineInfo<T2>
        Expression(T1 start, T2 end) : AstNode(start, end) {}
    };

    namespace expr
    {
        class Constant : public Expression {
            std::shared_ptr<Token> token;

          public:
            Constant(const std::shared_ptr<Token> &token) : Expression(token, token), token(token) {}

            const std::shared_ptr<Token> &get_token() const {
                return token;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Super : public Expression {
            std::shared_ptr<Reference> reference;

          public:
            Super(const std::shared_ptr<Token> &start, const std::shared_ptr<Token> &end,
                  const std::shared_ptr<Reference> &reference)
                : Expression(start, end), reference(reference) {}

            const std::shared_ptr<Reference> &get_reference() const {
                return reference;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Self : public Expression {
          public:
            Self(const std::shared_ptr<Token> &self) : Expression(self, self) {}

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Postfix : public Expression {
          protected:
            std::shared_ptr<Expression> caller;
            std::shared_ptr<Token> safe;

          public:
            Postfix(const std::shared_ptr<Token> &end, const std::shared_ptr<Expression> &caller,
                    const std::shared_ptr<Token> &safe)
                : Expression(caller, end), caller(caller), safe(safe) {}

            const std::shared_ptr<Expression> &get_caller() const {
                return caller;
            }

            const std::shared_ptr<Token> &get_safe() const {
                return safe;
            }
        };

        class DotAccess : public Postfix {
          protected:
            std::shared_ptr<Token> member;

          public:
            DotAccess(const std::shared_ptr<Expression> &caller, const std::shared_ptr<Token> &safe,
                      const std::shared_ptr<Token> &member)
                : Postfix(member, caller, safe), member(member) {}

            const std::shared_ptr<Token> &get_member() const {
                return member;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Argument : public AstNode {
            std::shared_ptr<Token> name;
            std::shared_ptr<Expression> expr;

          public:
            Argument(const std::shared_ptr<Token> &name, const std::shared_ptr<Expression> &expr)
                : AstNode(name, expr), name(name), expr(expr) {}

            Argument(const std::shared_ptr<Expression> &expr) : AstNode(expr, expr), expr(expr) {}

            const std::shared_ptr<Token> &get_name() const {
                return name;
            }

            const std::shared_ptr<Expression> &get_expr() const {
                return expr;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Call : public Postfix {
            std::vector<std::shared_ptr<Argument>> args;

          public:
            Call(const std::shared_ptr<Token> &end, const std::shared_ptr<Expression> &caller,
                 const std::shared_ptr<Token> &safe, const std::vector<std::shared_ptr<Argument>> &args)
                : Postfix(end, caller, safe), args(args) {}

            const std::vector<std::shared_ptr<Argument>> &get_args() const {
                return args;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Reify : public Postfix {
            std::vector<std::shared_ptr<Type>> type_args;

          public:
            Reify(const std::shared_ptr<Token> &end, const std::shared_ptr<Expression> &caller,
                  const std::shared_ptr<Token> &safe, const std::vector<std::shared_ptr<Type>> &type_args)
                : Postfix(end, caller, safe), type_args(type_args) {}

            const std::vector<std::shared_ptr<Type>> &get_type_args() const {
                return type_args;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Slice : public AstNode {
          public:
            enum class Kind { INDEX, SLICE };

          private:
            Kind kind;
            std::shared_ptr<Expression> from;
            std::shared_ptr<Expression> to;
            std::shared_ptr<Expression> step;

          public:
            Slice(int line_start, int line_end, int col_start, int col_end, Kind kind, const std::shared_ptr<Expression> &from,
                  const std::shared_ptr<Expression> &to, const std::shared_ptr<Expression> &step)
                : AstNode(line_start, line_end, col_start, col_end), kind(kind), from(from), to(to), step(step) {}

            Kind get_kind() const {
                return kind;
            }

            const std::shared_ptr<Expression> &get_from() const {
                return from;
            }

            const std::shared_ptr<Expression> &get_to() const {
                return to;
            }

            const std::shared_ptr<Expression> &get_step() const {
                return step;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Index : public Postfix {
            std::vector<std::shared_ptr<Slice>> slices;

          public:
            Index(const std::shared_ptr<Token> &end, const std::shared_ptr<Expression> &caller,
                  const std::shared_ptr<Token> &safe, const std::vector<std::shared_ptr<Slice>> &slices)
                : Postfix(end, caller, safe), slices(slices) {}

            const std::vector<std::shared_ptr<Slice>> &get_slices() const {
                return slices;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Unary : public Expression {
            std::shared_ptr<Token> op;
            std::shared_ptr<Expression> expr;

          public:
            Unary(const std::shared_ptr<Token> &op, const std::shared_ptr<Expression> &expr)
                : Expression(op, expr), op(op), expr(expr) {}

            const std::shared_ptr<Token> &get_op() const {
                return op;
            }

            const std::shared_ptr<Expression> &get_expr() const {
                return expr;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Cast : public Expression {
            std::shared_ptr<Expression> expr;
            std::shared_ptr<Token> safe;
            std::shared_ptr<Type> type;

          public:
            Cast(const std::shared_ptr<Expression> &expr, const std::shared_ptr<Token> &safe, const std::shared_ptr<Type> &type)
                : Expression(expr, type), expr(expr), safe(safe), type(type) {}

            const std::shared_ptr<Expression> &get_expr() const {
                return expr;
            }

            const std::shared_ptr<Token> &get_safe() const {
                return safe;
            }

            const std::shared_ptr<Type> &get_type() const {
                return type;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Binary : public Expression {
          protected:
            std::shared_ptr<Expression> left;
            std::shared_ptr<Token> op1;
            std::shared_ptr<Token> op2;
            std::shared_ptr<Expression> right;

          public:
            Binary(const std::shared_ptr<Expression> &left, const std::shared_ptr<Token> &op,
                   const std::shared_ptr<Expression> &right)
                : Expression(left, right), left(left), op1(op), right(right) {}

            Binary(const std::shared_ptr<Expression> &left, const std::shared_ptr<Token> &op1,
                   const std::shared_ptr<Token> &op2, const std::shared_ptr<Expression> &right)
                : Expression(left, right), left(left), op1(op1), op2(op2), right(right) {}

            const std::shared_ptr<Expression> &get_left() const {
                return left;
            }

            const std::shared_ptr<Token> &get_op1() const {
                return op1;
            }

            const std::shared_ptr<Token> &get_op2() const {
                return op2;
            }

            const std::shared_ptr<Expression> &get_right() const {
                return right;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class ChainBinary : public Expression {
            std::vector<std::shared_ptr<Expression>> exprs;
            std::vector<std::shared_ptr<Token>> ops;

          public:
            ChainBinary(const std::vector<std::shared_ptr<Expression>> &exprs, const std::vector<std::shared_ptr<Token>> &ops)
                : Expression(exprs.front(), exprs.back()), exprs(exprs), ops(ops) {}

            const std::vector<std::shared_ptr<Expression>> &get_exprs() const {
                return exprs;
            }

            const std::vector<std::shared_ptr<Token>> &get_ops() const {
                return ops;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Ternary : public Expression {
            std::shared_ptr<Expression> condition;
            std::shared_ptr<Expression> on_true;
            std::shared_ptr<Expression> on_false;

          public:
            Ternary(const std::shared_ptr<Expression> &condition, const std::shared_ptr<Expression> &on_true,
                    const std::shared_ptr<Expression> &on_false)
                : Expression(condition, on_false), condition(condition), on_true(on_true), on_false(on_false) {}

            const std::shared_ptr<Expression> &get_condition() const {
                return condition;
            }

            const std::shared_ptr<Expression> &get_on_true() const {
                return on_true;
            }

            const std::shared_ptr<Expression> &get_on_false() const {
                return on_false;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Assignment : public Expression {
            std::vector<std::shared_ptr<Expression>> assignees;
            std::shared_ptr<Token> op1;
            std::shared_ptr<Token> op2;
            std::vector<std::shared_ptr<Expression>> exprs;

          public:
            Assignment(const std::vector<std::shared_ptr<Expression>> &assignees, const std::shared_ptr<Token> &op1,
                       const std::shared_ptr<Token> &op2, const std::vector<std::shared_ptr<Expression>> &exprs)
                : Expression(assignees.front(), exprs.back()), assignees(assignees), op1(op1), op2(op2), exprs(exprs) {}

            const std::vector<std::shared_ptr<Expression>> &get_assignees() const {
                return assignees;
            }

            const std::shared_ptr<Token> &get_op1() const {
                return op1;
            }

            const std::shared_ptr<Token> &get_op2() const {
                return op2;
            }

            const std::vector<std::shared_ptr<Expression>> &get_exprs() const {
                return exprs;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };
    }    // namespace expr

    class Statement : public AstNode {
      public:
        template<typename T1, typename T2>
            requires HasLineInfo<T1> && HasLineInfo<T2>
        Statement(T1 start, T2 end) : AstNode(start, end) {}
    };

    namespace stmt
    {
        class Block final : public Statement {
            std::vector<std::shared_ptr<Statement>> statements;

          public:
            Block(const std::shared_ptr<Token> &start, const std::shared_ptr<Token> &end,
                  const std::vector<std::shared_ptr<Statement>> &statements)
                : Statement(start, end), statements(statements) {}

            const std::vector<std::shared_ptr<Statement>> &get_statements() const {
                return statements;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class If final : public Statement {
            std::shared_ptr<Expression> condition;
            std::shared_ptr<Statement> body;
            std::shared_ptr<Statement> else_body;

          public:
            If(const std::shared_ptr<Token> &token, const std::shared_ptr<Expression> &condition,
               const std::shared_ptr<Statement> &body, const std::shared_ptr<Statement> &else_body)
                : Statement(token, else_body ? else_body : body), condition(condition), body(body), else_body(else_body) {}

            const std::shared_ptr<Expression> &get_condition() const {
                return condition;
            }

            const std::shared_ptr<Statement> &get_body() const {
                return body;
            }

            const std::shared_ptr<Statement> &get_else_body() const {
                return else_body;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class While final : public Statement {
            std::shared_ptr<Expression> condition;
            std::shared_ptr<Statement> body;
            std::shared_ptr<Statement> else_body;

          public:
            While(const std::shared_ptr<Token> &token, const std::shared_ptr<Expression> &condition,
                  const std::shared_ptr<Statement> &body, const std::shared_ptr<Statement> &else_body)
                : Statement(token, else_body ? else_body : body), condition(condition), body(body), else_body(else_body) {}

            const std::shared_ptr<Expression> &get_condition() const {
                return condition;
            }

            const std::shared_ptr<Statement> &get_body() const {
                return body;
            }

            const std::shared_ptr<Statement> &get_else_body() const {
                return else_body;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class DoWhile final : public Statement {
            std::shared_ptr<Statement> body;
            std::shared_ptr<Expression> condition;
            std::shared_ptr<Statement> else_body;

          public:
            DoWhile(const std::shared_ptr<Token> &token, const std::shared_ptr<Statement> &body,
                    const std::shared_ptr<Expression> &condition, const std::shared_ptr<Statement> &else_body)
                : Statement(token, else_body ? else_body : body), body(body), condition(condition), else_body(else_body) {}

            const std::shared_ptr<Expression> &get_condition() const {
                return condition;
            }

            const std::shared_ptr<Statement> &get_body() const {
                return body;
            }

            const std::shared_ptr<Statement> &get_else_body() const {
                return else_body;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Throw final : public Statement {
            std::shared_ptr<Expression> expression;

          public:
            Throw(const std::shared_ptr<Token> &token, const std::shared_ptr<Expression> &expression)
                : Statement(token, expression), expression(expression) {}

            const std::shared_ptr<Expression> &get_expression() const {
                return expression;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Catch final : public Statement {
            std::vector<std::shared_ptr<Reference>> references;
            std::shared_ptr<Token> symbol;
            std::shared_ptr<Statement> body;

          public:
            Catch(const std::shared_ptr<Token> &token, const std::vector<std::shared_ptr<Reference>> &references,
                  const std::shared_ptr<Token> &symbol, const std::shared_ptr<Statement> &body)
                : Statement(token, body), references(references), symbol(symbol), body(body) {}

            const std::vector<std::shared_ptr<Reference>> &get_references() const {
                return references;
            }

            const std::shared_ptr<Token> &get_symbol() const {
                return symbol;
            }

            const std::shared_ptr<Statement> &get_body() const {
                return body;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Try final : public Statement {
            std::shared_ptr<Statement> body;
            std::vector<std::shared_ptr<Statement>> catches;
            std::shared_ptr<Statement> finally;

          public:
            Try(const std::shared_ptr<Token> &token, const std::shared_ptr<Statement> &body,
                const std::vector<std::shared_ptr<Statement>> &catches, const std::shared_ptr<Statement> &finally)
                : Statement(token, finally ? finally : catches.back()), body(body), catches(catches), finally(finally) {}

            const std::shared_ptr<Statement> &get_body() const {
                return body;
            }

            const std::vector<std::shared_ptr<Statement>> &get_catches() const {
                return catches;
            }

            const std::shared_ptr<Statement> &get_finally() const {
                return finally;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Continue final : public Statement {
          public:
            Continue(const std::shared_ptr<Token> &token) : Statement(token, token) {}

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Break final : public Statement {
          public:
            Break(const std::shared_ptr<Token> &token) : Statement(token, token) {}

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Return final : public Statement {
            std::shared_ptr<Expression> expression;

          public:
            Return(const std::shared_ptr<Token> &token, const std::shared_ptr<Expression> &expression)
                : Statement(token, expression), expression(expression) {}

            const std::shared_ptr<Expression> &get_expression() const {
                return expression;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Yield final : public Statement {
            std::shared_ptr<Expression> expression;

          public:
            Yield(const std::shared_ptr<Token> &token, const std::shared_ptr<Expression> &expression)
                : Statement(token, expression), expression(expression) {}

            const std::shared_ptr<Expression> &get_expression() const {
                return expression;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Expr final : public Statement {
            std::shared_ptr<Expression> expression;

          public:
            explicit Expr(const std::shared_ptr<Expression> &expression)
                : Statement(expression, expression), expression(expression) {}

            const std::shared_ptr<Expression> &get_expression() const {
                return expression;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };
    }    // namespace stmt

    class Declaration : public AstNode {
        std::vector<std::shared_ptr<Token>> modifiers;

      public:
        Declaration(int line_start, int line_end, int col_start, int col_end)
            : AstNode(line_start, line_end, col_start, col_end) {}

        template<typename T1, typename T2>
            requires HasLineInfo<T1> && HasLineInfo<T2>
        Declaration(T1 start, T2 end) : AstNode(start, end) {}

        const std::vector<std::shared_ptr<Token>> &get_modifiers() const {
            return modifiers;
        }

        void set_modifiers(const std::vector<std::shared_ptr<Token>> &modifiers) {
            this->modifiers = modifiers;
        }
    };

    namespace decl
    {
        class TypeParam final : public Declaration {
            std::shared_ptr<Token> variance;
            std::shared_ptr<Token> name;
            std::shared_ptr<Type> default_type;

          public:
            template<typename T>
                requires HasLineInfo<T>
            TypeParam(const std::shared_ptr<Token> &variance, T end, const std::shared_ptr<Token> &name,
                      const std::shared_ptr<Type> &default_type)
                : Declaration(variance ? variance : name, end), variance(variance), name(name), default_type(default_type) {}

            const std::shared_ptr<Token> &get_variance() const {
                return variance;
            }

            const std::shared_ptr<Token> &get_name() const {
                return name;
            }

            const std::shared_ptr<Type> &get_default_type() const {
                return default_type;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Constraint final : public AstNode {
            std::shared_ptr<Token> arg;
            std::shared_ptr<Type> type;

          public:
            Constraint(const std::shared_ptr<Token> &arg, const std::shared_ptr<Type> &type)
                : AstNode(arg, type), arg(arg), type(type) {}

            const std::shared_ptr<Token> &get_arg() const {
                return arg;
            }

            const std::shared_ptr<Type> &get_type() const {
                return type;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Param final : public Declaration {
            std::shared_ptr<Token> is_const;
            std::shared_ptr<Token> variadic;
            std::shared_ptr<Token> name;
            std::shared_ptr<Type> type;
            std::shared_ptr<Expression> default_expr;

          public:
            Param(int line_start, int line_end, int col_start, int col_end, const std::shared_ptr<Token> &is_const,
                  const std::shared_ptr<Token> &variadic, const std::shared_ptr<Token> &name, const std::shared_ptr<Type> &type,
                  const std::shared_ptr<Expression> &default_expr)
                : Declaration(line_start, line_end, col_start, col_end),
                  is_const(is_const),
                  variadic(variadic),
                  name(name),
                  type(type),
                  default_expr(default_expr) {}

            const std::shared_ptr<Token> &get_is_const() const {
                return is_const;
            }

            const std::shared_ptr<Token> &get_variadic() const {
                return variadic;
            }

            const std::shared_ptr<Token> &get_name() const {
                return name;
            }

            const std::shared_ptr<Type> &get_type() const {
                return type;
            }

            const std::shared_ptr<Expression> &get_default_expr() const {
                return default_expr;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Params final : public Declaration {
            std::vector<std::shared_ptr<Param>> pos_only;
            std::vector<std::shared_ptr<Param>> pos_kwd;
            std::vector<std::shared_ptr<Param>> kwd_only;

          public:
            template<typename T1, typename T2>
                requires HasLineInfo<T1> && HasLineInfo<T2>
            Params(T1 start, T2 end, const std::vector<std::shared_ptr<Param>> &pos_only,
                   const std::vector<std::shared_ptr<Param>> &pos_kwd, const std::vector<std::shared_ptr<Param>> &kwd_only)
                : Declaration(start, end), pos_only(pos_only), pos_kwd(pos_kwd), kwd_only(kwd_only) {}

            const std::vector<std::shared_ptr<Param>> &get_pos_only() const {
                return pos_only;
            }

            const std::vector<std::shared_ptr<Param>> &get_pos_kwd() const {
                return pos_kwd;
            }

            const std::vector<std::shared_ptr<Param>> &get_kwd_only() const {
                return kwd_only;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Function final : public Declaration {
            std::shared_ptr<Token> name;
            std::vector<std::shared_ptr<TypeParam>> type_params;
            std::vector<std::shared_ptr<Constraint>> constraints;
            std::shared_ptr<Params> params;
            std::shared_ptr<Type> return_type;
            std::shared_ptr<Statement> definition;

            // Analyzer specific
            string qualified_name;

          public:
            template<typename T>
                requires HasLineInfo<T>
            Function(const std::shared_ptr<Token> &token, T end, const std::shared_ptr<Token> &name,
                     const std::vector<std::shared_ptr<TypeParam>> &type_params,
                     const std::vector<std::shared_ptr<Constraint>> &constraints, const std::shared_ptr<Params> &params,
                     const std::shared_ptr<Type> &return_type, const std::shared_ptr<Statement> &definition)
                : Declaration(token, end),
                  name(name),
                  type_params(type_params),
                  constraints(constraints),
                  params(params),
                  return_type(return_type),
                  definition(definition) {}

            const std::shared_ptr<Token> &get_name() const {
                return name;
            }

            const std::vector<std::shared_ptr<TypeParam>> &get_type_params() const {
                return type_params;
            }

            const std::vector<std::shared_ptr<Constraint>> &get_constraints() const {
                return constraints;
            }

            const std::shared_ptr<Params> &get_params() const {
                return params;
            }

            const std::shared_ptr<Type> &get_return_type() const {
                return return_type;
            }

            const std::shared_ptr<Statement> &get_definition() const {
                return definition;
            }

            const string &get_qualified_name() const {
                return qualified_name;
            }

            void set_qualified_name(const string &qualified_name) {
                this->qualified_name = qualified_name;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Variable final : public Declaration {
            std::shared_ptr<Token> token;
            std::shared_ptr<Token> name;
            std::shared_ptr<Type> type;
            std::shared_ptr<Expression> expr;

          public:
            template<typename T>
                requires HasLineInfo<T>
            Variable(const std::shared_ptr<Token> &token, T end, const std::shared_ptr<Token> &name,
                     const std::shared_ptr<Type> &type, const std::shared_ptr<Expression> &expr)
                : Declaration(token, end), token(token), name(name), type(type), expr(expr) {}

            const std::shared_ptr<Token> &get_token() const {
                return token;
            }

            const std::shared_ptr<Token> &get_name() const {
                return name;
            }

            const std::shared_ptr<Type> &get_type() const {
                return type;
            }

            const std::shared_ptr<Expression> &get_expr() const {
                return expr;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Parent final : public AstNode {
            std::shared_ptr<Reference> reference;
            std::vector<std::shared_ptr<Type>> type_args;

          public:
            template<typename T>
                requires HasLineInfo<T>
            Parent(T end, std::shared_ptr<Reference> reference, const std::vector<std::shared_ptr<Type>> &type_args)
                : AstNode(reference, end), reference(reference), type_args(type_args) {}

            const std::shared_ptr<Reference> &get_reference() const {
                return reference;
            }

            const std::vector<std::shared_ptr<Type>> &get_type_args() const {
                return type_args;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Enumerator final : public Declaration {
            std::shared_ptr<Token> name;
            std::shared_ptr<Expression> expr;
            std::optional<std::vector<std::shared_ptr<expr::Argument>>> args;

          public:
            explicit Enumerator(const std::shared_ptr<Token> &name) : Declaration(name, name), name(name) {}

            Enumerator(const std::shared_ptr<Token> &name, const std::shared_ptr<Expression> &expr)
                : Declaration(name, expr), name(name), expr(expr) {}

            template<typename T>
                requires HasLineInfo<T>
            Enumerator(T end, std::shared_ptr<Token> name, const std::vector<std::shared_ptr<expr::Argument>> &args)
                : Declaration(name, end), name(name), args(args) {}

            const std::shared_ptr<Token> &get_name() const {
                return name;
            }

            const std::shared_ptr<Expression> &get_expr() const {
                return expr;
            }

            const std::optional<std::vector<std::shared_ptr<expr::Argument>>> &get_args() const {
                return args;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };

        class Compound final : public Declaration {
            std::shared_ptr<Token> token;
            std::shared_ptr<Token> name;
            std::vector<std::shared_ptr<TypeParam>> type_params;
            std::vector<std::shared_ptr<Constraint>> constraints;
            std::vector<std::shared_ptr<Parent>> parents;
            std::vector<std::shared_ptr<Enumerator>> enumerators;
            std::vector<std::shared_ptr<Declaration>> members;

          public:
            template<typename T>
                requires HasLineInfo<T>
            Compound(std::shared_ptr<Token> token, T end, const std::shared_ptr<Token> &name,
                     const std::vector<std::shared_ptr<TypeParam>> &type_params,
                     const std::vector<std::shared_ptr<Constraint>> &constraints,
                     const std::vector<std::shared_ptr<Parent>> &parents,
                     const std::vector<std::shared_ptr<Enumerator>> &enumerators,
                     const std::vector<std::shared_ptr<Declaration>> &members)
                : Declaration(token, end),
                  token(token),
                  name(name),
                  type_params(type_params),
                  constraints(constraints),
                  parents(parents),
                  enumerators(enumerators),
                  members(members) {}

            const std::shared_ptr<Token> &get_token() const {
                return token;
            }

            const std::shared_ptr<Token> &get_name() const {
                return name;
            }

            const std::vector<std::shared_ptr<TypeParam>> &get_type_params() const {
                return type_params;
            }

            const std::vector<std::shared_ptr<Constraint>> &get_constraints() const {
                return constraints;
            }

            const std::vector<std::shared_ptr<Parent>> &get_parents() const {
                return parents;
            }

            const std::vector<std::shared_ptr<Enumerator>> &get_enumerators() {
                return enumerators;
            }

            const std::vector<std::shared_ptr<Declaration>> &get_members() const {
                return members;
            }

            void accept(VisitorBase *visitor) override {
                visitor->visit(*this);
            }
        };
    }    // namespace decl

    class stmt::Declaration final : public Statement {
        std::shared_ptr<ast::Declaration> declaration;

      public:
        explicit Declaration(const std::shared_ptr<ast::Declaration> &declaration)
            : Statement(declaration, declaration), declaration(declaration) {}

        std::shared_ptr<ast::Declaration> get_declaration() const {
            return declaration;
        }

        void accept(VisitorBase *visitor) override {
            visitor->visit(*this);
        }
    };

    class Import final : public AstNode {
        string path;
        std::shared_ptr<Token> name;
        std::shared_ptr<Token> alias;

        // Analyzer specific
        std::weak_ptr<Module> module;

      public:
        template<typename T1, typename T2>
            requires HasLineInfo<T1> && HasLineInfo<T2>
        Import(T1 start, T2 end, const string &path, const std::shared_ptr<Token> &name, const std::shared_ptr<Token> &alias)
            : AstNode(start, end), path(path), name(name), alias(alias) {}

        const string &get_path() const {
            return path;
        }

        fs::path get_path(const fs::path &root_path, const std::shared_ptr<Module> &module) const;

        const std::shared_ptr<Token> &get_name() const {
            return name;
        }

        const std::shared_ptr<Token> &get_alias() const {
            return alias;
        }

        std::weak_ptr<Module> get_module() const {
            return module;
        }

        void set_module(const std::weak_ptr<Module> &module) {
            this->module = module;
        }

        void accept(VisitorBase *visitor) override {
            visitor->visit(*this);
        }
    };

    class Module : public AstNode {
      protected:
        fs::path file_path;
        std::vector<std::shared_ptr<Import>> imports;
        std::vector<std::shared_ptr<Declaration>> members;

        Module(const fs::path &path) : AstNode(-1, -1, -1, -1), file_path(path) {}

      public:
        template<typename T1, typename T2>
            requires HasLineInfo<T1> && HasLineInfo<T2>
        Module(T1 start, T2 end, const std::vector<std::shared_ptr<Import>> &imports,
               const std::vector<std::shared_ptr<Declaration>> &members, const fs::path &file_path)
            : AstNode(start, end), file_path(file_path), imports(imports), members(members) {}

        string get_name() const {
            return file_path.stem().string();
        }

        const fs::path &get_file_path() const {
            return file_path;
        }

        const std::vector<std::shared_ptr<Import>> &get_imports() const {
            return imports;
        }

        const std::vector<std::shared_ptr<Declaration>> &get_members() const {
            return members;
        }

        void accept(VisitorBase *visitor) override {
            visitor->visit(*this);
        }
    };

    class FolderModule final : public Module {
      public:
        FolderModule(const fs::path &path) : Module(path) {}

        void accept(VisitorBase *visitor) override {
            visitor->visit(*this);
        }
    };
}    // namespace spade::ast

namespace spade
{
    template<ast::HasLineInfo T>
    class LineInfoVector {
        int line_start = -1;
        int line_end = -1;
        int col_start = -1;
        int col_end = -1;

      public:
        explicit LineInfoVector(const std::vector<T> &items)
            : line_start(items.empty() ? -1 : items.front()->get_line_start()),
              line_end(items.empty() ? -1 : items.back()->get_line_end()),
              col_start(items.empty() ? -1 : items.front()->get_col_start()),
              col_end(items.empty() ? -1 : items.back()->get_col_end()) {}

        LineInfoVector() = default;
        LineInfoVector(const LineInfoVector &other) = default;
        LineInfoVector(LineInfoVector &&other) noexcept = default;
        LineInfoVector &operator=(const LineInfoVector &other) = default;
        LineInfoVector &operator=(LineInfoVector &&other) noexcept = default;
        ~LineInfoVector() = default;

        LineInfoVector &operator*() {
            return *this;
        }

        const LineInfoVector &operator*() const {
            return *this;
        }

        LineInfoVector *operator->() {
            return this;
        }

        const LineInfoVector *operator->() const {
            return this;
        }

        constexpr operator bool() const {
            return true;
        }

        int get_line_start() const {
            return line_start;
        }

        int get_line_end() const {
            return line_end;
        }

        int get_col_start() const {
            return col_start;
        }

        int get_col_end() const {
            return col_end;
        }
    };

    static_assert(ast::HasLineInfo<LineInfoVector<ast::AstNode *>>,
                  "spade::LineInfoVector must satisfy spade::ast::HasLineInfo concept");
}    // namespace spade