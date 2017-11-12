#include "PPU.h"

#include "Assert.h"

PPU::PPU()
{
}

PPU::~PPU()
{
}

uint8_t PPU::ReadMem(uint16_t address)
{
	OMBAssert(false, "Unimplemented");
	return 0;
}

void PPU::WriteMem(uint16_t address, uint8_t value)
{
	OMBAssert(false, "Unimplemented");
}

void PPU::PowerOn()
{
	OMBAssert(false, "Unimplemented");
}

void PPU::Reset()
{
	OMBAssert(false, "Unimplemented");
}
