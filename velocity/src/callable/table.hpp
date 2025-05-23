#pragma once

#include "objects/inbuilt_types.hpp"
#include "objects/obj.hpp"
#include <cstddef>

namespace spade
{
    /**
     * Represents the base class for nodes used in arg tables, local tables, etc.
     */
    class NamedRef final {
      protected:
        string name;
        Obj *value;
        Table<string> meta;

      public:
        NamedRef(const string &name, Obj *value, const Table<string> &meta) : name(name), value(value), meta(meta) {}

        NamedRef(const NamedRef &other);
        NamedRef(NamedRef &&other) noexcept;
        NamedRef &operator=(const NamedRef &other);
        NamedRef &operator=(NamedRef &&other) noexcept;
        ~NamedRef() = default;

        /**
         * Sets the value of the named ref
         * @param val
         */
        void set_value(Obj *val) {
            value = val;
        }

        /**
         * @return The value of the named ref
         */
        Obj *get_value() const {
            return value;
        }

        /**
         * @return The name attached to the named ref
         */
        const string &get_name() const {
            return name;
        }

        /**
         * @return The metadata associated to the named ref
         */
        const Table<string> &get_meta() const {
            return meta;
        }

        /**
         * @return The string representation of the named ref
         */
        string to_string() const {
            return name;
        }
    };

    /**
     * Represents an exception in the exception table
     */
    class Exception {
      private:
        uint32 from, to, target;
        Type *type;
        Table<string> meta;

      public:
        Exception(uint32 from, uint32 to, uint32 target, Type *type, Table<string> meta)
            : from(from), to(to), target(target), type(type), meta(std::move(meta)) {}

        Exception(const Exception &other) = default;
        Exception(Exception &&other) noexcept = default;
        Exception &operator=(const Exception &other) = default;
        Exception &operator=(Exception &&other) noexcept = default;
        ~Exception() = default;

        /**
         * @return The starting point <i>(of the try statement in code)</i>
         */
        uint32 get_from() const {
            return from;
        }

        /**
         * @return The ending point <i>(of the try statement in code)</i>
         */
        uint32 get_to() const {
            return to;
        }

        /**
         * @return The target point <i>(or the starting of the catch block)</i>
         */
        uint32 get_target() const {
            return target;
        }

        /**
         * @return The object representing the type of the exception
         */
        Type *get_type() const {
            return type;
        }

        /**
         * Sets the exception object of the exception handle
         * @param type_ the exception type object
         */
        void set_type(Type *type_) {
            type = type_;
        }

        /**
         * @return The metadata associated to the node
         */
        Table<string> get_meta() const {
            return meta;
        }

        static Exception NO_EXCEPTION() {
            return {0, 0, 0, null, {}};
        }

        static bool IS_NO_EXCEPTION(const Exception &exception) {
            return exception.type == null;
        }
    };

    /**
     * Represents a case in a match statement
     */
    class Case {
      private:
        Obj *value = null;
        uint32 location = -1;

      public:
        Case(Obj *value, uint32 location) : value(value), location(location) {}

        Case() = default;
        Case(const Case &other) = default;
        Case(Case &&other) noexcept = default;
        Case &operator=(const Case &other) = default;
        Case &operator=(Case &&other) noexcept = default;
        ~Case() = default;

        /**
         * @return The value to be matched
         */
        Obj *get_value() const {
            return value;
        }

        /**
         * @return The destination location in the code
         */
        uint32 get_location() const {
            return location;
        }
    };

    /**
     * Represents the argument table
     */
    class ArgsTable {
        friend class BasicCollector;

        friend class FrameTemplate;

      private:
        vector<NamedRef> args;

      public:
        ArgsTable() = default;
        ArgsTable(const ArgsTable &other) = default;
        ArgsTable(ArgsTable &&other) noexcept = default;
        ArgsTable &operator=(const ArgsTable &other) = default;
        ArgsTable &operator=(ArgsTable &&other) noexcept = default;
        ~ArgsTable() = default;

        /**
         * Sets the value of the argument at index i to val
         * @param i the argument index
         * @param val value to be changed
         */
        void set(uint8 i, Obj *val);

        /**
         * @return The value of the argument at index i
         * @param i the argument index
         */
        Obj *get(uint8 i) const;

        /**
         * Adds a new argument at the end of the table
         * @param arg the argument to be added
         */
        void add_arg(const NamedRef &arg) {
            args.push_back(arg);
        }

        /**
         * @return The argument at index i
         * @param i the argument index
         */
        const NamedRef &get_arg(uint8 i) const {
            return args[i];
        }

        /**
         * @return The argument at index i
         * @param i the argument index
         */
        NamedRef &get_arg(uint8 i) {
            return args[i];
        }

        ArgsTable copy() const;

        /**
         * @return The total number of arguments present
         */
        uint8 count() const {
            return args.size() & 0xff;
        }

        /**
         * @return The string representation of the table
         */
        string to_string() const {
            return "(" + list_to_string(args) + ")";
        }
    };

    /**
     * Represents the locals table
     */
    class LocalsTable {
        friend class BasicCollector;

        friend class FrameTemplate;

      private:
        uint16 closureStart;
        vector<NamedRef> locals;
        vector<NamedRef *> closures;

      public:
        explicit LocalsTable(uint16 closureStart) : closureStart(closureStart), locals() {}

        LocalsTable(const LocalsTable &other) = default;
        LocalsTable(LocalsTable &&other) noexcept = default;
        LocalsTable &operator=(const LocalsTable &other) = default;
        LocalsTable &operator=(LocalsTable &&other) noexcept = default;
        ~LocalsTable() = default;

