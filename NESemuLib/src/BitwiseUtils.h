#pragma once

#include <stdint.h>

namespace BitwiseUtils
{
    void SetFlag(uint8_t& registerRef, int shift, bool value);
    bool IsFlagSet(uint8_t registerValue, int shift);
}
