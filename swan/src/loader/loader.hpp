#pragma once

#include "callable/method.hpp"
#include "elpops/elpdef.hpp"

namespace spade
{
    class SpadeVM;

    struct SWAN_EXPORT LoadResult {
        ObjMethod *entry;
        std::vector<ObjMethod *> inits;
    };

    class SWAN_EXPORT Loader {
        SpadeVM *vm;

        std::vector<Obj *> scope_stack;
        std::vector<Sign> sign_stack;
        std::vector<std::vector<Value>> conpool_stack;

        std::vector<Sign> module_init_signs;

      public:
        explicit Loader(SpadeVM *vm);

        LoadResult load(const fs::path &path);

      private:
        void start_scope(Obj *scope);
        Obj *get_scope() const;
        void end_scope();

        void start_sign_scope(const string &name);
        const Sign &get_sign() const;
        void end_sign_scope();

        void start_conpool_scope(const vector<Value> &conpool);
        const vector<Value> &get_conpool() const;
        void end_conpool_scope();

        fs::path resolve_path(const fs::path &from_path, const fs::path &path);

        string load_elp(const ElpInfo &info, const fs::path &path, std::vector<fs::path> &imports);
        void load_module(const ModuleInfo &info);
        void load_method(const MethodInfo &info);
        void load_class(const ClassInfo &info);

        Table<string> load_meta(const MetaInfo &meta);
        vector<Value> load_const_pool(const vector<CpInfo> &cps);
        Value load_cp(const CpInfo &cp);
        string load_utf8(const _UTF8 &info);
    };
}    // namespace spade
