#include "foreign.hpp"
#include "ee/vm.hpp"

namespace spade
{
    void ObjForeign::link_library() {
        auto foreign_anno_type = SpadeVM::current()->get_symbol("spade::foreign.Foreign");
        auto annos = cast<ObjArray>(get_member("$annotations"));
        Obj *foreign_anno;
        annos->foreach ([foreign_anno_type, &foreign_anno](Obj *anno) {
            if (anno->get_type() == foreign_anno_type) {
                foreign_anno = anno;
            }
        });
        auto library_path = foreign_anno->get_member("path")->to_string();
        auto met_name = foreign_anno->get_member("name")->to_string();
        Library *lib = ForeignLoader::load_simple_library(library_path);
        library = lib;
        if (met_name.empty()) {
            met_name = "FAI";
            for (const auto &elm: sign.get_elements()) {
                met_name += "_" + elm.get_name();
            }
        }
        name = met_name;
    }

    void ObjForeign::call(const vector<Obj *> &args) {
        validate_call_site();
        this->call(const_cast<Obj **>(&args[0]));
    }

    string ObjForeign::to_string() const {
        static string kindNames[] = {"function", "method", "constructor"};
        return std::format("<foreign {} '{}'>", kindNames[static_cast<int>(kind)], sign.to_string());
    }

    void ObjForeign::set_self(Obj *selfObj) {
        this->self = selfObj;
    }
}    // namespace spade