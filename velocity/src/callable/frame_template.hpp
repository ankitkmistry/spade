#pragma once

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
        uint32 max_stack;
        ArgsTable args;
        LocalsTable locals;
        ExceptionTable exceptions;
        LineNumberTable lines;
        vector<ObjMethod *> lambdas;
        vector<MatchTable> matches;
        ObjMethod *method;

      public:
        FrameTemplate(uint32 code_count, uint8 *code, uint32 max_stack, ArgsTable args, LocalsTable locals, ExceptionTable exceptions,
                      LineNumberTable lines, vector<ObjMethod *> lambdas, vector<MatchTable> matches, ObjMethod *method = null)
            : code_count(code_count),
              code(code),
              max_stack(max_stack),
              args(args),
              locals(locals),
              exceptions(exceptions),
              lines(lines),
              lambdas(lambdas),
              matches(matches),
              method(method) {}

        Frame initialize();
        FrameTemplate *copy();

        uint32 get_code_count() const {
            return code_count;
        }

        uint8 *get_code() const {
            return code;
        }

        uint32 get_max_stack() const {
            return max_stack;
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

        const vector<ObjMethod *> &get_lambdas() const {
            return lambdas;
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

        vector<ObjMethod *> &get_lambdas() {
            return lambdas;
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
