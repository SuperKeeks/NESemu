#include "BitwiseUtils.h"

#include "Assert.h"

void BitwiseUtils::SetFlag(uint8_t& registerRef, int bitIndex, bool value)
{
    if (value)
    {
        registerRef |= (1 << bitIndex);
    }
    else
    {
        registerRef &= ~(1 << bitIndex);
    }
}

bool BitwiseUtils::IsFlagSet(uint8_t registerValue, int bitIndex)
{
    return (registerValue & (1 << bitIndex)) != 0;
}

uint8_t BitwiseUtils::GetBitRange(uint8_t value, int startBitIndex, int length)
{
    OMBAssert(length >= 1 && length <= 8, "The length has to be between 1 and 8");

    // Take the required bits using a mask
    uint8_t mask = 0;
    for (int i = startBitIndex; i > startBitIndex - length; --i)
    {
        mask |= (1 << i);
    }
    uint8_t maskedValue = value & mask;

    // Shift the result so the required bits are at the right
    return maskedValue >> (startBitIndex - length + 1);
}
