#pragma once

#include "objects/module.hpp"
#include "utils/common.hpp"
#include "callable/method.hpp"

namespace spade
{
    class SpadeVM;

    class Booter {
        struct ElpContext {
            string entry;
            vector<string> imports;
        };

      private:
        // Pointer to the vm
        SpadeVM *vm;
        // Stack of paths of files as they are loaded
        vector<fs::path> path_stack;
        // Stack of signatures as different symbols are loaded
        vector<Sign> sign_stack;
        /// List of all loaded modules in the form of [path, modules]
        std::unordered_map<fs::path, vector<ObjModule *>> loaded_mods;
        /// Pool of resolved references
        std::unordered_map<Sign, Type *> reference_pool;
        /// Pool of unresolved references
        std::unordered_map<Sign, Type *> unresolved;
        // Pointer to current module
        ObjModule *cur_mod = null;
        // List of post processing callbacks
        std::vector<std::function<void()>> post_callbacks;

      public:
        explicit Booter(SpadeVM *vm) : vm(vm) {}

        ObjMethod *load(const fs::path &path);

      private:
        std::pair<ElpContext, vector<ObjModule *>> _load(const fs::path &path);

        ElpContext load_elp(const ElpInfo &elp);

        ObjModule *load_module(const ModuleInfo &info);
        Obj *load_global(const GlobalInfo &info, const vector<Obj *> &conpool);

        Obj *load_method(const MethodInfo &info, const vector<Obj *> &conpool);
        NamedRef load_arg(const ArgInfo &arg, const vector<Obj *> &conpool);
        NamedRef load_local(const LocalInfo &local, const vector<Obj *> &conpool);
        Exception load_exception(const ExceptionTableInfo &exception, const vector<Obj *> &conpool);
        MatchTable load_match(const MatchInfo match, const vector<Obj *> &conpool);

        Obj *load_class(const ClassInfo &info, const vector<Obj *> &conpool);
        Obj *load_field(const FieldInfo &info, const vector<Obj *> &conpool);

        vector<Obj *> read_const_pool(const vector<CpInfo> &constants);
        Obj *read_cp(const CpInfo &cp);
        string read_utf8(const _UTF8 &value);
        Table<string> read_meta(const MetaInfo &meta);

        Type *find_type(const Sign &sign);
        Type *resolve_type(const Sign &sign);
        Obj *make_obj(const Sign &type_sign, Type *type);
        fs::path resolve_path(const fs::path &from_path, const fs::path &path);

        void begin_scope(const string &name, bool module = false) {
            sign_stack.emplace_back(sign_stack.empty() ? name
                                                       : sign_stack.back() | SignElement(name, module ? Sign::Kind::MODULE : Sign::Kind::CLASS));
        }

        const Sign &current_sign() const {
            return sign_stack.empty() ? Sign::EMPTY : sign_stack.back();
        }

        fs::path current_path() const {
            return path_stack.empty() ? "" : path_stack.back();
        }

        Sign end_scope() {
            if (sign_stack.empty())
                return Sign::EMPTY;
            const Sign sign = sign_stack.back();
            sign_stack.pop_back();
            return sign;
        }

        ObjModule *get_current_module() const {
            return cur_mod;
        }
    };
}    // namespace spade