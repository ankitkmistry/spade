#include "thread.hpp"
#include "utils/errors.hpp"
#include "vm.hpp"
#include <condition_variable>
#include <shared_mutex>

namespace spade
{
    ThreadState::ThreadState(size_t max_call_stack_depth)
        : stack_depth(max_call_stack_depth), call_stack(std::make_unique<Frame[]>(stack_depth)), fp(&call_stack[0]) {}

    void ThreadState::push_frame(Frame frame) {
        if (fp - &call_stack[0] >= stack_depth)
            throw StackOverflowError();
        *fp++ = std::move(frame);
    }

    bool ThreadState::pop_frame() {
        if (fp > &call_stack[0]) {
            // std::destroy_at(fp - 1);
            fp--;
            return true;
        }
        return false;
    }

    Thread::Thread(SpadeVM *vm, const std::function<void(Thread *)> &fun, const std::function<void()> &pre_fun)
        : thread(), vm(vm), state(vm->get_settings().max_call_stack_depth) {
        // The mutex to check if the current thread was registered
        std::mutex clr_mutex;
        // The cond var that notifies if the thread has got clearance to start
        std::condition_variable cv_clear;
        // The bool flag to set if the thread has got clearance to start
        bool clearance = false;

        // The mutex to check if the current thread has started
        std::mutex start_mutex;
        // The cond var that notifies if we can safely leave this constructor
        std::condition_variable cv_start;
        // The bool flag to set if the thread has started
        bool started = false;

        // Create the thread
        thread = std::thread([&, fun] {
            {
                // Acquire a lock on the clearance mutex
                std::unique_lock lock(clr_mutex);
                // Wait until we got the clearance
                cv_clear.wait(lock, [&] { return clearance; });
                pre_fun();
                {
                    // Acquire a lock on the start mutex
                    std::lock_guard lk(start_mutex);
                    // Inform that we have started
                    started = true;
                }
            }
            // Notify the ctor that it can safely exit and destroy all the stuff
            cv_start.notify_one();
            // Now we got clearance, we can start now
            fun(this);
        });

        {
            // Acquire a loack on the clearance mutex
            std::lock_guard lk(clr_mutex);
            // Put the thread in the table
            // INFO: Another thread calling Thread::current() which may fail
            // because the below statement may change and invalidate `threads` during rehashing.
            // So, a shared_mutex with an exclusive lock is used to prevent that situation
            std::unique_lock threads_lk(threads_mtx);
            threads[thread.get_id()] = this;
            // Give clearance
            clearance = true;
        }
        // Notify the thread to start
        cv_clear.notify_one();

        {
            // Acquire a lock on the start mutex
            std::unique_lock lock(start_mutex);
            // Wait until we have started the thread
            cv_start.wait(lock, [&] { return started; });
        }
        // Now exit with aura++
        // And destroy everything in the ctor
    }

    Thread *Thread::current() {
        std::shared_lock threads_lk(threads_mtx);
        if (const auto it = threads.find(std::this_thread::get_id()); it != threads.end())
            return it->second;
        return null;
    }
}    // namespace spade
