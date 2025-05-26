#pragma once

#include "memory/manager.hpp"
#include "memory/memory.hpp"
#include "utils/common.hpp"
#include "obj.hpp"

namespace spade
{
    class ObjBool : public ComparableObj {
      private:
        bool b;

      public:
        ObjBool(bool value);

        static ObjBool *value(bool b, MemoryManager *manager = null);

        bool truth() const override {
            return b;
        }

        string to_string() const override {
            return b ? "true" : "false";
        }

        Obj *copy() const override {
            // immutable state
            return (Obj *) this;
        }

        int32 compare(const Obj *rhs) const override;

        ObjBool *operator!() const {
            return halloc_mgr<ObjBool>(info.manager, !b);
        }
    };

    class ObjChar : public ComparableObj {
      private:
        char c;

      public:
        ObjChar(const char c);

        bool truth() const override {
            return c != '\0';
        }

        string to_string() const override {
            return string({c});
        }

        Obj *copy() const override {
            // immutable state
            return (Obj *) this;
        }

        int32 compare(const Obj *rhs) const override;
    };

    class ObjNull : public ComparableObj {
      public:
        ObjNull(ObjModule *module = null);

        static ObjNull *value(MemoryManager *manager = null);

        bool truth() const override {
            return false;
        }

        string to_string() const override {
            return "null";
        }

        Obj *copy() const override {
            // immutable state
            return (Obj *) this;
        }

        int32 compare(const Obj *rhs) const override;
    };

    class ObjString : public ComparableObj {
      private:
        string str;

      public:
        ObjString(string str);

        ObjString(const uint8 *bytes, uint16 len);

        bool truth() const override {
            return !str.empty();
        }

        string to_string() const override {
            return str;
        }

        Obj *copy() const override {
            // immutable state
            return (Obj *) this;
        }

        int32 compare(const Obj *rhs) const override;
    };

    class ObjArray final : public ComparableObj {
      private:
        Obj **array;
        uint16 length;

      public:
        explicit ObjArray(uint16 length);

        void foreach (const std::function<void(Obj *)> &func) const;

        Obj *get(int64 i) const;

        void set(int64 i, Obj *value);

        uint16 count() const {
            return length;
        }

        bool truth() const override {
            return length != 0;
        }

        string to_string() const override;

        Obj *copy() const override;

        /// Does lexicographical comparison
        int32 compare(const Obj *rhs) const override;
    };

    class ObjNumber : public ComparableObj {
      protected:
        ObjNumber() : ComparableObj(null) {}

      public:
        virtual Obj *operator-() const = 0;

        virtual Obj *power(const ObjNumber *n) const = 0;

        virtual Obj *operator+(const ObjNumber *n) const = 0;

        virtual Obj *operator-(const ObjNumber *n) const = 0;

        virtual Obj *operator*(const ObjNumber *n) const = 0;

        virtual Obj *operator/(const ObjNumber *n) const = 0;
    };
}    // namespace spade
