#ifndef VELOCITY_VM_HPP
#define VELOCITY_VM_HPP

#include "../utils/common.hpp"
#include "thread.hpp"
#include "settings.hpp"
#include "../objects/inbuilt_types.hpp"
#include "../loader/loader.hpp"
#include "../memory/manager.hpp"

namespace spade {
    class SpadeVM {
    private:
        /// The modules
        Table<ObjModule *> modules;
        /// The threads
        std::set<Thread *> threads;
        /// The loader
        Loader loader;
        /// The memory manager
        MemoryManager *manager;
        /// The actions to be performed when the vm terminates
        vector<function<void()>> onExitList;
        /// The vm settings
        Settings settings;
        /// Metadata associated with all objects
        Table<Table<string>> metadata;
        /// The output stream
        std::stringstream out;

    public:
        explicit SpadeVM(MemoryManager *manager, const Settings &settings = {});

        /**
         * This function registers the action which will be executed
         * when the virtual machine terminates
         * @param fun the action
         */
        void onExit(const function<void()> &fun);

        /**
         * This function initiates the virtual machine
         * @param filename the path to the bytecode file
         * @param args the command line args array
         * @return the exit code
         */
        int start(const string &filename, const vector<string> &args);

        /**
         * This function initiates the virtual machine
         * @warning This function is only to be used when the vm is properly loaded.
         * The loading operation is done by the loader but
         * this method does not perform the loading procedure.
         * @param entry the function object which is the entry point
         * @param args the array object which has the command line arguments
         * @return the exit code
         */
        int start(ObjMethod *entry, ObjArray *args);

        /**
         * The vm execution loop
         * @param thread the execution thread
         */
        Obj *run(Thread *thread);

        ThrowSignal runtimeError(const string &str) const;

        /**
         * @throws IllegalAccessError if the symbol cannot be found
         * @param sign the signature of the symbol
         * @return the value of the symbol corresponding to the signature \p sign
         */
        Obj *getSymbol(const string &sign) const;

        /**
         * @param sign the sign of the symbol
         * @return the metadata of the symbol corresponding to \p sign
         */
        const Table<string> &getMetadata(const string &sign);

        /**
         * Sets the metadata of the symbol corresponding to \p sign
         * @param sign the sign of the symbol
         * @param meta the metadata to be set
         */
        void setMetadata(const string &sign, const Table<string> &meta);

        /**
         * Set the value of the symbol corresponding to the signature \p sign
         * @throws IllegalAccessError if the symbol cannot be found
         * @param sign the signature of the symbol
         * @param val the value
         */
        void setSymbol(const string &sign, Obj *val) const;

        std::set<Thread *> &getThreads() { return threads; }

        const std::set<Thread *> &getThreads() const { return threads; }

        /**
         * @return the modules table
         */
        const Table<ObjModule *> &getModules() const { return modules; }

        /**
         * @return the modules table
         */
        Table<ObjModule *> &getModules() { return modules; }

        /**
         * @return the vm settings
         */
        Settings &getSettings() { return settings; }

        /**
         * @return the vm settings
         */
        const Settings &getSettings() const { return settings; }

        /**
         * @return the memory manager
         */
        MemoryManager *getMemoryManager() { return manager; }

        /**
         * @return the memory manager
         */
        const MemoryManager *getMemoryManager() const { return manager; }

        /**
         * @return whatever written to the output
         */
        string getOutput() const { return out.str(); }

        /**
         * @return the current vm respective to the current thread if present, null otherwise
         */
        static SpadeVM *current();

    private:

        /**
         * Checks the casting compatibility between two types
         * @param type1
         * @param type2
         * @return true if casting can be done, false otherwise
         */
        static bool checkCast(const Type *type1, const Type *type2);

        /**
         * Converts a C++ vector to ObjArray
         * @param args the vector
         * @return an array object containing the contents of args
         */
        ObjArray *argsRepr(const vector<string> &args) const;

        /**
         * This function writes to the output
         * @param str
         */
        void write(const string &str) { out << str; }
    };
}

#endif //VELOCITY_VM_HPP
