#include "vm.hpp"

namespace spade
{
    SpadeVM::SpadeVM(MemoryManager *manager, const Settings &settings) : loader(this), manager(manager), settings(settings) {
        this->manager->setVM(this);
        loader = Loader(this);
    }

    void SpadeVM::onExit(const function<void()> &fun) { onExitList.push_back(fun); }

    int SpadeVM::start(const string &filename, const vector<string> &args) {
        // Load the file and get the entry point
        auto entry = loader.load(filename);
        // Complain if there is no entry point
        if (entry == null) throw IllegalAccessError(std::format("cannot find entry point in '{}'", filename));
        if (entry->getFrameTemplate()->getArgs().count() != 1)
            throw runtimeError("entry point must have one argument (.array): " + entry->getSign().toString());
        // Execute from the entry
        return start(entry, argsRepr(args));
    }

    ObjArray *SpadeVM::argsRepr(const vector<string> &args) const {
        auto array = halloc<ObjArray>(manager, args.size());
        for (int i = 0; i < args.size(); ++i) { array->set(i, halloc<ObjString>(manager, args[i])); }
        return array;
    }

    int SpadeVM::start(ObjMethod *entry, ObjArray *args) {
        auto state = new VMState(this);
        Thread thread{state, [&](auto thr) {
                          thr->setStatus(Thread::RUNNING);
                          entry->call({args});
                          run(thr);
                      }};
        threads.insert(&thread);
        thread.join();

        threads.erase(&thread);
        if (threads.empty()) {
            for (auto &action: onExitList) action();
        }
        return thread.getExitCode();
    }

    ThrowSignal SpadeVM::runtimeError(const string &str) const { return ThrowSignal{halloc<ObjString>(manager, str)}; }

    Obj *SpadeVM::getSymbol(const string &sign) const {
        Sign symbolSign{sign};
        vector<SignElement> elements = symbolSign.getElements();
        if (elements.empty()) return ObjNull::value();
        Obj *acc;
        try {
            acc = modules.at(elements[0].toString());
            for (int i = 1; i < elements.size(); ++i) { acc = acc->getMember(elements[i].toString()); }
        } catch (const IllegalAccessError &) {
            throw IllegalAccessError(std::format("cannot find symbol: {}", sign));
        } catch (const std::out_of_range &) { throw IllegalAccessError(std::format("cannot find symbol: {}", sign)); }
        return acc;
    }

    void SpadeVM::setSymbol(const string &sign, Obj *val) const {
        Sign symbolSign{sign};
        vector<SignElement> elements = symbolSign.getElements();
        if (elements.empty()) return;
        try {
            Obj *acc = modules.at(elements[0].toString());
            for (int i = 1; i < elements.size(); ++i) {
                if (i == elements.size() - 1) {
                    acc->getMemberSlots()[elements.back().toString()].setValue(val);
                } else {
                    acc = acc->getMember(elements[i].toString());
                }
            }
        } catch (const IllegalAccessError &) {
            throw IllegalAccessError(std::format("cannot find symbol: {}", sign));
        } catch (const std::out_of_range &) { throw IllegalAccessError(std::format("cannot find symbol: {}", sign)); }
    }

    const Table<string> &SpadeVM::getMetadata(const string &sign) {
        try {
            return metadata.at(sign);
        } catch (const std::out_of_range &) { throw IllegalAccessError(std::format("cannot find metadata: {}", sign)); }
    }

    void SpadeVM::setMetadata(const string &sign, const Table<string> &meta) { metadata[sign] = meta; }

    bool SpadeVM::checkCast(const Type *type1, const Type *type2) {
        // TODO implement this
        return false;
    }

    SpadeVM *SpadeVM::current() {
        if (auto thread = Thread::current(); thread != null) return thread->getState()->getVM();
        return null;
    }
} // namespace spade