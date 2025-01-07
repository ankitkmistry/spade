#ifndef VELOCITY_MODULE_HPP
#define VELOCITY_MODULE_HPP

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
        vector<Obj *> constantPool;
        /// The dependencies of the module
        vector<string> dependencies;
        /// The information in the module
        ElpInfo elp;
        /// The module init method
        ObjMethod *init = null;

      public:
        ObjModule(const Sign &sign, const fs::path &path, const vector<Obj *> &constantPool, const ElpInfo &elp);

        static ObjModule *current();

        string getAbsolutePath();

        string getModuleName() const;

        State getState() const { return state; }

        void setState(State state_) { state = state_; }

        const fs::path &getPath() const { return path; }

        ElpInfo getElp() const { return elp; }

        const vector<Obj *> &getConstantPool() const { return constantPool; }

        const vector<string> &getDependencies() const { return dependencies; }

        ObjMethod *getInit() const { return init; }

        void setInit(ObjMethod *init_) { init = init_; }

        Obj *copy() override;

        bool truth() const override;

        string toString() const override;
    };
} // namespace spade

#endif // VELOCITY_MODULE_HPP
