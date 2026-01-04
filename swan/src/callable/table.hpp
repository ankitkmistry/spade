#pragma once

#include "ee/obj.hpp"
#include "spimp/utils.hpp"

namespace spade
{
    class VariableTable {
        vector<Obj *> values;
        vector<Table<string>> metas;

      public:
        VariableTable(size_t count) : values(count), metas(count) {}

        VariableTable() = default;
        VariableTable(const VariableTable &other);
        VariableTable(VariableTable &&other) noexcept;
        VariableTable &operator=(const VariableTable &other);
        VariableTable &operator=(VariableTable &&other) noexcept;
        ~VariableTable() = default;

        /**
         * Ramps up the value of the argument at index i to a pointer where the pointer 
         * points to the actual value of the argument before ramping up.
         * If the value was already ramped up, then the value is no longer ramped up
         * and the existing pointer is returned
         * @param i index of the argument
         * @return ObjPointer* the pointer obj
         */
        ObjCapture *ramp_up(uint8_t i);

        /**
         * @return The value of the argument at index i
         * @param i the argument index
         */
        Obj *get(uint8_t i) const;

        /**
         * Sets the value of the argument at index i to val
         * @param i the argument index
         * @param val value to be changed
         */
        void set(uint8_t i, Obj *val);

        /**
         * @param i index of the argument
         * @return The metadata associated to the argument at index i
         */
        const Table<string> &get_meta(uint8_t i) const;

        /**
         * @param i index of the argument
         * @return The metadata associated to the argument at index i
         */
        Table<string> &get_meta(uint8_t i);

        /**
         * Sets the metadata associated to the argument at index i
         * @param i index of the argument
         * @param meta the meta table to set
         */
        void set_meta(uint8_t i, const Table<string> &meta);

        /**
         * @return The total number of arguments present
         */
        uint8_t count() const {
            return values.size() & 0xff;
        }

        /**
         * @return The string representation of the table
         */
        string to_string() const {
            return "(" + list_to_string(values) + ")";
        }
    };

    /**
     * Represents an exception in the exception table
     */
    class Exception {
      private:
        uint32_t from, to, target;
        Type *type;
        Table<string> meta;

      public:
        Exception(uint32_t from, uint32_t to, uint32_t target, Type *type, Table<string> meta)
            : from(from), to(to), target(target), type(type), meta(std::move(meta)) {}

        Exception(const Exception &other) = default;
        Exception(Exception &&other) noexcept = default;
        Exception &operator=(const Exception &other) = default;
        Exception &operator=(Exception &&other) noexcept = default;
        ~Exception() = default;

        /**
         * @return The starting point <i>(of the try statement in code)</i>
         */
        uint32_t get_from() const {
            return from;
        }

        /**
         * @return The ending point <i>(of the try statement in code)</i>
         */
        uint32_t get_to() const {
            return to;
        }

        /**
         * @return The target point <i>(or the starting of the catch block)</i>
         */
        uint32_t get_target() const {
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
        uint8_t count() const {
            return exceptions.size() & 0xff;
        }

        /**
         * @return The exception that catches the program execution at pc and a throwable of type
         * @param pc the program counter
         * @param type the type of the throwable
         */
        Exception get_target(uint32_t pc, const Type *type) const;
    };

    /**
     * Represents a table which stores the corresponding line numbers
     * of the source code and byte code. It is used for printing stack trace and debugging purposes
     */
    class LineNumberTable {
      public:
        struct LineInfo {
            uint32_t sourceLine;
            uint16_t byteStart;
            uint16_t byteEnd;
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
        void add_line(uint8_t times, uint32_t source_line);

        /**
         * @return The corresponding line number in the source code
         * @param byteLine the line number in the bytecode
         */
        uint64_t get_source_line(uint32_t byte_line) const;

        const vector<LineInfo> &get_line_infos() const {
            return line_infos;
        }
    };

    /**
     * Represents a case in a match statement
     */
    class Case {
      private:
        Obj *value = null;
        uint32_t location = -1;

      public:
        Case(Obj *value, uint32_t location) : value(value), location(location) {}

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
        uint32_t get_location() const {
            return location;
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
            void hash(size_t &seed, Obj *obj) const;
            size_t operator()(Obj *obj) const;
        };

        std::unordered_map<Obj *, uint32_t, ObjHash, ObjEqual> table;
        uint32_t default_location;

      public:
        MatchTable(const vector<Case> &cases, uint32_t default_location);

        MatchTable(const MatchTable &other) = default;
        MatchTable(MatchTable &&other) noexcept = default;
        MatchTable &operator=(const MatchTable &other) = default;
        MatchTable &operator=(MatchTable &&other) noexcept = default;
        ~MatchTable() = default;

        /**
         * @return The default location of the check table <i>(starting of the default block)</i>
         */
        uint32_t get_default_location() const {
            return default_location;
        }

        std::unordered_map<Obj *, uint32_t, ObjHash, ObjEqual> get_table() const {
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
        uint32_t perform(Obj *value) const;
    };
}
