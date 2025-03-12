#include <algorithm>
#include <iostream>
#include <numeric>
#include <stack>
#include <stdexcept>
#include <unordered_map>

#include "analyzer.hpp"
#include "lexer/token.hpp"
#include "parser/ast.hpp"
#include "spimp/error.hpp"
#include "utils/error.hpp"

namespace spade
{
    class SymbolTableBuilder : public ast::VisitorBase {
        fs::path root_path;
        std::unordered_map<SymbolPath, ast::AstNode *> symbols;
        std::vector<std::shared_ptr<ast::Module>> modules;
        std::stack<SymbolPath> path_stack;

      public:
        SymbolTableBuilder(const std::vector<std::shared_ptr<ast::Module>> &modules) : modules(modules) {
            if (!modules.empty()) {
                root_path = modules[0]->get_file_path().parent_path();
                for (const auto &module: modules) {
                    fs::path path = module->get_file_path();
                    fs::path current_path = path.parent_path();
                    fs::path temp_path;

                    auto common_it = root_path.begin();
                    auto current_it = current_path.begin();

                    for (; common_it != root_path.end() && current_it != current_path.end() && *common_it == *current_it;
                         ++common_it, ++current_it) {
                        temp_path /= *common_it;
                    }
                    root_path = temp_path;
                }
            }
        }

        SymbolTableBuilder(const SymbolTableBuilder &other) = default;
        SymbolTableBuilder(SymbolTableBuilder &&other) noexcept = default;
        SymbolTableBuilder &operator=(const SymbolTableBuilder &other) = default;
        SymbolTableBuilder &operator=(SymbolTableBuilder &&other) noexcept = default;
        ~SymbolTableBuilder() override = default;

        void visit(ast::Reference &node) override {}

        void visit(ast::type::Reference &node) override {}

        void visit(ast::type::Function &node) override {}

        void visit(ast::type::TypeLiteral &node) override {}

        void visit(ast::type::TypeOf &node) override {}

        void visit(ast::type::BinaryOp &node) override {}

        void visit(ast::type::Nullable &node) override {}

        void visit(ast::type::TypeBuilder &node) override {}

        void visit(ast::type::TypeBuilderMember &node) override {}

        void visit(ast::expr::Constant &node) override {}

        void visit(ast::expr::Super &node) override {}

        void visit(ast::expr::Self &node) override {}

        void visit(ast::expr::DotAccess &node) override {}

        void visit(ast::expr::Call &node) override {}

        void visit(ast::expr::Argument &node) override {}

        void visit(ast::expr::Reify &node) override {}

        void visit(ast::expr::Index &node) override {}

        void visit(ast::expr::Slice &node) override {}

        void visit(ast::expr::Unary &node) override {}

        void visit(ast::expr::Cast &node) override {}

        void visit(ast::expr::Binary &node) override {}

        void visit(ast::expr::ChainBinary &node) override {}

        void visit(ast::expr::Ternary &node) override {}

        void visit(ast::expr::Assignment &node) override {}

        void visit(ast::stmt::Block &node) override {}

        void visit(ast::stmt::If &node) override {}

        void visit(ast::stmt::While &node) override {}

        void visit(ast::stmt::DoWhile &node) override {}

        void visit(ast::stmt::Throw &node) override {}

        void visit(ast::stmt::Catch &node) override {}

        void visit(ast::stmt::Try &node) override {}

        void visit(ast::stmt::Continue &node) override {}

        void visit(ast::stmt::Break &node) override {}

        void visit(ast::stmt::Return &node) override {}

        void visit(ast::stmt::Yield &node) override {}

        void visit(ast::stmt::Expr &node) override {}

        void visit(ast::stmt::Declaration &node) override {}

        void visit(ast::decl::TypeParam &node) override {}

        void visit(ast::decl::Constraint &node) override {}

        void visit(ast::decl::Param &node) override {}

        void visit(ast::decl::Params &node) override {}

