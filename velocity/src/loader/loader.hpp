#pragma once

#include "utils/common.hpp"
#include "objects/module.hpp"
#include "objects/obj.hpp"
#include "callable/table.hpp"
#include "callable/method.hpp"

namespace spade
{
    class SpadeVM;

    /**
     * Represents the loader of the vm
     */
    class Loader {
      private:
        /// Reference to the vm
        SpadeVM *vm;
        /// The memory manager
        MemoryManager *manager;
        /// List of all modules in the form of [path, module]
        std::unordered_map<string, ObjModule *> modules = {};
        /// Pool of unresolved references
        Table<Type *> reference_pool = {};
        ObjModule *current = null;

      public:
        explicit Loader(SpadeVM *vm);

        /**
         * This function loads the bytecode file at \p path and
         * returns the function object which is the entry point of the bytecode file
         * @param path the path to the file
         * @return the entry point if present, null otherwise
         */
        ObjMethod *load(const string &path);


      private:
        /**
         * Loads a module into the vm
         * @param module the module
         */
        void load_module(ObjModule *module);

        /**
         * Reads a module specified by \p path
         * @param path the path of the module
         * @return the module as an ObjModule
         */
        ObjModule *read_module(const string &path);
        Obj *read_global(GlobalInfo &global);
        Obj *read_obj(ObjInfo &obj);
        Obj *read_class(ClassInfo &klass);
        Obj *read_field(FieldInfo &field);
        Obj *read_method(const string &klassSign, MethodInfo &method);
        Obj *read_method(MethodInfo &method);
        Exception read_exception(MethodInfo::ExceptionTableInfo &exception);
        MatchTable read_match(MethodInfo::MatchInfo match);
        NamedRef *read_local(MethodInfo::LocalInfo &local);
        NamedRef *read_arg(MethodInfo::ArgInfo &arg);

        /**
         * Reads the constant pool
         * @param constantPool the constant pool info
         * @param count the size of the constant pool
         * @return the constant pool as a vector
         */
        std::vector<Obj *> read_const_pool(const CpInfo *constantPool, uint16 count);


        /**
         * Reads a constant
         * @param cpInfo the info of the constant
         * @return the constant as an Obj
         */
        Obj *read_cp(const CpInfo &cpInfo);

        static string read_utf8(const _UTF8 &value);

        /**
         * Reads the meta info of the object
         * @param meta the meta info
         * @return the meta info as a table of strings
         */
        static Table<string> read_meta(const MetaInfo &meta);

        /**
         * @param index index of the signature
         * @return the signature specified by the \p index from the constant pool
         *         of the current module
         */
        Sign get_sign(cpidx index) const {
            return {get_constant_pool()[index]->to_string()};
        }

        /**
         * Searches the type specified by \p param in the globals table.
         * If found it returns that type otherwise it checks the unresolved references pool.
         * If it is found in referencePool it returns that type otherwise it creates
         * a unresolved type of param and returns it.
         * @param sign the param of the type
         * @return the type associated with \p param
         */
        Type *find_type(const string &sign);

        /**
         * Resolves the type associated with \p param .
         * Searches for the type in the reference pool.
         * If found then it copies the info from \p type to the unresolved type
         * and returns the unresolved type as resolved. Otherwise, it returns a
         * new Kind with its contents copied from \p type .
         * @param sign the signature of the type
         * @param type type info
         * @return the associated type
         */
        Type *resolve_type(const string &sign, const Type &type);

        ObjModule *get_current_module() const {
            return current;
        }

        const vector<Obj *> &get_constant_pool() const {
            return get_current_module()->get_constant_pool();
        }

        CorruptFileError corrupt() const {
            return CorruptFileError(get_current_module()->get_absolute_path());
        }

        Obj *make_obj(const string &typeSign, const Sign &objSign, Type *type) const;

        Obj *make_obj(const string &typeSign, Type *type) const;

        string resolve_path(const string &pathStr);

        fs::path get_load_path();
    };
}    // namespace spade