        /**
         * @return The index of the locals table starting from which closures are stored up to the end of the table
         */
        uint16 get_closure_start() const {
            return closureStart;
        }

        /**
         * Sets the value of the local at index i to val.
         * If @p setAsThisRef is true, marks the local as this
         * @param i the local index
         * @param val value to be changed
         */
        void set(uint16 i, Obj *val);

        /**
         * @return The value of the local at index i
         * @param i the local index
         */
        Obj *get(uint16 i) const;

        /**
         * Adds a new local at the end of the table
         * @param local the local to be added
         */
        void add_local(NamedRef local) {
            locals.push_back(local);
        }

        /**
         * Adds a new closure at the end of the table
         * @param closure the closure to be added
         */
        void add_closure(NamedRef *closure) {
            closures.push_back(closure);
        }

        /**
         * @return The local at index i
         * @param i the local index
         */
        const NamedRef &get_local(uint16 i) const;

        /**
         * @return The local at index i
         * @param i the local index
         */
        NamedRef &get_local(uint16 i);

        /**
         * @return The closure at index i
         * @param i the closure index
         */
        NamedRef *get_closure(uint16 i) const;

        LocalsTable copy() const;

        /**
         * @return The total number of locals and closures present
         */
        uint16 count() const {
            return locals.size() + closures.size() & 0xffff;
        }

        /**
         * @return The string representation of the table
         */
        string to_string() const {
            return "(" + list_to_string(locals) + ")";
        }
    };

    class ExceptionTable {
        friend class BasicCollector;

        friend class FrameTemplate;

      private:
        vector<Exception> exceptions;

      public:
        ExceptionTable() = default;
        ExceptionTable(const ExceptionTable &other) = default;
        ExceptionTable(ExceptionTable &&other) noexcept = default;
        ExceptionTable &operator=(const ExceptionTable &other) = default;
        ExceptionTable &operator=(ExceptionTable &&other) noexcept = default;
        ~ExceptionTable() = default;

        /**
         * Adds a new exception at the end of the table
         * @param exception the exception to be added
         */
        void add_exception(const Exception &exception) {
            exceptions.push_back(exception);
        }

        /**
         * @return The exception at index i
         * @param i the exception index
         */
        const Exception &get(int i) const {
            return exceptions[i];
        }

        /**
         * @return The total number of exceptions
         */
        uint8 count() const {
            return exceptions.size() & 0xff;
        }

        /**
         * @return The exception that catches the program execution at pc and a throwable of type
         * @param pc the program counter
         * @param type the type of the throwable
         */
        Exception get_target(uint32 pc, const Type *type) const;
    };

    /**
     * Represents a table which stores the corresponding line numbers
     * of the source code and byte code. It is used for printing stack trace and debugging purposes
     */
    class LineNumberTable {
      public:
        struct LineInfo {
            uint32 sourceLine;
            uint16 byteStart;
            uint16 byteEnd;
        };

      private:
        vector<LineInfo> line_infos;

      public:
        LineNumberTable() = default;
        LineNumberTable(const LineNumberTable &other) = default;
        LineNumberTable(LineNumberTable &&other) noexcept = default;
        LineNumberTable &operator=(const LineNumberTable &other) = default;
        LineNumberTable &operator=(LineNumberTable &&other) noexcept = default;
        ~LineNumberTable() = default;

        /**
         * Adds a line info at the end of the table
         * @param times the line number in the bytecode
         * @param sourceLine the line number in the source code
         */
        void add_line(uint8 times, uint32 source_line);

        /**
         * @return The corresponding line number in the source code
         * @param byteLine the line number in the bytecode
         */
        uint64 get_source_line(uint32 byte_line) const;

        const vector<LineInfo> &get_line_infos() const {
            return line_infos;
        }
    };

    /**
     * Represents a check table
     */
    class MatchTable {
        friend class BasicCollector;

      private:
        struct ObjEqual {
            bool operator()(Obj *lhs, Obj *rhs) const;
        };

        struct ObjHash {
            void hash_combine(size_t &seed, ObjArray *arr) const;
            size_t operator()(Obj *obj) const;
        };

        std::unordered_map<Obj *, uint32, ObjHash, ObjEqual> table;
        uint32 default_location;

      public:
        MatchTable(const vector<Case> &cases, uint32 default_location);

        MatchTable(const MatchTable &other) = default;
        MatchTable(MatchTable &&other) noexcept = default;
        MatchTable &operator=(const MatchTable &other) = default;
        MatchTable &operator=(MatchTable &&other) noexcept = default;
        ~MatchTable() = default;

        /**
         * @return The default location of the check table <i>(starting of the default block)</i>
         */
        uint32 get_default_location() const {
            return default_location;
        }

        std::unordered_map<Obj *, uint32, ObjHash, ObjEqual> get_table() const {
            return table;
        }

        /**
         * @return The number of the check cases
         */
        size_t count() const {
            return table.size();
        }

        /**
         * This function takes value and serially checks all the cases
         * and finds the case which matches with the value. Then the function returns
         * the destination location of the matched case.<br>
         * The time complexity of this operation is O(n)
         * <br><br>
         * This function is optimized for integers and performs index search
         * on the cases array. The optimized operation takes only O(1) time.
         * @param value value to be matched
         * @return The destination location
         */
        uint32 perform(Obj *value) const;
    };
}    // namespace spade
