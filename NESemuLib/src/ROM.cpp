#include "ROM.h"

#include "Assert.h"
#include "SizeOfArray.h"

ROM::ROM()
{
}

ROM::~ROM()
{
}

uint8_t ROM::ReadMem(uint16_t address)
{
    uint16_t index = ConvertToIndex(address);
    return _rom[index];
}

void ROM::WriteMem(uint16_t address, uint8_t value)
{
    // Do nothing
    OMBAssert(false, "Trying to write ROM!!!");
}

void ROM::PowerOn()
{
    for (int i = 0; i < sizeofarray(_rom); ++i)
    {
        _rom[i] = 0;
    }
}

void ROM::Reset()
{
    // Do nothing
}

uint16_t ROM::ConvertToIndex(uint16_t address) const
{
    OMBAssert(address >= kStartAddress && address < kStartAddress + sizeofarray(_rom), "Address is not within the ROM's address space");
    if (_is16KBROM)
    {
        return (address - kStartAddress) % 16384;
    }
    else
    {
        return address - kStartAddress;
    }
}