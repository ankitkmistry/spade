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
        ObjModule(const Sign &sign, ObjModule *current = null);

        static ObjModule *current();

        string get_module_name() const {
            return sign.get_name();
        }

        const fs::path &get_path() const {
            return path;
        }

        void set_path(const fs::path &path) {
            this->path = path;
        }

        const vector<Obj *> &get_constant_pool() const {
            return constant_pool;
        }

        void set_constant_pool(const vector<Obj *> &conpool) {
            constant_pool = conpool;
        }

        ObjMethod *get_init() const {
            return init;
        }

        void set_init(ObjMethod *init_) {
            init = init_;
        }

        Obj *copy() const override;

        bool truth() const override;

        string to_string() const override;
    };
}    // namespace spade
