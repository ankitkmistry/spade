#pragma once

#include "ast.hpp"
#include "lexer/lexer.hpp"

namespace spade
{
    class Parser final {
        static constexpr int FILL_CONSTANT = 64;

      private:
        fs::path file_path;
        Lexer *lexer;
        std::vector<std::shared_ptr<Token>> tokens;
        int index = 0;

        void fill_tokens_buffer(int n);

        template<typename... Ts>
            requires(std::same_as<TokenType, Ts> && ...)
        static string make_expected_string(Ts... types) {
            std::vector<TokenType> list{types...};
            string result;
            for (int i = 0; i < list.size(); ++i) {
                result += TokenInfo::get_repr(list[i]);
                if (i < list.size() - 1)
                    result += ", ";
            }
            return result;
        }

        std::shared_ptr<Token> current();
        std::shared_ptr<Token> peek(int i = 0);
        std::shared_ptr<Token> advance();

        template<typename... T>
            requires(std::same_as<TokenType, T> && ...)
        std::shared_ptr<Token> match(T... types) {
            const std::vector<TokenType> ts = {types...};
            for (auto t: ts) {
                if (peek()->get_type() == t)
                    return advance();
            }
            return null;
        }

        std::shared_ptr<Token> match(const string &text) {
            if (peek()->get_text() == text)
                return advance();
            return null;
        }

        template<typename... T>
            requires(std::same_as<TokenType, T> && ...)
        std::shared_ptr<Token> expect(T... types) {
            const std::vector<TokenType> ts = {types...};
            for (auto t: ts) {
                if (peek()->get_type() == t)
                    return advance();
            }
            throw error(std::format("expected {}", make_expected_string(types...)));
        }

        ParserError error(const string &msg, const std::shared_ptr<Token> &token);
        ParserError error(const string &msg);

        template<typename R, typename C1, typename C2>
        std::shared_ptr<R> rule_or(std::function<std::shared_ptr<C1>()> rule1, std::function<std::shared_ptr<C2>()> rule2) {
            int tok_idx = index;
            try {
                return spade::cast<R>(rule1());
            } catch (const ParserError &) {
                index = tok_idx;
                return spade::cast<R>(rule2());
            }
        }

        template<typename T>
        std::shared_ptr<T> rule_or(std::function<std::shared_ptr<T>()> rule1, std::function<std::shared_ptr<T>()> rule2) {
            int tok_idx = index;
            try {
                return rule1();
            } catch (const ParserError &) {
                index = tok_idx;
                return rule2();
            }
        }

        template<typename T>
        std::shared_ptr<T> rule_optional(std::shared_ptr<T> (Parser::*rule)()) {
            int tok_idx = index;
            try {
                return (this->*rule)();
            } catch (const ParserError &) {
                index = tok_idx;
                return null;
            }
        }

        // template<typename R, typename C1, typename C2>
        // std::shared_ptr<R> rule_or(std::shared_ptr<C1> (Parser::*rule1)(), std::shared_ptr<C2> (Parser::*rule2)()) {
        //     int tok_idx = index;
        //     try {
        //         return spade::cast<R>(this->*rule1());
        //     } catch (const ParserError &) {
        //         index = tok_idx;
        //         return spade::cast<R>(this->*rule2());
        //     }
        // }

        template<typename T>
        std::shared_ptr<T> rule_or(std::shared_ptr<T> (Parser::*rule1)(), std::shared_ptr<T> (Parser::*rule2)()) {
            int tok_idx = index;
            try {
                return (this->*rule1)();
            } catch (const ParserError &) {
                index = tok_idx;
                return (this->*rule2)();
            }
        }

        // Parser rules
        /// module ::= import* declaration* END_OF_FILE
        std::shared_ptr<ast::Module> module();
        /// import ::= 'import' ('..' | '.')? reference ('.' '*' | 'as' IDENTIFIER)?
        std::shared_ptr<ast::Import> import();
        /// reference ::= IDENTIFIER ('.' IDENTIFIER)*
        std::shared_ptr<ast::Reference> reference();

        // Declarations
        /// declaration ::= variable_decl | function_decl | compound_decl
        std::shared_ptr<ast::Declaration> declaration();
        /// compound_decl ::= ('class' | 'enum' | 'interface' | 'annotation') IDENTIFIER
        ///                     ('[' type_param_list ']' set context_generics on)?
        ///                     (':' parent_list)?
        ///                     (if context_generics then "where" constraint_list)?
        ///                     ('{' enumerator_list? member_decl* '}')?
        std::shared_ptr<ast::Declaration> compound_decl();
        /// member_decl ::= variable_decl | function_decl | init_decl | compound_decl
        std::shared_ptr<ast::Declaration> member_decl();
        /// init_decl ::= 'init' '(' params ')' ('=' statement | block)
        std::shared_ptr<ast::Declaration> init_decl();
        /// variable_decl ::= ('var' | 'const') IDENTIFIER (':' type)? ('=' expression)?
        std::shared_ptr<ast::Declaration> variable_decl();
        /// function_decl ::= 'fun' IDENTIFIER
        ///                     ('[' type_param_list ']' set context_generics on)?
        ///                     '(' params? ')' ('->' type)?
        ///                     (if context_generics "where" constraint_list)?
        ///                     ('=' statement | block)?
        std::shared_ptr<ast::Declaration> function_decl();

