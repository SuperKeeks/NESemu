#include "CPU.h"

#include "LogUtils.h"

int CPU::LDA(AddressingMode mode)
{
	Log::Debug("LDA %d", mode);
	return mode;
}