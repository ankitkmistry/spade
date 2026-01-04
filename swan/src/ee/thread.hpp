#pragma once

#include "callable/frame.hpp"
#include "obj.hpp"
#include <thread>
#include <shared_mutex>

namespace spade
{
    class SpadeVM;

    class ThreadState {
        /// Maximum call stack depth
        ptrdiff_t stack_depth;
        /// Call stack
        std::unique_ptr<Frame[]> call_stack;
        /// Frame pointer to the next frame of the current active frame
        Frame *fp = null;

      public:
        ThreadState(size_t max_call_stack_depth);

        ThreadState() = delete;
        ThreadState(const ThreadState &) = delete;
        ThreadState(ThreadState &&) = default;
        ThreadState &operator=(const ThreadState &) = delete;
        ThreadState &operator=(ThreadState &&) = default;
        ~ThreadState() = default;

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
        void push(Obj *val) {
            get_frame()->push(val);
        }

        /**
         * Pops the operand stack
         * @return the popped value
         */
        Obj *pop() {
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
        const Frame *get_frame() const {
            return fp - 1;
        }

        /**
         * @return The active frame
         */
        Frame *get_frame() {
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
    };

    /**
     * Representation of a vm thread
     */
    class Thread {
        inline static std::shared_mutex threads_mtx;
        inline static std::unordered_map<std::thread::id, Thread *> threads;

      public:
        enum Status {
            /// The thread has not started yet
            NOT_STARTED,
            /// This thread is currently in execution
            RUNNING,
            /// The thread has terminated
            TERMINATED,
        };

      private:
        /// Underlying thread object
        std::thread thread;
        // TODO: Fix program representation
        /// Program representation
        Obj *value = null;
        /// The vm state stored in the thread
        SpadeVM *vm;
        /// The current state of the thread;
        ThreadState state;
        /// Status of the thread
        Status status = NOT_STARTED;
        /// Exit code of the thread
        int exit_code = 0;

      public:
        /**
         * Constructs a new Thread object and blocks until the thread is started.
         * This is because the global threads table should be updated before @p fun is called.
         * @p pre_fun is also called before @p fun is called.
         * This is necessary to ensure that @p fun is able to function normally and does not get
         * involved in a data race
         * @param vm The vm of the thread
         * @param fun The primary function to execute
         * @param pre_fun The function to execute before @p fun is called
         */
        Thread(SpadeVM *vm, const std::function<void(Thread *)> &fun, const std::function<void()> &pre_fun = [] {});

        /**
         * @return The exitcode of the thread
         */
        int get_exit_code() const {
            return exit_code;
        }

        /**
         * @return The object representation of the thread
         */
        Obj *get_value() const {
            return value;
        }

        /**
         * @return The vm instance
         */
        const SpadeVM *get_vm() const {
            return vm;
        }

        /**
         * @return The vm instance
         */
        SpadeVM *get_vm() {
            return vm;
        }

        /**
         * @return The thread state
         */
        const ThreadState &get_state() const {
            return state;
        }

        /**
         * @return The thread state
         */
        ThreadState &get_state() {
            return state;
        }

        /**
         * @return The status of the thread
         */
        Status get_status() const {
            return status;
        }

        /**
         * Sets the status of the thread
         * @param status_ the new thread status
         */
        void set_status(Status status_) {
            status = status_;
        }

        /**
         * Sets the exit code of the thread
         * @param code the new exit code value
         */
        void set_exit_code(int code) {
            exit_code = code;
        }

        /**
         * @return true if the thread is running, false otherwise
         */
        bool is_running() {
            return status == RUNNING;
        }

        /**
         * Blocks the caller thread until this thread completes.
         * Upon the completion of this thread the function returns to the caller thread
         */
        void join() {
            thread.join();
        }

        /**
         * @return the current thread
         */
        static Thread *current();
    };
}    // namespace spade
