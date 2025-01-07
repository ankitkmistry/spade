#ifndef VELOCITY_FOREIGN_LOADER_HPP
#define VELOCITY_FOREIGN_LOADER_HPP

#include "../utils/common.hpp"
#include "../utils/exceptions.hpp"

#if defined OS_WINDOWS

#include <windows.h>

#elif defined OS_LINUX

#include <dlfcn.h>

#endif

#if defined OS_WINDOWS

string getErrorMessage(DWORD errorCode);

string getErrorMessage(DWORD errorCode, HMODULE module);

class Library {
public:
    enum class Kind {
        SIMPLE, FOREIGN
    };
private:
    Kind kind;
    string name;
    HMODULE module;
public:
    Library(Kind kind, string name, HMODULE module) : kind(kind), name(name), module(module) {}

    Kind getKind() const { return kind; }

    const string &getName() const { return name; }

    const HMODULE getModule() const { return module; }

    template<typename ReturnType, typename... Args>
    ReturnType call(string functionName, Args...args) {
        using FunctionType = ReturnType(CALLBACK *)(Args...);
        FunctionType function = (FunctionType) GetProcAddress(module, functionName.c_str());
        if (function == null) {
            DWORD errorCode = GetLastError();
            auto errMsg = getErrorMessage(errorCode, module);
            throw spade::NativeLibraryError(name, functionName, format("error code %d: %s", errorCode, errMsg.c_str()));
        }
        return function(args...);
    }

    void unload();
};

#elif defined OS_LINUX

class Library {
public:
    enum class Kind {
        SIMPLE, FOREIGN
    };
private:
    Kind kind;
    string name;
    void *module;
public:
    Library(Kind kind, string name, void *module) : kind(kind), name(name), module(module) {}

    Kind getKind() const { return kind; }

    const string &getName() const { return name; }

    const void *getModule() const { return module; }

    template<typename ReturnType, typename... Args>
    ReturnType call(string functionName, Args...args) {
        using FunctionType = ReturnType(*)(Args...);
        FunctionType function = (FunctionType) dlsym(module, functionName.c_str());
        if (function == null) {
            throw spade::NativeLibraryError(name, functionName, dlerror());
        }
        return function(args...);
    }

    void unload();
};

#endif

class ForeignLoader {
private:
    inline static spade::Table<Library *> libraries = {};
public:
    static Library *loadSimpleLibrary(string path);

    static void unloadLibraries();
};

#endif //VELOCITY_FOREIGN_LOADER_HPP
