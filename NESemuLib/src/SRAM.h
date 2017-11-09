#pragma once

#include "MemoryHandler.h"

class SRAM : public MemoryHandler
{
public:
	SRAM();
	virtual ~SRAM();

	virtual uint8_t ReadMem(uint16_t address);
	virtual void WriteMem(uint16_t address, uint8_t value);
	virtual void Reset();
};
