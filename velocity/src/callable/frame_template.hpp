#pragma once

#include <cstring>

#include "utils/common.hpp"
#include "objects/obj.hpp"
#include "frame.hpp"
#include "table.hpp"

namespace spade
{
    class ObjMethod;

    class FrameTemplate {
        uint32 code_count;
        uint8 *code;
        uint32 stack_max;
        ArgsTable args;
        LocalsTable locals;
        ExceptionTable exceptions;
        LineNumberTable lines;
        vector<MatchTable> matches;
        ObjMethod *method;

      public:
        FrameTemplate(const vector<uint8> &code, uint32 stack_max, const ArgsTable &args, const LocalsTable &locals, const ExceptionTable &exceptions,
                      const LineNumberTable &lines, const vector<MatchTable> &matches, ObjMethod *method = null)
            : code_count(static_cast<uint32>(code.size())),
              code(null),
              stack_max(stack_max),
              args(args),
              locals(locals),
              exceptions(exceptions),
              lines(lines),
              matches(matches),
              method(method) {
            this->code = new uint8[code_count];
            std::memcpy(this->code, code.data(), code_count * sizeof(uint8));
        }

        // Copy constructor
        FrameTemplate(const FrameTemplate &other)
            : code_count(other.code_count),
              code(new uint8[other.code_count]),
              stack_max(other.stack_max),
              args(other.args),
              locals(other.locals),
              exceptions(other.exceptions),
              lines(other.lines),
              matches(other.matches),
              method(other.method) {
            std::memcpy(code, other.code, code_count * sizeof(uint8));
        }

        // Move constructor
        FrameTemplate(FrameTemplate &&other) noexcept
            : code_count(other.code_count),
              code(other.code),
              stack_max(other.stack_max),
              args(std::move(other.args)),
              locals(std::move(other.locals)),
              exceptions(std::move(other.exceptions)),
              lines(std::move(other.lines)),
              matches(std::move(other.matches)),
              method(other.method) {
            other.code = null;
            other.code_count = 0;
            other.method = null;
        }

        // Copy assignment
        FrameTemplate &operator=(const FrameTemplate &other) {
            if (this != &other) {
                delete[] code;
                code_count = other.code_count;
                code = new uint8[code_count];
                std::memcpy(code, other.code, code_count * sizeof(uint8));
                stack_max = other.stack_max;
                args = other.args;
                locals = other.locals;
                exceptions = other.exceptions;
                lines = other.lines;
                matches = other.matches;
                method = other.method;
            }
            return *this;
        }

        // Move assignment
        FrameTemplate &operator=(FrameTemplate &&other) noexcept {
            if (this != &other) {
                delete[] code;
                code_count = other.code_count;
                code = other.code;
                stack_max = other.stack_max;
                args = std::move(other.args);
                locals = std::move(other.locals);
                exceptions = std::move(other.exceptions);
                lines = std::move(other.lines);
                matches = std::move(other.matches);
                method = other.method;

                other.code = null;
                other.code_count = 0;
                other.method = null;
            }
            return *this;
        }

        ~FrameTemplate() {
            delete[] code;
        }

        Frame initialize();
        FrameTemplate *copy();

        uint32 get_code_count() const {
            return code_count;
        }

        uint8 *get_code() const {
            return code;
        }

        uint32 get_stack_max() const {
            return stack_max;
        }

        const ArgsTable &get_args() const {
            return args;
        }

        const LocalsTable &get_locals() const {
            return locals;
        }

        const ExceptionTable &get_exceptions() const {
            return exceptions;
        }

        const LineNumberTable &get_lines() const {
            return lines;
        }

        const vector<MatchTable> &get_matches() const {
            return matches;
        }

        ArgsTable &get_args() {
            return args;
        }

        LocalsTable &get_locals() {
            return locals;
        }

        ExceptionTable &get_exceptions() {
            return exceptions;
        }

        LineNumberTable &get_lines() {
            return lines;
        }

        vector<MatchTable> &get_matches() {
            return matches;
        }

        ObjMethod *get_method() const {
            return method;
        }

        void set_method(ObjMethod *method_) {
            method = method_;
        }
    };
}    // namespace spade
