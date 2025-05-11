#include "vm.hpp"

namespace spade
{
    SpadeVM::SpadeVM(MemoryManager *manager, const Settings &settings) : loader(this), manager(manager), settings(settings) {
        this->manager->set_vm(this);
        loader = Loader(this);
    }

    void SpadeVM::on_exit(const std::function<void()> &fun) {
        on_exit_list.push_back(fun);
    }

    int SpadeVM::start(const string &file_name, const vector<string> &args) {
        // Load the file and get the entry point
        auto entry = loader.load(file_name);
        // Complain if there is no entry point
        if (entry == null)
            throw IllegalAccessError(std::format("cannot find entry point in '{}'", file_name));
        if (entry->get_frame_template()->get_args().count() != 1)
            throw runtime_error("entry point must have one argument (.array): " + entry->get_sign().to_string());
        // Execute from the entry
        return start(entry, args_repr(args));
    }

    ObjArray *SpadeVM::args_repr(const vector<string> &args) const {
        auto array = halloc<ObjArray>(manager, args.size());
        for (int i = 0; i < args.size(); ++i) {
            array->set(i, halloc<ObjString>(manager, args[i]));
        }
        return array;
    }

    int SpadeVM::start(ObjMethod *entry, ObjArray *args) {
        auto state = new VMState(this);
        Thread thread{state, [&](auto thr) {
                          thr->set_status(Thread::RUNNING);
                          entry->call({args});
                          run(thr);
                      }};
        threads.insert(&thread);
        thread.join();

        threads.erase(&thread);
        if (threads.empty()) {
            for (auto &action: on_exit_list) action();
        }
        return thread.get_exit_code();
    }

    ThrowSignal SpadeVM::runtime_error(const string &str) const {
        return ThrowSignal{halloc<ObjString>(manager, str)};
    }

    Obj *SpadeVM::get_symbol(const string &sign) const {
        Sign symbol_sign{sign};
        vector<SignElement> elements = symbol_sign.get_elements();
        if (elements.empty())
            return ObjNull::value();
        Obj *acc;
        try {
            acc = modules.at(elements[0].to_string());
            for (int i = 1; i < elements.size(); ++i) {
                acc = acc->get_member(elements[i].to_string());
            }
        } catch (const IllegalAccessError &) {
            throw IllegalAccessError(std::format("cannot find symbol: {}", sign));
        } catch (const std::out_of_range &) {
            throw IllegalAccessError(std::format("cannot find symbol: {}", sign));
        }
        return acc;
    }

    void SpadeVM::set_symbol(const string &sign, Obj *val) const {
        Sign symbol_sign{sign};
        vector<SignElement> elements = symbol_sign.get_elements();
        if (elements.empty())
            return;
        try {
            Obj *acc = modules.at(elements[0].to_string());
            for (int i = 1; i < elements.size(); ++i) {
                if (i == elements.size() - 1) {
                    acc->get_member_slots()[elements.back().to_string()].set_value(val);
                } else {
                    acc = acc->get_member(elements[i].to_string());
                }
            }
        } catch (const IllegalAccessError &) {
            throw IllegalAccessError(std::format("cannot find symbol: {}", sign));
        } catch (const std::out_of_range &) {
            throw IllegalAccessError(std::format("cannot find symbol: {}", sign));
        }
    }

    const Table<string> &SpadeVM::get_metadata(const string &sign) {
        try {
            return metadata.at(sign);
        } catch (const std::out_of_range &) {
            throw IllegalAccessError(std::format("cannot find metadata: {}", sign));
        }
    }

    void SpadeVM::set_metadata(const string &sign, const Table<string> &meta) {
        metadata[sign] = meta;
    }

    bool SpadeVM::check_cast(const Type *type1, const Type *type2) {
        // TODO implement this
        return false;
    }

    SpadeVM *SpadeVM::current() {
        if (auto thread = Thread::current(); thread != null)
            return thread->get_state()->get_vm();
        return null;
    }
}    // namespace spade