        void check_modifiers(ast::AstNode *node, const std::vector<std::shared_ptr<Token>> &modifiers) {
            std::unordered_map<TokenType, size_t> modifier_counts = {
                    {TokenType::ABSTRACT,  0},
                    {TokenType::FINAL,     0},
                    {TokenType::STATIC,    0},
                    {TokenType::OVERRIDE,  0},
                    {TokenType::PRIVATE,   0},
                    {TokenType::INTERNAL,  0},
                    {TokenType::PROTECTED, 0},
                    {TokenType::PUBLIC,    0},
            };
            // Get the count of each modifier
            for (const auto &modifier: modifiers) {
                try {
                    modifier_counts.at(modifier->get_type())++;
                } catch (const std::out_of_range &) {
                    throw Unreachable();    // surely some parser error
                }
            }
            // Check for duplicate modifiers
            for (const auto &[modifier, count]: modifier_counts) {
                if (count > 1) {
                    throw AnalyzerError(std::format("duplicate modifier: {}", TokenInfo::get_repr(modifier)), node);
                }
            }
            // Check for conflicting modifiers
            const auto CHECK_EXCLUSIVE = [&](TokenType a, TokenType b) {
                if (modifier_counts[a] + modifier_counts[b] > 1) {
                    throw AnalyzerError(
                            std::format("{} and {} are mutually exclusive", TokenInfo::get_repr(a), TokenInfo::get_repr(b)),
                            node);
                }
            };

            CHECK_EXCLUSIVE(TokenType::ABSTRACT, TokenType::FINAL);
            CHECK_EXCLUSIVE(TokenType::STATIC, TokenType::OVERRIDE);
            CHECK_EXCLUSIVE(TokenType::ABSTRACT, TokenType::PRIVATE);
            CHECK_EXCLUSIVE(TokenType::FINAL, TokenType::PRIVATE);
            CHECK_EXCLUSIVE(TokenType::OVERRIDE, TokenType::PRIVATE);

            if (modifier_counts[TokenType::PRIVATE] + modifier_counts[TokenType::PROTECTED] +
                        modifier_counts[TokenType::INTERNAL] + modifier_counts[TokenType::PUBLIC] >
                1) {
                throw AnalyzerError("access modifiers are mutually exclusive", node);
            }

            if (modifier_counts[TokenType::ABSTRACT] + modifier_counts[TokenType::STATIC] > 1) {
                try {
                    if (cast<ast::decl::Compound>(node)->get_token()->get_type() == TokenType::CLASS &&
                        cast<ast::decl::Compound>(symbols.at(path_stack.top()))->get_token()->get_type() == TokenType::CLASS) {
                    } else
                        throw AnalyzerError("'abstract' and 'static' are mutually exclusive", node);
                } catch (const CastError &) {
                    throw AnalyzerError("'abstract' and 'static' are mutually exclusive", node);
                } catch (const std::out_of_range &) {
                    throw Unreachable();    // not a folder module expected a file module
                }
            }

            // Module level declaration specific checks
            try {
                if (is<ast::Module>(symbols.at(path_stack.top()))) {
                    if (modifier_counts[TokenType::STATIC] > 0)
                        throw AnalyzerError("module level declarations cannot be 'static'", node);
                    else if (modifier_counts[TokenType::OVERRIDE] > 0)
                        throw AnalyzerError("module level declarations cannot be 'override'", node);

                    if (is<ast::decl::Function>(node)) {
                        if (modifier_counts[TokenType::ABSTRACT] > 0)
                            throw AnalyzerError("global functions cannot be 'abstract'", node);
                    }
                }
            } catch (const std::out_of_range &) {
                throw Unreachable();    // not a folder module expected a file module
            }
            // Variable specific checks
            if (is<ast::decl::Variable>(node)) {
                if (modifier_counts[TokenType::ABSTRACT] > 0)
                    throw AnalyzerError("variables/constants cannot be 'abstract'", node);
                else if (modifier_counts[TokenType::FINAL] > 0)
                    throw AnalyzerError("variables/constants cannot be 'final'", node);
                else if (modifier_counts[TokenType::OVERRIDE] > 0)
                    throw AnalyzerError("variables/constants cannot be 'override'", node);
            }
            // Compound declaration specific checks
            try {
                switch (cast<ast::decl::Compound>(node)->get_token()->get_type()) {
                    case TokenType::CLASS:
                        break;
                    case TokenType::ENUM:
                        if (modifier_counts[TokenType::ABSTRACT] > 0)
                            throw AnalyzerError("enums cannot be 'abstract'", node);
                        break;
                    case TokenType::INTERFACE:
                        if (modifier_counts[TokenType::ABSTRACT] > 0)
                            throw AnalyzerError("interfaces cannot be 'abstract'", node);
                        break;
                    case TokenType::ANNOTATION:
                        if (modifier_counts[TokenType::ABSTRACT] > 0)
                            throw AnalyzerError("annotations cannot be 'abstract'", node);
                        break;
                    default:
                        throw Unreachable();    // surely some parser error
                }
            } catch (const CastError &) {
            } catch (const std::out_of_range &) {
                throw Unreachable();    // not a folder module expected a file module
            }
            // Parent class/compound specific checks
            try {
                switch (cast<ast::decl::Compound>(symbols.at(path_stack.top()))->get_token()->get_type()) {
                    case TokenType::CLASS:
                        break;
                    case TokenType::ENUM:
                        if (modifier_counts[TokenType::ABSTRACT] > 0)
                            throw AnalyzerError("'abstract' members are not allowed in enums", node);
                        else if (modifier_counts[TokenType::FINAL] > 0)
                            throw AnalyzerError("'final' members are not allowed in enums", node);
                        break;
                    case TokenType::INTERFACE:
                        if (modifier_counts[TokenType::ABSTRACT] > 0)
                            throw AnalyzerError("'abstract' members are not allowed in interfaces", node);
                        else if (modifier_counts[TokenType::FINAL] > 0 && modifier_counts[TokenType::STATIC] == 0)
                            throw AnalyzerError("'final' members are not allowed in interfaces (but final static is allowed)",
                                                node);
                        break;
                    case TokenType::ANNOTATION:
                        if (modifier_counts[TokenType::ABSTRACT] > 0)
                            throw AnalyzerError("'abstract' members are not allowed in annotations", node);
                        break;
                    default:
                        throw Unreachable();    // surely some parser error
                }
            } catch (const CastError &) {
            } catch (const std::out_of_range &) {
                throw Unreachable();    // not a folder module expected a file module
            }
            // Constructor specific checks
            if (is<ast::decl::Init>(node)) {
                if (modifier_counts[TokenType::ABSTRACT] > 0)
                    throw AnalyzerError("constructor cannot be 'abstract'", node);
                else if (modifier_counts[TokenType::STATIC] > 0)
                    throw AnalyzerError("constructor cannot be 'static'", node);
                else if (modifier_counts[TokenType::STATIC] > 0)
                    throw AnalyzerError("constructor cannot be 'final'", node);
                else if (modifier_counts[TokenType::OVERRIDE] > 0)
                    throw AnalyzerError("constructor cannot be 'override'", node);
            }
        }

