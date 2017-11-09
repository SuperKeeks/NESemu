#include "NESemu.h"

NESemu::NESemu()
{
}

NESemu::~NESemu()
{
}

uint8_t NESemu::ReadMem(uint16_t address)
{
	MemoryHandler& handler = GetMemoryHandlerForAddress(address);
	return handler.ReadMem(address);
}

void NESemu::WriteMem(uint16_t address, uint8_t value)
{
	MemoryHandler& handler = GetMemoryHandlerForAddress(address);
	handler.WriteMem(address, value);
}

void NESemu::Reset()
{
    _ram.Reset();
	_ppu.Reset();
}

MemoryHandler& NESemu::GetMemoryHandlerForAddress(uint16_t address)
{
	if (address >= 0x0 && address < 0x2000)
	{
		return _ram;
	}
	else if (address >= 0x2000 && address < 0x4000)
	{
		return _ppu;
	}
	else
	{
		OMBAssert(false, "Unimplemented!");
		return *(new MemoryHandler());
	}
}
