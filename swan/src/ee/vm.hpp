#pragma once

#include "ee/debugger.hpp"
#include "loader/loader.hpp"
#include "obj.hpp"
#include "thread.hpp"
#include "../utils/errors.hpp"
#include <format>
#include <set>
#include <shared_mutex>

namespace spade
{
    /**
     * Represents VM settings
     */
    struct Settings {
        string VERSION = "0.0.0";
        string LANG_NAME = "Spade";
        string VM_NAME = "Swan";
        string INFO_STRING = std::format("{} {} {}", LANG_NAME, VM_NAME, VERSION);

        size_t max_call_stack_depth = 1024;

        fs::path lib_path;
        vector<fs::path> mod_path;
    };

    class SpadeVM {
        /// The modules
        Table<ObjModule *> modules;
        /// The threads
        std::set<Thread *> threads;
        /// The loader
        Loader loader;
        /// The memory manager
        MemoryManager *manager;
        /// The actions to be performed when the vm terminates
        std::vector<std::function<void()>> on_exit_list;
        /// The vm settings
        Settings settings;
        /// Metadata associated with all objects
        Table<Table<string>> metadata;
        std::shared_mutex metadata_mtx;
        /// The exit code of the vm (-1 represents unfinished state)
        int exit_code;
        /// The vm debugger
        std::unique_ptr<Debugger> debugger;
        /// The output stream
        std::stringstream out;

        // State variables
        /// Maximum call stack depth
        ptrdiff_t stack_depth;
        /// Call stack
        std::unique_ptr<Frame[]> call_stack;
        /// Frame pointer to the next frame of the current active frame
        Frame *fp = null;

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
        void start(const string &filename, const vector<string> &args, bool block = false);

        /**
         * The vm execution loop
         * @param thread the execution thread
         */
        Obj *run(Thread *thread);

        ThrowSignal runtime_error(const string &str) const;

        /**
         * @throws IllegalAccessError if the symbol cannot be found
         * @param sign the signature of the symbol
         * @return the value of the symbol corresponding to the signature @p sign
         */
        Obj *get_symbol(const string &sign, bool strict = true) const;

        /**
         * Set the value of the symbol corresponding to the signature @p sign
         * @throws IllegalAccessError if the symbol cannot be found
         * @param sign the signature of the symbol
         * @param val the value
         */
        void set_symbol(const string &sign, Obj *val);

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
        Type *get_vm_type(ObjTag tag);

        // State operations
        // Frame operations
        /**
         * Pushes a call frame on top of the call stack
         * @param frame the frame to be pushed
         */
        void push_frame(Frame frame);

        /**
         * Pops the active call frame and reloads the state
         * @return true if a frame was popped
         */
        bool pop_frame();

        // Stack operations
        /**
         * Pushes val on top of the operand stack
         * @param val value to be pushed
         */
        void push(Obj *val) const {
            get_frame()->push(val);
        }

        /**
         * Pops the operand stack
         * @return the popped value
         */
        Obj *pop() const {
            return get_frame()->pop();
        }

        /**
         * @return the value on top of the operand stack
         */
        Obj *peek() const {
            return get_frame()->peek();
        }

        // Constant pool operations
        /**
         * Loads the constant from the constant pool at index
         * @param index
         * @return the loaded value
         */
        Obj *load_const(uint16_t index) const {
            return get_frame()->get_const_pool()[index]->copy();
        }

        // Code operations
        /**
         * Advances ip by 1 byte and returns the byte read
         * @return the byte
         */
        uint8_t read_byte() {
            return *get_frame()->ip++;
        }

        /**
         * Advances ip by 2 bytes and returns the bytes read
         * @return the short
         */
        uint16_t read_short() {
            const auto frame = get_frame();
            frame->ip += 2;
            return (frame->ip[-2] << 8) | frame->ip[-1];
        }

        /**
         * Adjusts the ip by offset
         * @param offset offset to be adjusted
         */
        void adjust(ptrdiff_t offset) {
            get_frame()->ip += offset;
        }

        /**
         * @return The call stack
         */
        const Frame *get_call_stack() const {
            return &call_stack[0];
        }

        /**
         * @return The call stack
         */
        Frame *get_call_stack() {
            return &call_stack[0];
        }

        /**
         * @return The active frame
         */
        Frame *get_frame() const {
            return fp - 1;
        }

        /**
         * @return The size of the call stack
         */
        uint16_t get_call_stack_size() const {
            return fp - &call_stack[0];
        }

        /**
         * @return The program counter
         */
        uint32_t get_pc() const {
            return get_frame()->ip - &get_frame()->code[0];
        }

        /**
         * Sets the program counter
         * @param pc the program counter value
         */
        void set_pc(uint32_t pc) {
            get_frame()->ip = &get_frame()->code[0] + pc;
        }

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
        int get_exit_code() const {
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
