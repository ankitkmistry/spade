#include <ranges>

#include "foreign_loader.hpp"

#if defined OS_WINDOWS

string get_error_message(DWORD error_code) {
    LPVOID err_msg_buf;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |          // Allocates a buffer for the message
                           FORMAT_MESSAGE_FROM_SYSTEM |      // Searches the system message table
                           FORMAT_MESSAGE_IGNORE_INSERTS,    // Ignores insert sequences in the message definition.
                   null,                                     // Handle to the module containing the message table
                   error_code,                               // Error code to format
                   0,                                        // Default language
                   (LPSTR) &err_msg_buf,                     // Output buffer for the formatted message
                   0,                                        // Minimum size of the output buffer
                   null);                                    // No arguments for insert sequences
    auto size = strlen((LPSTR) err_msg_buf);
    // Allocate a new buffer
    auto err_msg = new char[size];
    // Copy the buffer
    memcpy(err_msg, err_msg_buf, size);
    // Free the old buffer
    LocalFree(err_msg_buf);
    // Build the string
    string msg_str = err_msg;
    return msg_str;
}

string get_error_message(DWORD error_code, HMODULE module) {
    LPVOID err_msg_buf;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |    // Allocates a buffer for the message
                           FORMAT_MESSAGE_FROM_HMODULE |
                           // Indicates that the message definition is in the module
                           FORMAT_MESSAGE_FROM_SYSTEM |
                           // Searches the system message table if the message is not found in the module
                           FORMAT_MESSAGE_IGNORE_INSERTS,    // Ignores insert sequences in the message definition.
                   module,                                   // Handle to the module containing the message table
                   error_code,                               // Error code to format
                   0,                                        // Default language
                   (LPSTR) &err_msg_buf,                     // Output buffer for the formatted message
                   0,                                        // Minimum size of the output buffer
                   null);                                    // No arguments for insert sequences
    auto size = strlen((LPSTR) err_msg_buf);
    // Allocate a new buffer
    auto err_msg = new char[size];
    // Copy the buffer
    memcpy(err_msg, err_msg_buf, size);
    // Free the old buffer
    LocalFree(err_msg_buf);
    // Build the string
    string msg_str = err_msg;
    return msg_str;
}

void Library::unload() {
    FreeLibrary(module);
}

Library *ForeignLoader::load_simple_library(const string &path) {
    // TODO: Add advanced lookup
    const HMODULE module = LoadLibraryA(path.c_str());
    if (module == null) {
        const DWORD error_code = GetLastError();
        const auto err_msg = get_error_message(error_code);
        throw spade::NativeLibraryError(path, std::format("{} ({:#0x})", err_msg.c_str(), error_code));
    }
    auto library = new Library(Library::Kind::SIMPLE, path, module);
    libraries[path] = library;
    return library;
}

#elif defined OS_LINUX

void Library::unload() {
    dlclose(module);
}

Library *ForeignLoader::load_simple_library(const string &path) {
    // TODO: Add advanced lookup
    void *module = dlopen(path.c_str(), RTLD_LAZY);
    if (module == null) {
        throw spade::NativeLibraryError(path, dlerror());
    }
    auto library = new Library(Library::Kind::SIMPLE, path, module);
    libraries[path] = library;
    return library;
}

#endif

void ForeignLoader::unload_libraries() {
    for (auto library: libraries | std::views::values) {
        library->unload();
    }
}
