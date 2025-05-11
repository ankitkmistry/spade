#ifndef VELOCITY_THREAD_HPP
#define VELOCITY_THREAD_HPP

#include <thread>
#include "../utils/common.hpp"
#include "state.hpp"

namespace spade
{
    /**
     * Representation of a vm thread
     */
    class Thread {
        inline static std::unordered_map<std::thread::id, Thread *> threads = {};

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
        // TODO Fix program representation
        /// Program representation
        Obj *value = null;
        /// The vm state stored in the thread
        VMState *state;
        /// Status of the thread
        Status status = NOT_STARTED;
        /// Exit code of the thread
        int exit_code = 0;

      public:
        Thread(VMState *state, std::function<void(Thread *)> fun);

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
         * @return The vm state
         */
        VMState *get_state() const {
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

#endif    //VELOCITY_THREAD_HPP
