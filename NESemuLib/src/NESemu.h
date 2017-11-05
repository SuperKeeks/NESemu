#pragma once

#include <stdint.h>
#include "RAM.h"

class NESemu : public MemoryHandler
{
public:
	NESemu();
	~NESemu();

private:
    RAM _ram;

    virtual uint8_t ReadMem(uint16_t address) const;
    virtual void WriteMem(uint16_t address, uint8_t value);
    virtual void Reset();
};
