#pragma once

#include "debugger.hpp"
#include "loader/loader.hpp"
#include "obj.hpp"
#include "thread.hpp"
#include "utils/errors.hpp"
#include <set>
#include <shared_mutex>

namespace spade
{
    /**
     * Represents VM settings
     */
    struct SWAN_EXPORT Settings {
        string VERSION = "0.0.0";
        string LANG_NAME = "Spade";
        string VM_NAME = "Swan";
        string INFO_STRING = std::format("{} {} {}", LANG_NAME, VM_NAME, VERSION);

        size_t max_call_stack_depth = 1024;

        fs::path lib_path;
        vector<fs::path> mod_path;
    };

    class SWAN_EXPORT SpadeVM {
        /// The modules
        Table<ObjModule *> modules;
        /// The threads
        std::set<Thread *> threads;
        /// The memory manager
        MemoryManager *manager;
        /// The loader
        Loader loader;
        /// The actions to be performed when the vm terminates
        std::vector<std::function<void()>> on_exit_list;
        /// The vm settings
        Settings settings;
        /// Metadata associated with all objects
        Table<Table<string>> metadata;
        std::shared_mutex metadata_mtx;
        /// The exit code of the vm (-1 represents unfinished state)
        int32_t exit_code;
        /// The vm debugger
        std::unique_ptr<Debugger> debugger;
        /// The output stream
        std::stringstream out;

        // State variables

      public:
        explicit SpadeVM(MemoryManager *manager, std::unique_ptr<Debugger> debugger = null, const Settings &settings = {});

        /**
         * This function registers the action which will be executed
         * when the virtual machine terminates
         * @param fun the action
         */
        void on_exit(const std::function<void()> &fun);

        /**
         * This function initiates the virtual machine
         * @param filename the path to the bytecode file
         * @param args the command line args array
         * @param block blocks the caller if the flag is set
         */
        void start(const string &filename, const vector<string> &args = {}, bool block = false);

        /**
         * The vm execution loop
         * @param thread the execution thread
         */
        Value run(Thread *thread);

        ThrowSignal runtime_error(const string &str) const;

        /**
         * @throws IllegalAccessError if the symbol cannot be found
         * @param sign the signature of the symbol
         * @return the value of the symbol corresponding to the signature @p sign
         */
        Value get_symbol(const string &sign, bool strict = true) const;

        /**
         * Set the value of the symbol corresponding to the signature @p sign
         * @throws IllegalAccessError if the symbol cannot be found
         * @param sign the signature of the symbol
         * @param val the value
         */
        void set_symbol(const string &sign, Value val);

        /**
         * @param sign the sign of the symbol
         * @return the metadata of the symbol corresponding to @p sign
         */
        const Table<string> &get_metadata(const string &sign);

        /**
         * Sets the metadata of the symbol corresponding to @p sign
         * @param sign the sign of the symbol
         * @param meta the metadata to be set
         */
        void set_metadata(const string &sign, const Table<string> &meta);

        /**
         * Returns the vm standard type for @p tag or null if the type is unspecified
         * @exception IllegalAccessError if the standard types are not loaded yet
         * @param tag the kind of object to get the standard type for
         * @return Type* the standard type
         */
        // Type *get_vm_type(ObjTag tag);

        /**
         * @return the set of vm threads
         */
        const std::set<Thread *> &get_threads() const {
            return threads;
        }

        /**
         * @return the modules table
         */
        const Table<ObjModule *> &get_modules() const {
            return modules;
        }

        /**
         * @return the vm settings
         */
        Settings &get_settings() {
            return settings;
        }

        /**
         * @return the vm settings
         */
        const Settings &get_settings() const {
            return settings;
        }

        /**
         * @return the memory manager
         */
        MemoryManager *get_memory_manager() {
            return manager;
        }

        /**
         * @return the memory manager
         */
        const MemoryManager *get_memory_manager() const {
            return manager;
        }

        /**
         * @return the exit code of the vm
         */
        int32_t get_exit_code() const {
            return exit_code;
        }

        /**
         * @return whatever written to the output
         */
        string get_output() const {
            return out.str();
        }

        /**
         * @return the current vm respective to the current thread if present, null otherwise
         */
        static SpadeVM *current();

      private:
        /**
         * Loads the basic types and modules required by the vm
         */
        void load_basic();

        void vm_main(const string &filename, const vector<string> &args, Thread *thread);

        /**
         * Checks the casting compatibility between two types
         * @param type1 from type
         * @param type2 destination type
         * @return true if casting can be done, false otherwise
         */
        static bool check_cast(const Type *type1, const Type *type2);

        /**
         * This function writes to the output
         * @param str
         */
        void write(const string &str) {
            out << str;
        }
    };
}    // namespace spade
