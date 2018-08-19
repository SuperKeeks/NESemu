#pragma once

#include <stdint.h>

namespace BitwiseUtils
{
    void SetFlag(uint8_t& registerRef, int bitIndex, bool value);
    bool IsFlagSet(uint8_t registerValue, int bitIndex);

    // Returns the value within the specified range, shifted to the right
    // Eg: If we want the 4th and 3rd bits of a (8 bit) value, we'll call the function like this:
    //      GetBitRange(value, 3, 2);
    uint8_t GetBitRange(uint8_t value, int startBitIndex, int length);
}
