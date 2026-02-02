#pragma once

#include "callable.hpp"
#include "callable/table.hpp"
#include "ee/obj.hpp"
#include "frame.hpp"
#include <cstdint>

namespace spade
{
    class SWAN_EXPORT ObjMethod final : public ObjCallable {
        struct CaptureInfo {
            uint16_t local_index;
            ObjCapture *capture;
        };

      private:
        uint32_t code_count;
        std::unique_ptr<uint8_t[]> code;
        uint32_t stack_max;
        uint8_t args_count;
        uint16_t locals_count;
        vector<CaptureInfo> captures;
        ExceptionTable exceptions;
        LineNumberTable lines;
        vector<MatchTable> matches;

      public:
        ObjMethod(Kind kind, const Sign &sign, const vector<uint8_t> &code, uint32_t stack_max, uint8_t args_count, uint16_t locals_count,
                  const ExceptionTable &exceptions, const LineNumberTable &lines, const vector<MatchTable> &matches);

        void call(Obj *self, vector<Value> args) override;
        void call(Obj *self, Value *args) override;

        void set_capture(uint16_t local_idx, ObjCapture *capture);
        ObjMethod *force_copy() const;

        uint32_t get_code_count() const {
            return code_count;
        }

        uint8_t *get_code() const {
            return &code[0];
        }

        uint32_t get_stack_max() const {
            return stack_max;
        }

        size_t get_args_count() const override {
            return args_count;
        }

        size_t get_locals_count() const {
            return locals_count;
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
