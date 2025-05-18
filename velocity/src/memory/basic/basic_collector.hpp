#pragma once

#include "callable/frame.hpp"
#include "ee/thread.hpp"
#include "objects/obj.hpp"
#include "basic_manager.hpp"

namespace spade::basic
{
    class BasicCollector {
      private:
        BasicMemoryManager *manager;
        vector<Obj *> grayMaterial;

        void mark_roots();

        void mark_table(const Table<Obj *> &table);

        void mark(Obj *obj);

        void mark_thread(Thread *thread);

        void mark_frame(Frame *frame);

        void trace_references();

        void sweep();

      public:
        explicit BasicCollector(BasicMemoryManager *manager) : manager(manager) {}

        void gc();
    };
}    // namespace spade::basic