        string build_type_params_string(const std::vector<std::shared_ptr<ast::decl::TypeParam>> &type_params) {
            return std::accumulate(type_params.begin(), type_params.end(), string(),
                                   [](const string &a, const std::shared_ptr<ast::decl::TypeParam> &b) {
                                       return a + (a.empty() ? "" : ",") + b->get_name()->get_text();
                                   });
        }

        string build_params_string(const std::shared_ptr<ast::decl::Params> &params) {
            if (!params)
                return "";
            string param_string;
            // TODO: Add absolute path for types
            param_string = std::accumulate(params->get_pos_only().begin(), params->get_pos_only().end(), param_string,
                                           [](const string &a, const std::shared_ptr<ast::decl::Param> &b) {
                                               return a + (a.empty() ? "" : ",") + b->get_name()->get_text();
                                           });
            param_string = std::accumulate(params->get_pos_kwd().begin(), params->get_pos_kwd().end(), param_string,
                                           [](const string &a, const std::shared_ptr<ast::decl::Param> &b) {
                                               return a + (a.empty() ? "" : ",") + b->get_name()->get_text();
                                           });
            param_string = std::accumulate(params->get_kwd_only().begin(), params->get_kwd_only().end(), param_string,
                                           [](const string &a, const std::shared_ptr<ast::decl::Param> &b) {
                                               return a + (a.empty() ? "" : ",") + b->get_name()->get_text();
                                           });
            return param_string;
        }

