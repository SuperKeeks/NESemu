#include "SRAM.h"

#include "Assert.h"

SRAM::SRAM()
{
}

SRAM::~SRAM()
{
}

uint8_t SRAM::ReadMem(uint16_t address)
{
	OMBAssert(false, "Unimplemented");
	return 0;
}

void SRAM::WriteMem(uint16_t address, uint8_t value)
{
	OMBAssert(false, "Unimplemented");
}

void SRAM::Reset()
{
	OMBAssert(false, "Unimplemented");
}
