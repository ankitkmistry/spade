#include "basic_manager.hpp"
#include "basic_collector.hpp"

namespace spade::basic
{
    BasicMemoryManager::~BasicMemoryManager() {
        for (auto node = head; node;) {
            const auto next = node->next;
            delete (char *) node->data;
            delete node;
            node = next;
        }
    }

    void *BasicMemoryManager::allocate(size_t size) {
        cur_alloc_size = size;
        allocation_size += size;
        return new char[size]{0};
    }

    void BasicMemoryManager::post_allocation(Obj *obj) {
        if (head == null || last == null) {
            auto node = new LNode;

            node->size = cur_alloc_size;
            cur_alloc_size = 0;

            node->data = obj;
            head = node;
            last = node;
        } else {
            auto node = new LNode;

            node->size = cur_alloc_size;
            cur_alloc_size = 0;

            node->data = obj;
            last->next = node;
            last = node;
        }
    }

    void BasicMemoryManager::deallocate(void *pointer) {
        LNode *prev = null;
        for (auto node = head; node; node = node->next) {
            if (node->data == pointer) {
                free_size += node->size;
                if (head == node && node == last) {
                    head = last = null;
                    delete node;
                } else if (node == head) {
                    head = node->next;
                    delete node;
                } else if (node == last) {
                    prev->next = null;
                    delete node;
                } else {
                    prev->next = node->next;
                    delete node;
                }
                break;
            }
            prev = node;
        }
        delete (char *) pointer;
    }

    void BasicMemoryManager::collect_garbage() {
        BasicCollector(this).gc();
    }
}    // namespace spade::basic
