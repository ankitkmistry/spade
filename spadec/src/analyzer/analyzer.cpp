#include "analyzer.hpp"

namespace spade
{
    class SymbolTableBuilder : public ast::VisitorBase {
        std::unordered_map<SymbolPath, ast::AstNode *> symbols;
        std::vector<std::shared_ptr<ast::Module>> modules;

        

      public:
        SymbolTableBuilder(const std::vector<std::shared_ptr<ast::Module>> &modules) : modules(modules) {}

        SymbolTableBuilder(const SymbolTableBuilder &other) = default;
        SymbolTableBuilder(SymbolTableBuilder &&other) noexcept = default;
        SymbolTableBuilder &operator=(const SymbolTableBuilder &other) = default;
        SymbolTableBuilder &operator=(SymbolTableBuilder &&other) noexcept = default;
        ~SymbolTableBuilder() override = default;

        void visit(ast::Reference &node) override;
        void visit(ast::type::Reference &node) override;
        void visit(ast::type::Function &node) override;
        void visit(ast::type::TypeLiteral &node) override;
        void visit(ast::type::TypeOf &node) override;
        void visit(ast::type::BinaryOp &node) override;
        void visit(ast::type::Nullable &node) override;
        void visit(ast::type::TypeBuilder &node) override;
        void visit(ast::type::TypeBuilderMember &node) override;
        void visit(ast::expr::Constant &node) override;
        void visit(ast::expr::Super &node) override;
        void visit(ast::expr::Self &node) override;
        void visit(ast::expr::DotAccess &node) override;
        void visit(ast::expr::Call &node) override;
        void visit(ast::expr::Argument &node) override;
        void visit(ast::expr::Reify &node) override;
        void visit(ast::expr::Index &node) override;
        void visit(ast::expr::Slice &node) override;
        void visit(ast::expr::Unary &node) override;
        void visit(ast::expr::Cast &node) override;
        void visit(ast::expr::Binary &node) override;
        void visit(ast::expr::ChainBinary &node) override;
        void visit(ast::expr::Ternary &node) override;
        void visit(ast::expr::Assignment &node) override;
        void visit(ast::stmt::Block &node) override;
        void visit(ast::stmt::If &node) override;
        void visit(ast::stmt::While &node) override;
        void visit(ast::stmt::DoWhile &node) override;
        void visit(ast::stmt::Throw &node) override;
        void visit(ast::stmt::Catch &node) override;
        void visit(ast::stmt::Try &node) override;
        void visit(ast::stmt::Continue &node) override;
        void visit(ast::stmt::Break &node) override;
        void visit(ast::stmt::Return &node) override;
        void visit(ast::stmt::Yield &node) override;
        void visit(ast::stmt::Expr &node) override;
        void visit(ast::stmt::Declaration &node) override;
        void visit(ast::decl::TypeParam &node) override;
        void visit(ast::decl::Constraint &node) override;
        void visit(ast::decl::Param &node) override;
        void visit(ast::decl::Params &node) override;
        void visit(ast::decl::Function &node) override;
        void visit(ast::decl::Variable &node) override;
        void visit(ast::decl::Init &node) override;
        void visit(ast::decl::Parent &node) override;
        void visit(ast::decl::Enumerator &node) override;
        void visit(ast::decl::Compound &node) override;
        void visit(ast::Import &node) override;

        void visit(ast::Module &node) override {
            symbols[node.get_name()] = &node;
        }

        const std::unordered_map<SymbolPath, ast::AstNode *> &build() {

            return symbols;
        }
    };

    SymbolPath::SymbolPath(const string &name) {
        std::stringstream ss(name);
        string element;
        while (std::getline(ss, element, '.')) elements.push_back(element);
    }

    void Analyzer::set_root_path(const std::vector<std::shared_ptr<ast::Module>> &modules) {
        for (const auto &module: modules)
            if (root_path.empty()) root_path = module->get_file_path().parent_path();
            else if (root_path != module->get_file_path().parent_path())
                for (auto path1 = module->get_file_path(); path1 != fs::current_path().root_path(); path1 = path1.parent_path())
                    for (auto path2 = root_path; path2 != fs::current_path().root_path(); path2 = path2.parent_path())
                        if (path1 == path2) {
                            root_path = path1;
                            break;
                        }
    }

    void Analyzer::analyze(const std::vector<std::shared_ptr<ast::Module>> &modules) {
        set_root_path(modules);
    }
}    // namespace spade