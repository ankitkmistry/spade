#pragma once

#include "objects/obj.hpp"

namespace spade
{
    class ObjModule : public Obj {
      private:
        Sign sign;
        /// Path of the module
        fs::path path;
        /// The constant pool of the module
        vector<Obj *> constant_pool;
        /// The module init method
        ObjMethod *init = null;

      public:
        static ObjModule *current();

        ObjModule(const Sign &sign);

        const Sign &get_sign() const {
            return sign;
        }

        void set_sign(const Sign &sign) {
            this->sign = sign;
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

        void set_init(ObjMethod *init) {
            this->init = init;
        }

        Obj *copy() const override;

        bool truth() const override;

        string to_string() const override;
    };
}    // namespace spade
