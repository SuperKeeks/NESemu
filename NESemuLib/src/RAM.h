#pragma once

#include "MemoryHandler.h"

class RAM : public MemoryHandler
{
public:
	RAM();
	virtual ~RAM();

	virtual uint8_t ReadMem(uint16_t address) const;
	virtual void WriteMem(uint16_t address, uint8_t value);

private:
	uint8_t _ram[2048];
	uint16_t ConvertToRealAddress(uint16_t address) const;
};