        string build_function_name(const ast::decl::Function &node) {
            std::stringstream ss;
            ss << node.get_name()->get_text();
            if (auto type_params = node.get_type_params(); !type_params.empty()) {
                ss << '<';
                ss << build_type_params_string(node.get_type_params());
                ss << '>';
            }
            auto params = node.get_params();
            ss << '(';
            ss << build_params_string(params);
            ss << ')';
            return ss.str();
        }

        void visit(ast::decl::Function &node) override {
            check_modifiers(&node, node.get_modifiers());
            if (path_stack.empty()) {
                throw Unreachable();    // surely some parser error
            }

            SymbolPath path(path_stack.top() / build_function_name(node));
            symbols[path] = &node;
            // path_stack.push(path);
            // path_stack.pop();
        }

        void visit(ast::decl::Variable &node) override {
            check_modifiers(&node, node.get_modifiers());
            if (!path_stack.empty()) {
                try {
                    // constants and static variables are allowed in interfaces
                    if (cast<ast::decl::Compound>(symbols.at(path_stack.top()))->get_token()->get_type() ==
                                TokenType::INTERFACE &&
                        node.get_token()->get_type() != TokenType::CONST &&
                        std::find_if(node.get_modifiers().begin(), node.get_modifiers().end(),
                                     [](const std::shared_ptr<Token> &modifier) {
                                         return modifier->get_type() == TokenType::STATIC;
                                     }) == node.get_modifiers().end())
                        throw AnalyzerError("fields are not allowed in interfaces", &node);
                } catch (const CastError &) {
                    if (!is<ast::Module>(symbols.at(path_stack.top())))
                        throw Unreachable();    // surely some parser error
                } catch (const std::out_of_range &) {
                    throw Unreachable();    // not a module
                }
            } else {
                throw Unreachable();    // surely some parser error
            }

            SymbolPath path(path_stack.top() / node.get_name()->get_text());
            symbols[path] = &node;
        }

        string build_init_name(const ast::decl::Init &node) {
            return "init(" + build_params_string(node.get_params()) + ")";
        }

        void visit(ast::decl::Init &node) override {
            check_modifiers(&node, node.get_modifiers());
            if (!path_stack.empty()) {
                try {
                    if (cast<ast::decl::Compound>(symbols.at(path_stack.top()))->get_token()->get_type() ==
                        TokenType::INTERFACE)
                        throw AnalyzerError("constructors are not allowed in interfaces", &node);
                } catch (const CastError &) {
                    throw Unreachable();    // surely some parser error
                } catch (const std::out_of_range &) {
                    throw Unreachable();    // not a module
                }
            } else {
                throw Unreachable();    // surely some parser error
            }

            SymbolPath path(path_stack.top() / build_init_name(node));
            symbols[path] = &node;
            // path_stack.push(path);
            // path_stack.pop();
        }

        void visit(ast::decl::Parent &node) override {}

        void visit(ast::decl::Enumerator &node) override {
            if (!node.get_modifiers().empty())
                throw Unreachable();    // surely some parser error
            if (!path_stack.empty()) {
                try {
                    if (cast<ast::decl::Compound>(symbols.at(path_stack.top()))->get_token()->get_type() != TokenType::ENUM)
                        throw AnalyzerError("enumerators are allowed in enums only", &node);
                } catch (const CastError &) {
                    throw Unreachable();    // surely some parser error
                } catch (const std::out_of_range &) {
                    throw Unreachable();    // not a module
                }
            } else {
                throw Unreachable();    // surely some parser error
            }

            SymbolPath path(path_stack.top() / node.get_name()->get_text());
            symbols[path] = &node;
            // path_stack.push(path);
            // path_stack.pop();
        }

        string build_compound_name(const ast::decl::Compound &node) {
            std::stringstream ss;
            ss << node.get_name()->get_text();
            if (auto type_params = node.get_type_params(); !type_params.empty()) {
                ss << '<';
                ss << build_type_params_string(type_params);
                ss << '>';
            }
            return ss.str();
        }

