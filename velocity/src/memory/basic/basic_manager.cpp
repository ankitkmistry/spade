#include "basic_manager.hpp"
#include "basic_collector.hpp"

namespace spade::basic
{
    void *BasicMemoryManager::allocate(size_t size) {
        auto p = new char[size]{0};
        return p;
    }

    void BasicMemoryManager::postAllocation(Collectible *obj) {
        if (head == null || last == null) {
            auto node = new LNode;
            node->data = obj;
            head = node;
            last = node;
        } else {
            auto node = new LNode;
            node->data = obj;
            last->next = node;
            node->prev = last;
            last = node;
        }
    }

    void BasicMemoryManager::deallocate(void *pointer) {
        for (auto node = head; node->next; node = node->next) {
            if (node->data == pointer) {
                if (head == node && node == last) {
                    head = last = null;
                    delete node;
                } else if (node == head) {
                    head = node->next;
                    head->prev = null;
                    delete node;
                } else if (node == last) {
                    last = node->prev;
                    last->next = null;
                    delete node;
                } else {
                    node->prev->next = node->next;
                    node->next->prev = node->prev;
                    delete node;
                }
                break;
            }
        }
        delete (char *) pointer;
    }

    void BasicMemoryManager::collectGarbage() {
        BasicCollector collector{this};
        collector.gc();
    }
}    // namespace spade::basic
