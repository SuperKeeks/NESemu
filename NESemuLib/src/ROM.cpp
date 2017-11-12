#include "ROM.h"

#include "Assert.h"

ROM::ROM()
{
}

ROM::~ROM()
{
}

uint8_t ROM::ReadMem(uint16_t address)
{
	OMBAssert(false, "Unimplemented");
	return 0;
}

void ROM::WriteMem(uint16_t address, uint8_t value)
{
	OMBAssert(false, "Unimplemented");
}

void ROM::PowerOn()
{
	OMBAssert(false, "Unimplemented");
}

void ROM::Reset()
{
	OMBAssert(false, "Unimplemented");
}
