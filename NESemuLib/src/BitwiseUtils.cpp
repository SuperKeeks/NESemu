#include "BitwiseUtils.h"

void BitwiseUtils::SetFlag(uint8_t& registerRef, int shift, bool value)
{
    if (value)
    {
        registerRef |= (1 << shift);
    }
    else
    {
        registerRef &= ~(1 << shift);
    }
}

bool BitwiseUtils::IsFlagSet(uint8_t registerValue, int shift)
{
    return (registerValue & (1 << shift)) != 0;
}
