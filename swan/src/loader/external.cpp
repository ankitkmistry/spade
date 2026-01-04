#include "utils/common.hpp"
#include <cassert>

#ifdef OS_WINDOWS
#    include <windows.h>
#endif

#ifdef OS_LINUX
#    include <dlfcn.h>
#endif

#include "external.hpp"
#include "utils/errors.hpp"

namespace spade
{
#ifdef OS_WINDOWS
    string get_last_error() {
        char err_msg_buf[4096];
        DWORD size = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM                  // Searches the system message table
                                            | FORMAT_MESSAGE_IGNORE_INSERTS,    // Ignores insert sequences in the message definition.
                                    NULL,                                       // Handle to the module containing the message table
                                    GetLastError(),                             // Error code to format
                                    0,                                          // Default language
                                    err_msg_buf,                                // Output buffer for the formatted message
                                    4096,                                       // Minimum size of the output buffer
                                    NULL                                        // No arguments for insert sequences
        );
        if (size == 0)
            return get_last_error();
        return std::string(err_msg_buf, size);
    }

    string get_last_error(HMODULE module) {
        char err_msg_buf[4096];
        DWORD size = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM                  // Searches the system message table
                                            | FORMAT_MESSAGE_IGNORE_INSERTS,    // Ignores insert sequences in the message definition.
                                    module,                                     // Handle to the module containing the message table
                                    GetLastError(),                             // Error code to format
                                    0,                                          // Default language
                                    err_msg_buf,                                // Output buffer for the formatted message
                                    4096,                                       // Minimum size of the output buffer
                                    NULL                                        // No arguments for insert sequences
        );
        if (size == 0)
            return get_last_error(module);
        return std::string(err_msg_buf, size);
    }
#endif

    Library::Library(const string &path, void *handle) : ref_count(new std::atomic_size_t(1)), path(path), handle(handle) {}

    Library::Library(const Library &other) : ref_count(other.ref_count), path(other.path), handle(other.handle) {
        if (ref_count)
            *ref_count += 1;
    }

    Library::Library(Library &&other) : ref_count(other.ref_count), path(other.path), handle(other.handle) {
        other.ref_count = null;
        other.path.clear();
        other.handle = null;
    }

    Library &Library::operator=(const Library &other) {
        ref_count = other.ref_count;
        path = other.path;
        handle = other.handle;

        if (ref_count)
            *ref_count += 1;

        return *this;
    }

    Library &Library::operator=(Library &&other) {
        ref_count = other.ref_count;
        path = other.path;
        handle = other.handle;

        other.ref_count = null;
        other.path.clear();
        other.handle = null;

        return *this;
    }

    Library::~Library() {
        if (ref_count == null) {
            assert(handle == null);
            return;
        }

        *ref_count -= 1;
        if (*ref_count == 0) {
            delete ref_count;
            close_handle();
        }
    }

    void *Library::get_symbol(const string &name) {
        if (!is_valid())
            return null;

#ifdef OS_WINDOWS
        const HMODULE module = static_cast<HMODULE>(handle);
        void *symbol = (void *) GetProcAddress(module, name.c_str());
        if (symbol == null) {
            const auto code = GetLastError();
            const auto msg = get_last_error();
            throw NativeLibraryError(path, name, std::format("{} ({:#0x})", msg, code));
        }
        return symbol;
#endif

#ifdef OS_LINUX
        void *symbol = dlsym(handle, name.c_str());
        if (symbol == null)
            throw NativeLibraryError(path, name, string(dlerror()));
        return symbol;
#endif
    }

    void Library::close_handle() {
        if (!is_valid())
            return;

#ifdef OS_WINDOWS
        const HMODULE module = static_cast<HMODULE>(handle);
        if (FreeLibrary(module) == 0) {
            const auto code = GetLastError();
            const auto msg = get_last_error();
            throw NativeLibraryError(path, std::format("{} ({:#0x})", msg, code));
        }
        handle = null;
#endif

#ifdef OS_LINUX
        if (dlclose(handle) != 0)
            throw NativeLibraryError(path, string(dlerror()));
#endif
    }

    Library ExternalLoader::load_library(fs::path path) {
        // TODO: Add advanced lookup
        path = fs::canonical(path);
        const auto path_str = path.string();

        if (const auto it = libraries.find(path_str); it != libraries.end())
            return it->second;

#ifdef OS_WINDOWS
        const HMODULE module = LoadLibraryA(path_str.c_str());
        if (module == null) {
            const auto code = GetLastError();
            const auto msg = get_last_error();
            throw NativeLibraryError(path_str, std::format("{} ({:#0x})", msg, code));
        }

        Library library(path_str, module);
        libraries.emplace(path_str, library);
        return library;
#endif

#ifdef OS_LINUX
        const auto lib = dlopen(path_str.c_str(), RTLD_LAZY);
        if (lib == null)
            throw NativeLibraryError(path, string(dlerror()));

        Library library(path_str, lib);
        libraries.emplace(path_str, library);
        return library;
#endif
    }
}    // namespace spade
