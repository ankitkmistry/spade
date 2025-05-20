#include "vm.hpp"

namespace spade
{
    SpadeVM::SpadeVM(MemoryManager *manager, const Settings &settings) : loader(this), manager(manager), settings(settings) {
        this->manager->set_vm(this);
        this->loader = Booter(this);
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
        if (auto args_count = entry->get_frame_template().get_args().count(); args_count >= 1)
            throw runtime_error("entry point must have zero or one argument (basic.array): " + entry->get_sign().to_string());
        else if (args_count == 1)
            // Execute from the entry with cmdline args
            return start(entry, args_repr(args));
        else
            // Execute from the entry
            return start(entry);
    }

    ObjArray *SpadeVM::args_repr(const vector<string> &args) const {
        auto array = halloc_mgr<ObjArray>(manager, args.size());
        for (int i = 0; i < args.size(); ++i) {
            array->set(i, halloc_mgr<ObjString>(manager, args[i]));
        }
        return array;
    }

    int SpadeVM::start(ObjMethod *entry, ObjArray *args) {
        Thread thread(this, [&](const auto thr) {
            thr->set_status(Thread::RUNNING);
            entry->call(args ? vector<Obj *>{args} : vector<Obj *>{});
            run(thr);
        });
        threads.insert(&thread);
        thread.join();

        threads.erase(&thread);
        if (threads.empty())
            for (const auto &action: on_exit_list) action();
        return thread.get_exit_code();
    }

    ThrowSignal SpadeVM::runtime_error(const string &str) const {
        return ThrowSignal{halloc_mgr<ObjString>(manager, str)};
    }

    Obj *SpadeVM::get_symbol(const string &sign, bool strict) const {
        const Sign symbol_sign{sign};
        const vector<SignElement> elements = symbol_sign.get_elements();
        if (elements.empty())
            return ObjNull::value();
        Obj *obj;
        if (const auto it = modules.find(elements[0].to_string()); it != modules.end())
            obj = modules.at(elements[0].to_string());
        else {
            if (strict)
                throw IllegalAccessError(std::format("cannot find symbol: {}", sign));
            else
                return null;
        }
        for (int i = 1; i < elements.size(); ++i) {
            try {
                obj = obj->get_member(elements[i].to_string());
            } catch (const IllegalAccessError &) {
                if (strict)
                    throw IllegalAccessError(std::format("cannot find symbol: {}", sign));
                else
                    return null;
            }
        }
        return obj;
    }

    void SpadeVM::set_symbol(const string &sign, Obj *val) const {
        const Sign symbol_sign{sign};
        if (symbol_sign.empty())
            return;
        const vector<SignElement> elements = symbol_sign.get_elements();
        Obj *obj;
        if (const auto it = modules.find(elements[0].to_string()); it != modules.end())
            obj = it->second;
        else
            throw IllegalAccessError(std::format("cannot find symbol: {}", sign));
        for (int i = 1; i < elements.size(); ++i)
            if (i == elements.size() - 1) {
                obj->get_member_slots()[elements.back().to_string()].set_value(val);
            } else
                try {
                    obj = obj->get_member(elements[i].to_string());
                } catch (const IllegalAccessError &) {
                    throw IllegalAccessError(std::format("cannot find symbol: {}", sign));
                }
    }

    const Table<string> &SpadeVM::get_metadata(const string &sign) {
        if (const auto it = metadata.find(sign); it != metadata.end())
            return it->second;
        throw IllegalAccessError(std::format("cannot find metadata: {}", sign));
    }

    void SpadeVM::set_metadata(const string &sign, const Table<string> &meta) {
        metadata[sign] = meta;
    }

    bool SpadeVM::check_cast(const Type *type1, const Type *type2) {
        // TODO implement this
        return false;
    }

    SpadeVM *SpadeVM::current() {
        if (const auto thread = Thread::current())
            return thread->get_state()->get_vm();
        return null;
    }
}    // namespace spade