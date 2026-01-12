#pragma once

#include "utils/common.hpp"
#include <atomic>

namespace spade
{
    class SWAN_EXPORT Library {
        std::atomic_size_t *ref_count;
        fs::path path;
        void *handle;

      public:
        Library(const fs::path &path, void *handle);

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

        const fs::path &get_path() const {
            return path;
        }

      private:
        void close_handle();
    };

    class SWAN_EXPORT ExternalLoader {
        std::unordered_map<fs::path, Library> libraries;

      public:
        Library load_library(fs::path path);
    };
}    // namespace spade