        void visit(ast::decl::Compound &node) override {
            check_modifiers(&node, node.get_modifiers());
            if (!path_stack.empty()) {
                try {
                    switch (cast<ast::decl::Compound>(symbols.at(path_stack.top()))->get_token()->get_type()) {
                        case TokenType::CLASS:
                            break;
                        case TokenType::ENUM:
                            if (node.get_token()->get_type() == TokenType::ENUM)
                                throw AnalyzerError("nested enums are not allowed", &node);
                            else if (node.get_token()->get_type() == TokenType::ANNOTATION)
                                throw AnalyzerError("annotations are not allowed in enums", &node);
                            break;
                            break;
                        case TokenType::INTERFACE:
                            if (node.get_token()->get_type() == TokenType::CLASS)
                                throw AnalyzerError("classes are not allowed in interfaces", &node);
                            else if (node.get_token()->get_type() == TokenType::ENUM)
                                throw AnalyzerError("enums are not allowed in interfaces", &node);
                            else if (node.get_token()->get_type() == TokenType::ANNOTATION)
                                throw AnalyzerError("annotations are not allowed in interfaces", &node);
                            break;
                        case TokenType::ANNOTATION:
                            if (node.get_token()->get_type() == TokenType::CLASS)
                                throw AnalyzerError("classes are not allowed in annotations", &node);
                            else if (node.get_token()->get_type() == TokenType::ENUM)
                                throw AnalyzerError("enums are not allowed in annotations", &node);
                            else if (node.get_token()->get_type() == TokenType::ANNOTATION)
                                throw AnalyzerError("annotations are not allowed in annotations", &node);
                            break;
                        default:
                            throw Unreachable();    // surely some parser error
                    }
                } catch (const CastError &) {
                    if (!is<ast::Module>(symbols.at(path_stack.top())))
                        throw Unreachable();    // surely some parser error
                } catch (const std::out_of_range &) {
                    throw Unreachable();    // not a module
                }
            } else {
                throw Unreachable();    // surely some parser error
            }

            SymbolPath path(path_stack.top() / build_compound_name(node));
            symbols[path] = &node;
            path_stack.push(path);
            for (auto enumerator: node.get_enumerators()) {
                enumerator->accept(this);
            }
            for (auto member: node.get_members()) {
                member->accept(this);
            }
            path_stack.pop();
        }

        void visit(ast::Import &node) override {}

        void visit(ast::Module &node) override {
            if (!path_stack.empty()) {
                try {
                    if (!is<ast::Module>(symbols.at(path_stack.top())))
                        throw AnalyzerError("modules are allowed in modules only", &node);
                } catch (const std::out_of_range &) {}
            }

            SymbolPath path((path_stack.empty() ? SymbolPath() : path_stack.top()) / node.get_name());
            symbols[path] = &node;
            path_stack.push(path);
            for (auto member: node.get_members()) {
                member->accept(this);
            }
            path_stack.pop();
        }

        const std::unordered_map<SymbolPath, ast::AstNode *> &build() {
            for (auto module: modules) {
                {    // Also add the folders to the path stack for indexing
                    auto path = module->get_file_path().parent_path();
                    auto p_itr = path.begin();
                    for (auto rp_itr = root_path.begin(); rp_itr != root_path.end(); ++p_itr, ++rp_itr);
                    for (; p_itr != path.end(); ++p_itr) {
                        path_stack.emplace((path_stack.empty() ? SymbolPath() : path_stack.top()) / p_itr->stem().string());
                    }
                }
                module->accept(this);
            }
            return symbols;
        }
    };

    SymbolPath::SymbolPath(const string &name) {
        if (name.empty())
            return;
        std::stringstream ss(name);
        string element;
        while (std::getline(ss, element, '.')) elements.push_back(element);
    }

    void Analyzer::analyze(const std::vector<std::shared_ptr<ast::Module>> &modules) {
        if (modules.empty())
            return;
        SymbolTableBuilder builder(modules);
        symbol_table = builder.build();
        for (const auto &[path, _]: symbol_table) {
            std::cout << path.to_string() << '\n';
        }
    }
}    // namespace spade