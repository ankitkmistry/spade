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
        mark(thread->get_value());
        auto state = thread->get_state();
        for (auto frame = state->get_call_stack(); frame <= state->get_frame(); frame++) {
            // mark every frameTemplate
            mark_frame(frame);
        }
    }

    void BasicCollector::mark_frame(Frame *frame) {
        for (const auto constant: frame->get_const_pool()) {
            // mark every constant of the constant pool
            mark(constant);
        }
        for (auto obj = frame->stack; obj < frame->sp; obj++) {
            // mark every object of stack
            mark(*obj);
        }
        // mark args
        for (int i = 0; i < frame->get_args().count(); ++i) {
            const auto arg = frame->get_args().get_arg(i);
            const auto obj = arg.get_value();
            mark(obj);
        }
        // mark locals and closures
        for (int i = 0; i < frame->get_locals().count(); ++i) {
            if (i < frame->get_locals().get_closure_start()) {
                const auto local = frame->get_locals().get_local(i);
                const auto obj = local.get_value();
                mark(obj);
            } else {
                const auto closure = frame->get_locals().get_closure(i);
                const auto obj = closure->get_value();
                mark(obj);
            }
        }
        // mark exceptions
        for (int i = 0; i < frame->get_exceptions().count(); ++i) {
            const auto &exception = frame->get_exceptions().get(i);
            auto obj = exception.get_type();
            mark(obj);
        }
        for (const auto &match: frame->get_matches()) {
            // mark every match
            for (const auto &[value, _]: match.get_table()) {
                // mark every case value
                mark(value);
            }
        }
        mark(frame->get_method());
    }

    void BasicCollector::mark(Obj *obj) {
        if (obj == null)
            return;
        if (obj->get_info().marked)
            return;
        obj->get_info().marked = true;
        grayMaterial.push_back(obj);
        mark(obj->get_module());
        mark(obj->get_type());
    }

    void BasicCollector::trace_references() {
        for (const auto material: grayMaterial) {
            if (is<ObjArray>(material)) {
                auto array = cast<ObjArray>(material);
                array->foreach ([&](auto val) {
                    // mark every value of the array
                    mark(val);
                });
            } else if (is<ObjMethod>(material)) {
                auto method = cast<ObjMethod>(material);
                const FrameTemplate &frame_template = method->get_frame_template();
                // mark args
                for (int i = 0; i < frame_template.get_args().count(); ++i) {
                    const auto &arg = frame_template.get_args().get_arg(i);
                    const auto obj = arg.get_value();
                    mark(obj);
                }
                // mark locals and closures
                for (int i = 0; i < frame_template.get_locals().count(); ++i) {
                    if (i < frame_template.get_locals().get_closure_start()) {
                        const auto &local = frame_template.get_locals().get_local(i);
                        const auto obj = local.get_value();
                        mark(obj);
                    } else {
                        const auto closure = frame_template.get_locals().get_closure(i);
                        const auto obj = closure->get_value();
                        mark(obj);
                    }
                }
                // mark exceptions
                for (int i = 0; i < frame_template.get_exceptions().count(); ++i) {
                    const auto &exception = frame_template.get_exceptions().get(i);
                    const auto obj = exception.get_type();
                    mark(obj);
                }
                // mark matches
                for (const auto &match: frame_template.get_matches())
                    for (const auto &[value, _]: match.get_table())
                        // mark every case value
                        mark(value);
                // mark type params
                for (const auto &[name, typeParam]: method->get_type_params()) {
                    mark(typeParam);
                }
            } else if (is<Type>(material)) {
                const auto type = cast<Type>(material);
                for (const auto &[name, typeParam]: type->get_type_params()) {
                    // mark every type param
                    mark(typeParam);
                }
                for (const auto &[name, super]: type->get_supers()) {
                    // mark every super class
                    mark(super);
                }
                for (const auto &[name, member]: type->get_member_slots()) {
                    // mark every member
                    mark(member.get_value());
                }
            } else {
                const auto obj = material;
                for (const auto &[name, member]: obj->get_member_slots()) {
                    // mark every member
                    mark(member.get_value());
                }
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
