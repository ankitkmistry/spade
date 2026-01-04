#pragma once

#include "utils/common.hpp"
#include <cstddef>

namespace spade
{
    class Library {
        size_t *ref_count;
        string path;
        void *handle;

      public:
        Library(const string &path, void *handle);

        Library() = delete;
        Library(const Library &other);
        Library(Library &&other);
        Library &operator=(const Library &other);
        Library &operator=(Library &&other);
        ~Library();

        bool is_valid() const {
            return ref_count != null && handle != null;
        }

        operator bool() const {
            return is_valid();
        }

        void *get_symbol(const string &name);

        const string &get_path() const {
            return path;
        }

      private:
        void close_handle();
    };

    class ExternalLoader {
        Table<Library> libraries;

      public:
        Library load_library(fs::path path);
    };
}    // namespace spade
