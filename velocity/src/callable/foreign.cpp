#include "foreign.hpp"
#include "../ee/vm.hpp"

namespace spade
{
    void ObjForeign::linkLibrary() {
        auto foreignAnnoType = SpadeVM::current()->getSymbol("spade::foreign.Foreign");
        auto annos = cast<ObjArray *>(getMember("$annotations"));
        Obj *foreignAnno;
        annos->foreach ([foreignAnnoType, &foreignAnno](Obj *anno) {
            if (anno->getType() == foreignAnnoType) { foreignAnno = anno; }
        });
        auto libraryPath = foreignAnno->getMember("path")->toString();
        auto metName = foreignAnno->getMember("name")->toString();
        Library *lib = ForeignLoader::loadSimpleLibrary(libraryPath);
        library = lib;
        if (metName.empty()) {
            metName = "FAI";
            for (const auto &ele: sign.getElements()) { metName += "_" + ele.getName(); }
        }
        name = metName;
    }

    void ObjForeign::call(const vector<Obj *>& args) {
        validateCallSite();
        call(args.data());
    }


    string ObjForeign::toString() const {
        static string kindNames[] = {"function", "method", "constructor"};
        return std::format("<foreign {} '{}'>", kindNames[static_cast<int>(kind)], sign.toString());
    }

    void ObjForeign::setSelf(Obj *selfObj) { this->self = selfObj; }
} // namespace spade