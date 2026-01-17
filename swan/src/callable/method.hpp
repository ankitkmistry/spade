#pragma once

#include "callable.hpp"
#include "callable/table.hpp"
#include "frame.hpp"
#include <cstdint>

namespace spade
{
    class SWAN_EXPORT ObjMethod final : public ObjCallable {
      private:
        uint32_t code_count;
        std::unique_ptr<uint8_t[]> code;
        uint32_t stack_max;
        VariableTable arg_tbl;
        VariableTable loc_tbl;
        ExceptionTable exceptions;
        LineNumberTable lines;
        vector<MatchTable> matches;

      public:
        ObjMethod(Kind kind, const Sign &sign, const vector<uint8_t> &code, uint32_t stack_max, const VariableTable &args,
                  const VariableTable &locals, const ExceptionTable &exceptions, const LineNumberTable &lines, const vector<MatchTable> &matches);

        void call(Obj *self, vector<Value> args) override;
        void call(Obj *self, Value *args) override;

        uint32_t get_code_count() const {
            return code_count;
        }

        uint8_t *get_code() const {
            return &code[0];
        }

        uint32_t get_stack_max() const {
            return stack_max;
        }

        const VariableTable &get_args() const {
            return arg_tbl;
        }

        const VariableTable &get_locals() const {
            return loc_tbl;
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

        VariableTable &get_args() {
            return arg_tbl;
        }

        VariableTable &get_locals() {
            return loc_tbl;
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

        Obj *copy() const override {
            return (Obj *) this;
        }

        string to_string() const override;

      private:
        void call_impl(Obj *self, Value *args);
    };
}    // namespace spade
