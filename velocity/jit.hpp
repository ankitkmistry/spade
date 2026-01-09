#pragma once

#include "callable/method.hpp"

#define DISABLE_JIT

#ifndef DISABLE_JIT

namespace spade
{
    void jit_test(ObjMethod *method);
}

#endif