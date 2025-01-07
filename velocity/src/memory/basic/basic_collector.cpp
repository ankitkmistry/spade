#include "basic_collector.hpp"
#include "../../ee/vm.hpp"

namespace spade::basic
{
    void BasicCollector::gc() {
        markRoots();
        traceReferences();
        sweep();
    }

    void BasicCollector::markRoots() {
        auto vm = manager->getVM();
        // mark the globals
        for (auto [key, module]: vm->getModules()) {
            mark(module);
        }
        // mark the threads
        for (auto thread: vm->getThreads()) markThread(thread);
    }

    void BasicCollector::markTable(const Table<Obj *> &table) {
        for (auto [key, object]: table) {
            // mark every value in the table
            mark(object);
        }
    }

    void BasicCollector::markThread(Thread *thread) {
        // mark the value of the thread
        mark((Obj *) thread->getValue());
        auto state = thread->getState();
        for (auto frame = state->getCallStack(); frame <= state->getFrame(); frame++) {
            // mark every frameTemplate
            markFrame(frame);
        }
    }

    void BasicCollector::markFrame(Frame *frame) {
        for (auto constant: frame->getConstPool()) {
            // mark every constant of the constant pool
            mark(constant);
        }
        for (auto obj = *frame->stack; obj < *frame->sp; obj++) {
            // mark every object of stack
            mark(obj);
        }
        // mark args
        for (int i = 0; i < frame->getArgs().count(); ++i) {
            auto arg = frame->getArgs().getArg(i);
            auto obj = arg->getValue();
            mark(arg);
            mark(obj);
        }
        // mark locals and closures
        for (int i = 0; i < frame->getLocals().count(); ++i) {
            if (i < frame->getLocals().getClosureStart()) {
                auto local = frame->getLocals().getLocal(i);
                auto obj = local->getValue();
                mark(local);
                mark(obj);
            } else {
                auto closure = frame->getLocals().getClosure(i);
                auto obj = closure->getValue();
                mark(closure);
                mark(obj);
            }
        }
        // mark exceptions
        for (int i = 0; i < frame->getExceptions().count(); ++i) {
            const auto &exception = frame->getExceptions().get(i);
            auto obj = exception.getType();
            mark((Obj *) obj);
        }
        for (auto lambda: frame->getLambdas()) {
            mark(lambda);
        }
        for (const auto &match: frame->getMatches()) {
            // mark every check
            for (const auto &kase: match.getCases()) {
                // mark every case value
                auto obj = kase.getValue();
                mark(obj);
            }
        }
        mark((Obj *) frame->getMethod());
    }

    void BasicCollector::mark(Collectible *collectible) {
        if (collectible == null) return;
        if (collectible->getInfo().marked) return;
        collectible->getInfo().marked = true;
        grayMaterial.push_back(collectible);
        if (is<Obj *>(collectible)) {
            auto obj = cast<Obj *>(collectible);
            mark(obj->getModule());
            mark((Obj *) obj->getType());
        }
    }

    void BasicCollector::traceReferences() {
        for (auto material: grayMaterial) {
            if (is<ObjArray *>(material)) {
                auto array = cast<ObjArray *>(material);
                array->foreach ([&](auto val) {
                    // mark every value of the array
                    mark(val);
                });
            } else if (is<ObjMethod *>(material)) {
                auto method = cast<ObjMethod *>(material);
                const FrameTemplate *frameTemplate = method->getFrameTemplate();
                // mark args
                for (int i = 0; i < frameTemplate->getArgs().count(); ++i) {
                    auto arg = frameTemplate->getArgs().getArg(i);
                    auto obj = arg->getValue();
                    mark((Collectible *) arg);
                    mark(obj);
                }
                // mark locals and closures
                for (int i = 0; i < frameTemplate->getLocals().count(); ++i) {
                    if (i < frameTemplate->getLocals().getClosureStart()) {
                        auto local = frameTemplate->getLocals().getLocal(i);
                        auto obj = local->getValue();
                        mark(local);
                        mark(obj);
                    } else {
                        auto closure = frameTemplate->getLocals().getClosure(i);
                        auto obj = closure->getValue();
                        mark(closure);
                        mark(obj);
                    }
                }
                // mark exceptions
                for (int i = 0; i < frameTemplate->getExceptions().count(); ++i) {
                    const auto &exception = frameTemplate->getExceptions().get(i);
                    auto obj = exception.getType();
                    mark((Obj *) obj);
                }
                // mark lambdas
                for (auto lambda: frameTemplate->getLambdas()) {
                    mark(lambda);
                }
                // mark matches
                for (const auto &match: frameTemplate->getMatches()) {
                    for (const auto &kase: match.getCases()) {
                        // mark every case value
                        auto obj = kase.getValue();
                        mark(obj);
                    }
                }
                // mark type params
                for (auto [name, typeParam]: method->getTypeParams()) {
                    mark(typeParam);
                    mark(typeParam->getValue());
                }
            } else if (is<Type *>(material)) {
                auto type = cast<Type *>(material);
                for (auto [name, typeParam]: type->getTypeParams()) {
                    // mark every type param
                    mark((Obj *) typeParam);
                }
                for (auto [name, super]: type->getSupers()) {
                    // mark every super class
                    mark((Obj *) super);
                }
                for (auto [name, member]: type->getMemberSlots()) {
                    // mark every member
                    mark(member.getValue());
                }
            } else if (is<Obj *>(material)) {
                auto obj = cast<Obj *>(material);
                for (auto [name, member]: obj->getMemberSlots()) {
                    // mark every member
                    mark(member.getValue());
                }
            } else {
                auto ref = cast<NamedRef *>(material);
                mark(ref->getValue());
            }
        }
    }

    void BasicCollector::sweep() {
        LNode *previous = null;
        LNode *current = manager->head;
        while (current != null) {
            auto &info = current->data->getInfo();
            if (info.marked) {
                info.marked = false;
                info.life++;
                previous = current;
                current = current->next;
            } else {
                auto unreached = current;
                current = current->next;
                if (previous != null) previous->next = current;
                hfree(unreached->data);
                manager->deallocate(unreached);
            }
        }
    }

}    // namespace spade::basic
