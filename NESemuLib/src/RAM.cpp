#include "RAM.h"

#include "Assert.h"

RAM::RAM()
{
}

RAM::~RAM()
{
}

uint8_t RAM::ReadMem(uint16_t address)
{
	uint16_t realAddress = ConvertToRealAddress(address);
	return _ram[realAddress];
}

void RAM::WriteMem(uint16_t address, uint8_t value)
{
	uint16_t realAddress = ConvertToRealAddress(address);
	_ram[realAddress] = value;
}

uint16_t RAM::ConvertToRealAddress(uint16_t address) const
{
	OMBAssert(address >= 0 && address < 0x2000, "Address is not within the RAM's address space");
	return address % 2048; // Meant for Shadow RAM
}

void RAM::Reset()
{
    for (int i = 0; i < kRAMSize; ++i)
    {
        _ram[i] = 0;
    }
}
