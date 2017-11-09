#pragma once

#include "MemoryHandler.h"

class ROM : public MemoryHandler
{
public:
	ROM();
	virtual ~ROM();

	virtual uint8_t ReadMem(uint16_t address);
	virtual void WriteMem(uint16_t address, uint8_t value);
	virtual void Reset();
};
