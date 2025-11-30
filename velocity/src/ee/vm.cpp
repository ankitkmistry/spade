#include "vm.hpp"
#include "memory/memory.hpp"
#include "utils/common.hpp"
#include "objects/module.hpp"
#include "objects/obj.hpp"
#include "objects/type.hpp"
#include "objects/typeparam.hpp"
#include "objects/inbuilt_types.hpp"

// #include "jit/jit.hpp"

namespace spade
{
    SpadeVM::SpadeVM(MemoryManager *manager, const Settings &settings) : loader(this), manager(manager), settings(settings) {
        this->manager->set_vm(this);
        this->loader = Booter(this);
    }

    void SpadeVM::on_exit(const std::function<void()> &fun) {
        on_exit_list.push_back(fun);
    }

    void SpadeVM::load_basic() {
        if (modules.contains("basic"))
            return;
        const auto module = halloc_mgr<ObjModule>(manager, Sign("basic"));
        Table<MemberSlot> members;

        const auto type_any =
                halloc_mgr<Type>(manager, Sign("basic.any"), Type::Kind::CLASS, Table<TypeParam *>{}, Table<Type *>{}, Table<MemberSlot>{});
        const Table<Type *> supers = {
                {type_any->get_sign().to_string(), type_any}
        };

        const auto type_Enum = halloc_mgr<Type>(manager, Sign("basic.Enum"), Type::Kind::CLASS, Table<TypeParam *>{}, supers, Table<MemberSlot>{});
        const auto type_Annotation =
                halloc_mgr<Type>(manager, Sign("basic.Annotation"), Type::Kind::CLASS, Table<TypeParam *>{}, supers, Table<MemberSlot>{});
        const auto type_Throwable =
                halloc_mgr<Type>(manager, Sign("basic.Throwable"), Type::Kind::CLASS, Table<TypeParam *>{}, supers, Table<MemberSlot>{});

        const auto type_bool = halloc_mgr<Type>(manager, Sign("basic.bool"), Type::Kind::CLASS, Table<TypeParam *>{}, supers, Table<MemberSlot>{});
        const auto type_int = halloc_mgr<Type>(manager, Sign("basic.int"), Type::Kind::CLASS, Table<TypeParam *>{}, supers, Table<MemberSlot>{});
        const auto type_float = halloc_mgr<Type>(manager, Sign("basic.float"), Type::Kind::CLASS, Table<TypeParam *>{}, supers, Table<MemberSlot>{});
        const auto type_char = halloc_mgr<Type>(manager, Sign("basic.char"), Type::Kind::CLASS, Table<TypeParam *>{}, supers, Table<MemberSlot>{});
        const auto type_string =
                halloc_mgr<Type>(manager, Sign("basic.string"), Type::Kind::CLASS, Table<TypeParam *>{}, supers, Table<MemberSlot>{});

        const Table<TypeParam *> type_array_tps{
                {"[T]", halloc_mgr<TypeParam>(manager, Sign("[T]"))}
        };
        const auto type_array = halloc_mgr<Type>(manager, Sign("basic.array[T]"), Type::Kind::CLASS, type_array_tps, supers, Table<MemberSlot>{});

        members["any"] = MemberSlot(type_any);
        members["Enum"] = MemberSlot(type_Enum);
        members["Annotation"] = MemberSlot(type_Annotation);
        members["Throwable"] = MemberSlot(type_Throwable);
        members["bool"] = MemberSlot(type_bool);
        members["int"] = MemberSlot(type_int);
        members["float"] = MemberSlot(type_float);
        members["char"] = MemberSlot(type_char);
        members["string"] = MemberSlot(type_string);
        members["array[T]"] = MemberSlot(type_array);

        module->set_member_slots(members);
        modules["basic"] = module;
    }

    void SpadeVM::vm_main(const string &filename, const vector<string> &args, Thread *thread) {
        thread->set_status(Thread::RUNNING);
        try {
            // Load the basic types and module
            load_basic();
            // Load the file and ge the entry point
            const auto entry = loader.load(filename);
            // Complain if there is no entry point
            if (entry == null)
                throw IllegalAccessError(std::format("cannot find entry point in '{}'", filename));
            // Call the function
            if (const auto args_count = entry->get_frame_template().get_args().count(); args_count == 0)
                entry->call({});
            else if (const auto args_count = entry->get_frame_template().get_args().count(); args_count == 1) {
                // Convert vector<string> to ObjArray
                auto array = halloc_mgr<ObjArray>(manager, args.size());
                for (size_t i = 0; i < args.size(); ++i) array->set(i, halloc_mgr<ObjString>(manager, args[i]));
                entry->call(vector<Obj *>{array});
            } else
                throw runtime_error("entry point must have zero or one argument (basic.array): " + entry->get_sign().to_string());
            // Enter execution loop
            run(thread);
            // Try to compile to native
            // jit_test(entry);

        } catch (const SpadeError &error) {
            std::cout << "VM Error: " << error.what() << std::endl;
            exit_code = 1;
            return;
        }

        // Remove this thread after execution
        threads.erase(thread);
        // If it is empty then cleanup
        if (threads.empty()) {
            for (const auto &action: on_exit_list) action();
            exit_code = 0;
        }
    }

    void SpadeVM::start(const string &filename, const vector<string> &args, bool block) {
        Thread thread(this, std::bind(&SpadeVM::vm_main, this, filename, args, std::placeholders::_1), [&] {
            // Insert thread into vm threads before the thread starts
            threads.insert(&thread);
        });

        if (block)
            thread.join();
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
            obj = it->second;
        else {
            if (strict)
                throw IllegalAccessError(std::format("cannot find symbol: {}", sign));
            else
                return null;
        }
        for (size_t i = 1; i < elements.size(); ++i) {
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
        for (size_t i = 1; i < elements.size(); ++i)
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

    Type *SpadeVM::get_vm_type(ObjTag tag) {
        switch (tag) {
            case ObjTag::NULL_:
                return cast<Type>(get_symbol("basic.any"));
            case ObjTag::BOOL:
                return cast<Type>(get_symbol("basic.bool"));
            case ObjTag::CHAR:
                return cast<Type>(get_symbol("basic.char"));
            case ObjTag::STRING:
                return cast<Type>(get_symbol("basic.string"));
            case ObjTag::INT:
                return cast<Type>(get_symbol("basic.int"));
            case ObjTag::FLOAT:
                return cast<Type>(get_symbol("basic.float"));
            case ObjTag::ARRAY:
                return cast<Type>(get_symbol("basic.array[T]"));
            case ObjTag::OBJECT:
                return cast<Type>(get_symbol("basic.any"));
            default:
                return null;
        }
    }

    bool SpadeVM::check_cast(const Type *type1, const Type *type2) {
        (void) type1;
        (void) type2;

        // TODO implement this
        return false;
    }

    SpadeVM *SpadeVM::current() {
        if (const auto thread = Thread::current())
            return thread->get_state()->get_vm();
        return null;
    }
}    // namespace spade