        /// modifiers ::= ('abstract' | 'final' | 'static' | 'override' | 'private' | 'internal' | 'protected' | 'public')*
        std::vector<std::shared_ptr<Token>> modifiers();
        /// type_param ::= ('out' | 'in') IDENTIFIER ('=' type)?
        std::shared_ptr<ast::decl::TypeParam> type_param();
        /// constraint ::= IDENTIFIER ':' type
        std::shared_ptr<ast::decl::Constraint> constraint();
        /// parent ::= reference ('[' type_list ']')?
        std::shared_ptr<ast::decl::Parent> parent();
        /// enumerator ::= IDENTIFIER ('=' expression | '(' argument_list ')')?
        std::shared_ptr<ast::decl::Enumerator> enumerator();
        /// params ::= param_list ((if last_token != ',' then ',') '*' ',' param_list)? ((if last_token !=',' then ',') '/' ',' param_list)?
        std::shared_ptr<ast::decl::Params> params();
        /// param ::= 'const'? '*'? IDENTIFIER (':' type)? ('=' ternary)?
        std::shared_ptr<ast::decl::Param> param();

        // Statements
        /// statements ::= block | statement
        std::shared_ptr<ast::Statement> statements();
        /// block ::= '{' (block | declaration | statement)* '}'
        std::shared_ptr<ast::Statement> block();
        /// statement ::= if_stmt | while_stmt | do_while_stmt | try_stmt
        ///             | 'continue' | 'break'
        //              | 'throw' expression
        //              | 'return' expression?
        //              | 'yield' expression
        std::shared_ptr<ast::Statement> statement();
        /// if_stmt ::= 'if' expression (':' statement | block) ('else' (':' statement | block))?
        std::shared_ptr<ast::Statement> if_stmt();
        /// while_stmt ::= 'while' expression (':' statement | block) ('else' (':' statement | block))?
        std::shared_ptr<ast::Statement> while_stmt();
        /// do_while_stmt ::= 'do' block 'while' expression ('else' (':' statement | block))?
        std::shared_ptr<ast::Statement> do_while_stmt();
        /// try_stmt ::= 'try' (':' statement | block) (finally_stmt | catch_stmt+ finally_stmt?)
        /// finally_stmt ::= 'finally' (':' statement | block)
        std::shared_ptr<ast::Statement> try_stmt();
        /// catch_stmt ::= 'catch' reference_list ('as' IDENTIFIER)? (':' statement | block)
        std::shared_ptr<ast::Statement> catch_stmt();

        // Expressions
        /// expression ::= assignment | ternary
        std::shared_ptr<ast::Expression> expression();
        /// assignment ::= assignee_list ('+' | '-' | '*' | '/' | '%' | '**' | '<<' | '>>' | '>>>' | '&' | '|' | '^' | '??') '=' expr_list
        std::shared_ptr<ast::Expression> assignment();
        /// ternary ::= logic_or ('if' logic_or 'else' logic_or)?
        std::shared_ptr<ast::Expression> ternary();

        // Binary
        /// logic_or ::= logic_and ('or' logic_and)*
        std::shared_ptr<ast::Expression> logic_or();
        /// logic_and ::= logic_not ('and' logic_not)*
        std::shared_ptr<ast::Expression> logic_and();
        /// logic_not ::= 'not'* conditional
        std::shared_ptr<ast::Expression> logic_not();
        /// conditional ::= relational (('is' 'not'? | 'not'? 'in') relational)*
        std::shared_ptr<ast::Expression> conditional();
        /// relational ::= bit_or (('<' | '<=' | '==' | '!=' | '>=' | '>') bit_or)*
        std::shared_ptr<ast::Expression> relational();
        /// bit_or ::= bit_xor ('|' bit_xor)*
        std::shared_ptr<ast::Expression> bit_or();
        /// bit_xor ::= bit_and ('^' bit_and)*
        std::shared_ptr<ast::Expression> bit_xor();
        /// bit_and ::= shift ('&' shift)*
        std::shared_ptr<ast::Expression> bit_and();
        /// shift ::= term (('<<' | '>>' | '>>>') term)*
        std::shared_ptr<ast::Expression> shift();
        /// term ::= factor (('+' | '-') factor)*
        std::shared_ptr<ast::Expression> term();
        /// factor ::= power (('*' | '/' | '%') power)*
        std::shared_ptr<ast::Expression> factor();
        /// power ::= (cast '**')* cast
        std::shared_ptr<ast::Expression> power();
        /// cast ::= elvis ('as' type)*
        std::shared_ptr<ast::Expression> cast();
        /// elvis ::= unary ('??' unary)*
        std::shared_ptr<ast::Expression> elvis();

