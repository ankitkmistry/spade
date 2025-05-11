#include "basic_collector.hpp"
#include "ee/vm.hpp"

namespace spade::basic
{
    void BasicCollector::gc() {
        mark_roots();
        trace_references();
        sweep();
    }

    void BasicCollector::mark_roots() {
        auto vm = manager->get_vm();
        // mark the globals
        for (auto [key, module]: vm->get_modules()) {
            mark(module);
        }
        // mark the threads
        for (auto thread: vm->get_threads()) mark_thread(thread);
    }

    void BasicCollector::mark_table(const Table<Obj *> &table) {
        for (auto [key, object]: table) {
            // mark every value in the table
            mark(object);
        }
    }

    void BasicCollector::mark_thread(Thread *thread) {
        // mark the value of the thread
        mark((Obj *) thread->get_value());
        auto state = thread->get_state();
        for (auto frame = state->get_call_stack(); frame <= state->get_frame(); frame++) {
            // mark every frameTemplate
            mark_frame(frame);
        }
    }

    void BasicCollector::mark_frame(Frame *frame) {
        for (auto constant: frame->get_const_pool()) {
            // mark every constant of the constant pool
            mark(constant);
        }
        for (auto obj = *frame->stack; obj < *frame->sp; obj++) {
            // mark every object of stack
            mark(obj);
        }
        // mark args
        for (int i = 0; i < frame->get_args().count(); ++i) {
            auto arg = frame->get_args().get_arg(i);
            auto obj = arg->get_value();
            mark(arg);
            mark(obj);
        }
        // mark locals and closures
        for (int i = 0; i < frame->get_locals().count(); ++i) {
            if (i < frame->get_locals().get_closure_start()) {
                auto local = frame->get_locals().get_local(i);
                auto obj = local->get_value();
                mark(local);
                mark(obj);
            } else {
                auto closure = frame->get_locals().get_closure(i);
                auto obj = closure->get_value();
                mark(closure);
                mark(obj);
            }
        }
        // mark exceptions
        for (int i = 0; i < frame->get_exceptions().count(); ++i) {
            const auto &exception = frame->get_exceptions().get(i);
            auto obj = exception.get_type();
            mark((Obj *) obj);
        }
        for (auto lambda: frame->get_lambdas()) {
            mark(lambda);
        }
        for (const auto &match: frame->get_matches()) {
            // mark every check
            for (const auto &kase: match.get_cases()) {
                // mark every case value
                auto obj = kase.get_value();
                mark(obj);
            }
        }
        mark((Obj *) frame->get_method());
    }

    void BasicCollector::mark(Collectible *collectible) {
        if (collectible == null)
            return;
        if (collectible->get_info().marked)
            return;
        collectible->get_info().marked = true;
        grayMaterial.push_back(collectible);
        if (is<Obj>(collectible)) {
            auto obj = cast<Obj>(collectible);
            mark(obj->get_module());
            mark((Obj *) obj->get_type());
        }
    }

    void BasicCollector::trace_references() {
        for (auto material: grayMaterial) {
            if (is<ObjArray>(material)) {
                auto array = cast<ObjArray>(material);
                array->foreach ([&](auto val) {
                    // mark every value of the array
                    mark(val);
                });
            } else if (is<ObjMethod>(material)) {
                auto method = cast<ObjMethod>(material);
                const FrameTemplate *frameTemplate = method->get_frame_template();
                // mark args
                for (int i = 0; i < frameTemplate->get_args().count(); ++i) {
                    auto arg = frameTemplate->get_args().get_arg(i);
                    auto obj = arg->get_value();
                    mark((Collectible *) arg);
                    mark(obj);
                }
                // mark locals and closures
                for (int i = 0; i < frameTemplate->get_locals().count(); ++i) {
                    if (i < frameTemplate->get_locals().get_closure_start()) {
                        auto local = frameTemplate->get_locals().get_local(i);
                        auto obj = local->get_value();
                        mark(local);
                        mark(obj);
                    } else {
                        auto closure = frameTemplate->get_locals().get_closure(i);
                        auto obj = closure->get_value();
                        mark(closure);
                        mark(obj);
                    }
                }
                // mark exceptions
                for (int i = 0; i < frameTemplate->get_exceptions().count(); ++i) {
                    const auto &exception = frameTemplate->get_exceptions().get(i);
                    auto obj = exception.get_type();
                    mark((Obj *) obj);
                }
                // mark lambdas
                for (auto lambda: frameTemplate->get_lambdas()) {
                    mark(lambda);
                }
                // mark matches
                for (const auto &match: frameTemplate->get_matches()) {
                    for (const auto &kase: match.get_cases()) {
                        // mark every case value
                        auto obj = kase.get_value();
                        mark(obj);
                    }
                }
                // mark type params
                for (auto [name, typeParam]: method->get_type_params()) {
                    mark(typeParam);
                    mark(typeParam->get_value());
                }
            } else if (is<Type>(material)) {
                auto type = cast<Type>(material);
                for (auto [name, typeParam]: type->get_type_params()) {
                    // mark every type param
                    mark((Obj *) typeParam);
                }
                for (auto [name, super]: type->get_supers()) {
                    // mark every super class
                    mark((Obj *) super);
                }
                for (auto [name, member]: type->get_member_slots()) {
                    // mark every member
                    mark(member.get_value());
                }
            } else if (is<Obj>(material)) {
                auto obj = cast<Obj>(material);
                for (auto [name, member]: obj->get_member_slots()) {
                    // mark every member
                    mark(member.get_value());
                }
            } else {
                auto ref = cast<NamedRef>(material);
                mark(ref->get_value());
            }
        }
    }

    void BasicCollector::sweep() {
        LNode *previous = null;
        LNode *current = manager->head;
        while (current != null) {
            auto &info = current->data->get_info();
            if (info.marked) {
                info.marked = false;
                info.life++;
                previous = current;
                current = current->next;
            } else {
                auto unreached = current;
                current = current->next;
                if (previous != null)
                    previous->next = current;
                hfree(unreached->data);
                manager->deallocate(unreached);
            }
        }
    }

}    // namespace spade::basic
