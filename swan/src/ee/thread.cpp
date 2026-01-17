#include "thread.hpp"
#include "callable/frame.hpp"
#include "utils/errors.hpp"
#include "vm.hpp"
#include <atomic>
#include <condition_variable>
#include <cstring>
#include <mutex>
#include <shared_mutex>
#include <spdlog/spdlog.h>
#include <sstream>
#include <thread>

namespace spade
{
    ThreadState::ThreadState(size_t max_call_stack_depth)
        // : stack_depth(max_call_stack_depth), call_stack(std::make_unique<Frame[]>(stack_depth)), fc(0) {
        : stack_depth(max_call_stack_depth), call_stack() {}

    void ThreadState::push_frame(Frame &&frame) {
        // if (fc >= stack_depth)
        //     throw StackOverflowError();
        // call_stack[fc] = std::move(frame);
        // fc++;
        if (call_stack.size() >= stack_depth)
            throw StackOverflowError();
        call_stack.push_back(std::move(frame));
    }

    bool ThreadState::pop_frame() {
        // if (fc > 0) {
        //     fc--;
        //     // std::destroy_at(&frame[fc]);
        //     return true;
        // }
        // return false;
        if (call_stack.size() > 0) {
            call_stack.pop_back();
            return true;
        }
        return false;
    }

    Thread::Thread(SpadeVM *vm, const std::function<void(Thread *)> &fun, const std::function<void()> &pre_fun)
        : thread(), vm(vm), state(vm->get_settings().max_call_stack_depth) {
        // The mutex to check if the current thread has started
        std::mutex start_mutex;
        // The cond var that notifies if we can safely leave this constructor
        std::condition_variable cv_start;
        // The bool flag to set if the thread has started
        std::atomic_flag started;

        // Create the thread
        thread = std::thread([&, fun] {
            {
                // Put the thread in the table
                // INFO: Another thread calling Thread::current() which may fail
                // because the below statement may change and invalidate `threads` during rehashing.
                // So, a shared_mutex with an exclusive lock is used to prevent that situation
                std::unique_lock threads_lk(threads_mtx);
                threads.emplace(std::this_thread::get_id(), this);
            }
            pre_fun();
            {
                // Acquire a lock on the start mutex
                std::lock_guard start_lk(start_mutex);
                // Inform that we have started
                started.test_and_set();
            }
            // Notify the ctor that it can safely exit and destroy all the stuff
            cv_start.notify_all();
            // Now we got clearance, we can start now
            fun(this);
        });

        // Acquire a lock on the start mutex
        std::unique_lock start_lk(start_mutex);
        // Wait until we have started the thread
        cv_start.wait(start_lk, [&] { return started.test(); });

        // Now exit with aura++
        // And destroy everything in the ctor
    }

    Thread::~Thread() {
        std::unique_lock threads_lk(threads_mtx);
        threads.erase(thread.get_id());
    }

    Thread *Thread::current() {
        std::shared_lock threads_lk(threads_mtx);
        if (const auto it = threads.find(std::this_thread::get_id()); it != threads.end())
            return it->second;
        return null;
    }
}    // namespace spade
