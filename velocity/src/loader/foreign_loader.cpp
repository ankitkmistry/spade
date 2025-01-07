#include "foreign_loader.hpp"

#include <ranges>

#if defined OS_WINDOWS

string getErrorMessage(DWORD errorCode) {
    LPVOID errMsgBuf;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | // Allocates a buffer for the message
                   FORMAT_MESSAGE_FROM_SYSTEM |     // Searches the system message table
                   FORMAT_MESSAGE_IGNORE_INSERTS,   // Ignores insert sequences in the message definition.
                   null,                            // Handle to the module containing the message table
                   errorCode,                       // Error code to format
                   0,                               // Default language
                   (LPSTR) &errMsgBuf,              // Output buffer for the formatted message
                   0,                               // Minimum size of the output buffer
                   null);                           // No arguments for insert sequences
    auto size = strlen((LPSTR) errMsgBuf);
    // Allocate a new buffer
    auto errMsg = new char[size];
    // Copy the buffer
    memcpy(errMsg, errMsgBuf, size);
    // Free the old buffer
    LocalFree(errMsgBuf);
    // Build the string
    string msgStr = errMsg;
    return msgStr;
}

string getErrorMessage(DWORD errorCode, HMODULE module) {
    LPVOID errMsgBuf;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | // Allocates a buffer for the message
                   FORMAT_MESSAGE_FROM_HMODULE |
                   // Indicates that the message definition is in the module
                   FORMAT_MESSAGE_FROM_SYSTEM |
                   // Searches the system message table if the message is not found in the module
                   FORMAT_MESSAGE_IGNORE_INSERTS,   // Ignores insert sequences in the message definition.
                   module,                          // Handle to the module containing the message table
                   errorCode,                       // Error code to format
                   0,                               // Default language
                   (LPSTR) &errMsgBuf,              // Output buffer for the formatted message
                   0,                               // Minimum size of the output buffer
                   null);                           // No arguments for insert sequences
    auto size = strlen((LPSTR) errMsgBuf);
    // Allocate a new buffer
    auto errMsg = new char[size];
    // Copy the buffer
    memcpy(errMsg, errMsgBuf, size);
    // Free the old buffer
    LocalFree(errMsgBuf);
    // Build the string
    string msgStr = errMsg;
    return msgStr;
}

void Library::unload() {
    FreeLibrary(module);
}

Library *ForeignLoader::loadSimpleLibrary(string path) {
    // TODO: Add advanced lookup
    HMODULE module = LoadLibrary(path.c_str());
    if (module == null) {
        DWORD errorCode = GetLastError();
        auto errMsg = getErrorMessage(errorCode);
        throw spade::NativeLibraryError(path, format("%s (0x%X)", errMsg.c_str(), errorCode));
    }
    auto library = new Library(Library::Kind::SIMPLE, path, module);
    libraries[path] = library;
    return library;
}

#elif defined OS_LINUX

void Library::unload() {
    dlclose(module);
}

Library *ForeignLoader::loadSimpleLibrary(string path) {
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

void ForeignLoader::unloadLibraries() {
    for (auto library: libraries | std::views::values) {
        library->unload();
    }
}
