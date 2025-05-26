#pragma once

#include "utils/common.hpp"
#include "utils/exceptions.hpp"

#if defined OS_WINDOWS

#    include <windows.h>

#elif defined OS_LINUX

#    include <dlfcn.h>

#endif

#if defined OS_WINDOWS

string get_error_message(DWORD error_code);

string get_error_message(DWORD error_code, HMODULE module);

class Library {
  public:
    enum class Kind { SIMPLE, FOREIGN };

  private:
    Kind kind;
    string name;
    HMODULE module;

  public:
    Library(Kind kind, const string &name, HMODULE module) : kind(kind), name(name), module(module) {}

    Kind get_kind() const {
        return kind;
    }

    const string &get_name() const {
        return name;
    }

    const HMODULE get_module() const {
        return module;
    }

    template<typename ReturnType, typename... Args>
    ReturnType call(const string &function_name, Args... args) {
        using FunctionType = ReturnType(CALLBACK *)(Args...);
        FunctionType function = (FunctionType) GetProcAddress(module, function_name.c_str());
        if (function == null) {
            DWORD error_code = GetLastError();
            string err_msg = get_error_message(error_code, module);
            throw spade::NativeLibraryError(name, function_name, std::format("error code {}: {}", error_code, err_msg));
        }
        return function(args...);
    }

    void unload();
};

#elif defined OS_LINUX

class Library {
  public:
    enum class Kind { SIMPLE, FOREIGN };

  private:
    Kind kind;
    string name;
    void *module;

  public:
    Library(Kind kind, string name, void *module) : kind(kind), name(name), module(module) {}

    Kind getKind() const {
        return kind;
    }

    const string &getName() const {
        return name;
    }

    const void *getModule() const {
        return module;
    }

    template<typename ReturnType, typename... Args>
    ReturnType call(string function_name, Args... args) {
        using FunctionType = ReturnType (*)(Args...);
        FunctionType function = (FunctionType) dlsym(module, function_name.c_str());
        if (function == null) {
            throw spade::NativeLibraryError(name, function_name, dlerror());
        }
        return function(args...);
    }

    void unload();
};

#endif

class ForeignLoader {
  private:
    inline static spade::Table<Library *> libraries;

  public:
    static Library *load_simple_library(const string &path);

    static void unload_libraries();
};
