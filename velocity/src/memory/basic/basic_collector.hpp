#pragma once

#include "../../callable/frame.hpp"
#include "../../ee/thread.hpp"
#include "../../objects/obj.hpp"
#include "basic_manager.hpp"

namespace spade::basic
{
    class BasicCollector {
      private:
        BasicMemoryManager *manager;
        vector<Collectible *> grayMaterial;

        void markRoots();

        void markTable(const Table<Obj *> &table);

        void mark(Collectible *collectible);

        void markThread(Thread *thread);

        void markFrame(Frame *frame);

        void traceReferences();

        void sweep();

      public:
        explicit BasicCollector(BasicMemoryManager *manager) : manager(manager) {}

        void gc();
    };
}    // namespace spade::basic
