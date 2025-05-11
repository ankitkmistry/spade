#pragma once

#include "../callable/method.hpp"

namespace spade
{
    class ObjModule : public Obj {
      public:
        enum class State { NOT_READ, READ, LOADED, INITIALIZED };

      private:
        /// State of the module
        State state = State::NOT_READ;
        /// Path of the module
        fs::path path;
        /// The constant pool of the module
        vector<Obj *> constant_pool;
        /// The dependencies of the module
        vector<string> dependencies;
        /// The information in the module
        ElpInfo elp;
        /// The module init method
        ObjMethod *init = null;

      public:
        ObjModule(const Sign &sign, const fs::path &path, const vector<Obj *> &constant_pool, const vector<string> &dependencies,
                  const ElpInfo &elp);

        static ObjModule *current();

        string get_absolute_path();

        string get_module_name() const;

        State get_state() const {
            return state;
        }

        void set_state(State state_) {
            state = state_;
        }

        const fs::path &get_path() const {
            return path;
        }

        ElpInfo get_elp() const {
            return elp;
        }

        const vector<Obj *> &get_constant_pool() const {
            return constant_pool;
        }

        const vector<string> &get_dependencies() const {
            return dependencies;
        }

        ObjMethod *get_init() const {
            return init;
        }

        void set_init(ObjMethod *init_) {
            init = init_;
        }

        Obj *copy() override;

        bool truth() const override;

        string to_string() const override;
    };
}    // namespace spade
