#pragma once

#include "../callable/method.hpp"

namespace spade
{
    class ObjModule : public Obj {
      private:
        /// Path of the module
        fs::path path;
        /// The constant pool of the module
        vector<Obj *> constant_pool;
        /// The module init method
        ObjMethod *init = null;

      public:
        ObjModule(const Sign &sign, const fs::path &path, const vector<Obj *> &constant_pool, const Table<MemberSlot> &member_slots);

        static ObjModule *current();

        const fs::path &get_path() {
            return path;
        }

        string get_module_name() const {
            return path.stem().string();
        }

        const fs::path &get_path() const {
            return path;
        }

        const vector<Obj *> &get_constant_pool() const {
            return constant_pool;
        }

        ObjMethod *get_init() const {
            return init;
        }

        void set_init(ObjMethod *init_) {
            init = init_;
        }

        Obj *copy()const override;

        bool truth() const override;

        string to_string() const override;
    };
}    // namespace spade
