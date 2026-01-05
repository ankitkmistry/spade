#include "vm.hpp"
#include "callable/foreign.hpp"
#include "utils/errors.hpp"
#include "memory/memory.hpp"
#include "loader/loader.hpp"
#include "spimp/utils.hpp"
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <spdlog/spdlog.h>

namespace spade
{
    SpadeVM::SpadeVM(MemoryManager *manager, std::unique_ptr<Debugger> debugger, const Settings &settings)
        : modules(),
          threads(),
          manager(manager),
          loader(this),
          on_exit_list(),
          settings(settings),
          metadata(),
          metadata_mtx(),
          exit_code(1),
          out(),
          debugger(std::move(debugger)) {
        manager->set_vm(this);
    }

    void SpadeVM::on_exit(const std::function<void()> &fun) {
        spdlog::info("SpadeVM: registered exit hook");
        on_exit_list.push_back(fun);
    }

    void SpadeVM::start(const string &filename, const vector<string> &args, bool block) {
        Thread thread(this, std::bind(&SpadeVM::vm_main, this, filename, args, std::placeholders::_1), [&] {
            spdlog::info("SpadeVM: Thread registered in the vm");
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
        const Sign symbol_sign(sign);
        if (symbol_sign.empty())
            return null;

        const auto &elements = symbol_sign.get_elements();
        size_t i = 0;
        Obj *obj;

        if (const auto it = modules.find(elements[i++].to_string()); it != modules.end())
            obj = it->second;
        else {
            if (strict)
                throw IllegalAccessError(std::format("cannot find symbol: {}", sign));
            else
                return null;
        }

        for (; i < elements.size(); ++i) {
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

    void SpadeVM::set_symbol(const string &sign, Obj *val) {
        const Sign symbol_sign(sign);
        if (symbol_sign.empty())
            return;

        const auto &elements = symbol_sign.get_elements();
        size_t i = 0;
        Obj *obj;

        if (const auto it = modules.find(elements[i++].to_string()); it != modules.end())
            obj = it->second;
        else {
            if (i >= elements.size())
                modules[sign] = cast<ObjModule>(val);
            else
                throw IllegalAccessError(std::format("cannot find symbol: {}", sign));
        }

        for (; i < elements.size(); i++) {
            if (i >= elements.size() - 1) {
                obj->set_member(elements.back().to_string(), val);
            } else {
                try {
                    obj = obj->get_member(elements[i].to_string());
                } catch (const IllegalAccessError &) {
                    throw IllegalAccessError(std::format("cannot find symbol: {}", sign));
                }
            }
        }
    }

    const Table<string> &SpadeVM::get_metadata(const string &sign) {
        {
            std::shared_lock metadata_lk(metadata_mtx);
            if (const auto it = metadata.find(sign); it != metadata.end())
                return it->second;
        }
        throw IllegalAccessError(std::format("cannot find metadata: {}", sign));
    }

    void SpadeVM::set_metadata(const string &sign, const Table<string> &meta) {
        std::unique_lock metadata_lk(metadata_mtx);
        metadata[sign] = meta;
    }

    Type *SpadeVM::get_vm_type(ObjTag tag) {
        switch (tag) {
        case ObjTag::NULL_OBJ:
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

    SpadeVM *SpadeVM::current() {
        if (const auto thread = Thread::current())
            return thread->get_vm();
        return null;
    }

    void SpadeVM::load_basic() {
        if (modules.contains("basic"))
            return;

        const auto module = halloc_mgr<ObjModule>(manager, Sign("basic"));

        const auto type_any = halloc_mgr<Type>(manager, Type::Kind::CLASS, Sign("basic.any"), Table<Type *>{}, vector<Sign>{});
        const vector<Sign> supers = {type_any->get_sign()};

        const auto type_Enum = halloc_mgr<Type>(manager, Type::Kind::CLASS, Sign("basic.Enum"), Table<Type *>{}, supers);
        const auto type_Annotation = halloc_mgr<Type>(manager, Type::Kind::CLASS, Sign("basic.Annotation"), Table<Type *>{}, supers);
        const auto type_Throwable = halloc_mgr<Type>(manager, Type::Kind::CLASS, Sign("basic.Throwable"), Table<Type *>{}, supers);

        const auto type_bool = halloc_mgr<Type>(manager, Type::Kind::CLASS, Sign("basic.bool"), Table<Type *>{}, supers);
        const auto type_int = halloc_mgr<Type>(manager, Type::Kind::CLASS, Sign("basic.int"), Table<Type *>{}, supers);
        const auto type_float = halloc_mgr<Type>(manager, Type::Kind::CLASS, Sign("basic.float"), Table<Type *>{}, supers);
        const auto type_char = halloc_mgr<Type>(manager, Type::Kind::CLASS, Sign("basic.char"), Table<Type *>{}, supers);
        const auto type_string = halloc_mgr<Type>(manager, Type::Kind::CLASS, Sign("basic.string"), Table<Type *>{}, supers);

        const Table<Type *> type_array_tps{
                {"[T]", halloc_mgr<Type>(manager, Sign("[T]"))}
        };
        const auto type_array = halloc_mgr<Type>(manager, Type::Kind::CLASS, Sign("basic.array[T]"), type_array_tps, supers);

        module->set_member("any", type_any);
        module->set_member("Enum", type_Enum);
        module->set_member("Annotation", type_Annotation);
        module->set_member("Throwable", type_Throwable);
        module->set_member("bool", type_bool);
        module->set_member("int", type_int);
        module->set_member("float", type_float);
        module->set_member("char", type_char);
        module->set_member("string", type_string);
        module->set_member("array[T]", type_array);

        modules["basic"] = module;

        spdlog::info("SpadeVM: Loaded basic module");
    }

    void SpadeVM::vm_main(const string &filename, const vector<string> &args, Thread *thread) {
        thread->set_status(Thread::RUNNING);
        spdlog::info("SpadeVM: Thread set to running");
        try {
            if (debugger) {
                debugger->init(this);
                spdlog::info("SpadeVM: Debugger initialized");
            }
            // Load the basic types and module
            load_basic();
            // Load the file and ge the entry point
            const auto result = loader.load(filename);
            const auto entry = result.entry;
            // Initialize the modules
            for (const auto init: result.inits) {
                init->call(null, {});
                run(thread);
                spdlog::info("SpadeVM: Called module initializer: {}", init->get_sign().to_string());
            }
            // TODO: inserting intrinsics
            // Sign sign("hello.clock()");
            // auto hello_clock = halloc_mgr<ObjForeign>(
            //         manager, sign,
            //         (void *) [](Thread * thread) {
            //             spdlog::info("hello.clock() from libffi");
            //             return halloc<ObjInt>(clock());
            //         },
            //         false);
            // set_symbol(sign.to_string(), hello_clock);
            // hello_clock->invoke(null, {});
            // Complain if there is no entry point
            if (entry == null)
                throw IllegalAccessError(std::format("cannot find entry point in '{}'", filename));
            // Call the function
            if (const auto args_count = entry->get_frame_template().get_args().count(); args_count == 0)
                entry->call(null, {});
            else if (const auto args_count = entry->get_frame_template().get_args().count(); args_count == 1) {
                // Convert vector<string> to ObjArray
                auto array = halloc_mgr<ObjArray>(manager, args.size());
                for (size_t i = 0; i < args.size(); ++i) array->set(i, halloc_mgr<ObjString>(manager, args[i]));
                entry->call(null, vector<Obj *>{array});
            } else
                throw runtime_error("entry point must have zero or one argument (basic.array): " + entry->get_sign().to_string());
            // Enter execution loop
            spdlog::info("SpadeVM: Calling entry point: {}", entry->get_sign().to_string());
            run(thread);
            // Mark the thread as terminated
            thread->set_status(Thread::Status::TERMINATED);
            spdlog::info("SpadeVM: Thread set to terminated");
            // Try to compile to native
            // jit_test(entry);

        } catch (const SpadeError &error) {
            std::cerr << "VM Error: " << error.what() << std::endl;
            exit_code = 1;
            std::exit(exit_code);
        }

        // Remove this thread after execution
        threads.erase(thread);
        spdlog::info("SpadeVM: Thread unregistered in the vm");
        // If it is empty then cleanup
        if (threads.empty()) {
            spdlog::info("SpadeVM: Cleaning up");
            for (const auto &action: on_exit_list) action();
            exit_code = thread->get_exit_code();
            if (debugger)
                debugger->cleanup(this);
            spdlog::info("SpadeVM: Exit");
        }
    }

    bool SpadeVM::check_cast(const Type *type1, const Type *type2) {
        (void) type1;
        (void) type2;

        // TODO: implement this
        return false;
    }
}    // namespace spade
