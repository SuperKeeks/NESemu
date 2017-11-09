#include "Input.h"

#include "Assert.h"

Input::Input()
{
}

Input::~Input()
{
}

uint8_t Input::ReadMem(uint16_t address)
{
	OMBAssert(false, "Unimplemented");
	return 0;
}

void Input::WriteMem(uint16_t address, uint8_t value)
{
	OMBAssert(false, "Unimplemented");
}

void Input::Reset()
{
	OMBAssert(false, "Unimplemented");
}
