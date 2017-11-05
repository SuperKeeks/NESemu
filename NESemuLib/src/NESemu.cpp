#include "NESemu.h"

NESemu::NESemu()
{
}

NESemu::~NESemu()
{
}

uint8_t NESemu::ReadMem(uint16_t address) const
{
	return 0;
}

void NESemu::WriteMem(uint16_t address, uint8_t value)
{
}

void NESemu::Reset()
{
    _ram.Reset();
}