        // Unary
        /// unary ::= ('~' | '-' | '+')? postfix
        std::shared_ptr<ast::Expression> unary();

        // Postfix
        /// postfix ::= primary
        ///                 ('?'? '.' (IDENTIFIER | 'init')     # dot_access or safe_dot_access
        ///               | '(' argument_list? ')'              # call
        ///               | '[' (slice_list | type_list) ']')?  # indexer or reify
        std::shared_ptr<ast::Expression> postfix();
        /// argument ::= (IDENTIFIER ':')? expression
        std::shared_ptr<ast::expr::Argument> argument();
        /// slice ::= expression
        ///         | expression? ':' expression? (':' expression?)?
        std::shared_ptr<ast::expr::Slice> slice();

        // Primary
        /// primary ::= 'true' | 'false' | 'null'
        ///           | INTEGER | FLOAT | STRING | IDENTIFIER
        ///           | 'super' | 'self'
        ///           | '(' expression ')'
        std::shared_ptr<ast::Expression> primary();

        // Type expressions
        /// type ::= union_type
        std::shared_ptr<ast::Type> type();
        /// union_type ::= intersection_type ('|' intersection_type)*
        std::shared_ptr<ast::Type> union_type();
        /// intersection_type ::= nullable_type ('&' nullable_type)*
        std::shared_ptr<ast::Type> intersection_type();
        /// nullable_type ::= primary_type '?'?
        std::shared_ptr<ast::Type> nullable_type();
        /// primary_type ::= reference ('[' type_list? ']')?                # reference_type
        ///                | 'type'                                         # type_literal
        ///                | '(' type_list? ')' '->' type                   # function_type
        ///                | object ('{' type_builder_member_list ? '}')?   # object_builder_type
        ///                | '(' type ')'                                   # grouped_type
        std::shared_ptr<ast::Type> primary_type();
        /// type_builder_member ::= (IDENTIFER | 'init') (':' type)?
        std::shared_ptr<ast::type::TypeBuilderMember> type_builder_member();

        // Comma separated lists
        /// type_list ::= type (',' type)* ','?
        std::vector<std::shared_ptr<ast::Type>> type_list();
        /// assignee_list ::= assignee (',' assignee)* ','?
        std::vector<std::shared_ptr<ast::Expression>> assignee_list();
        /// expr_list ::= expression (',' expression)* ','?
        std::vector<std::shared_ptr<ast::Expression>> expr_list();
        /// argument_list ::= argument (',' argument)* ','?
        std::vector<std::shared_ptr<ast::expr::Argument>> argument_list();
        /// slice_list ::= slice (',' slice)* ','?
        std::vector<std::shared_ptr<ast::expr::Slice>> slice_list();
        /// reference_list ::= reference (',' reference)* ','?
        std::vector<std::shared_ptr<ast::Reference>> reference_list();
        /// param_list ::= (param (',' param)* ','?)?
        std::vector<std::shared_ptr<ast::decl::Param>> param_list();
        /// type_param_list ::= type_param (',' type_param)* ','?
        std::vector<std::shared_ptr<ast::decl::TypeParam>> type_param_list();
        /// constraint_list ::= constraint (',' constraint)* ','?
        std::vector<std::shared_ptr<ast::decl::Constraint>> constraint_list();
        /// parent_list ::= parent (',' parent)* ','?
        std::vector<std::shared_ptr<ast::decl::Parent>> parent_list();
        /// enumerator_list ::= enumerator (',' enumerator)* ','?
        std::vector<std::shared_ptr<ast::decl::Enumerator>> enumerator_list();
        /// type_builder_member_list ::= type_builder_member (',' type_builder_member)* ','?
        std::vector<std::shared_ptr<ast::type::TypeBuilderMember>> type_builder_member_list();

      public:
        /**
         * Construct a new Parser object
         * @param file_path the absolute path to the file
         * @param lexer pointer to the token source
         */
        explicit Parser(const fs::path &file_path, Lexer *lexer) : file_path(file_path), lexer(lexer) {
            assert(file_path.is_absolute());
        }

        Parser(const Parser &other) = default;
        Parser(Parser &&other) noexcept = default;
        Parser &operator=(const Parser &other) = default;
        Parser &operator=(Parser &&other) noexcept = default;
        ~Parser() = default;

        std::shared_ptr<ast::Module> parse();

        Lexer *get_lexer() const {
            return lexer;
        }
    };
}    // namespace spade