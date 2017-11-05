#include "RAM.h"

#include <assert.h>

RAM::RAM()
{
}

RAM::~RAM()
{
}

uint8_t RAM::ReadMem(uint16_t address) const
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
	assert(address >= 0 && address < 0x2000);
	return address % 2048; // Meant for Shadow RAM
}