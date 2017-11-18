#pragma once

#include <functional>
#include <map>
#include <stdint.h>

class MemoryHandler;

class CPU
{
public:
	static const int kStackSize = 256;
	static const int kResetVectorAddressL = 0xFFFC;
	static const int kResetVectorAddressH = kResetVectorAddressL + 1;

	enum AddressingMode
	{
		Immediate,
		ZeroPage,
		ZeroPageX,
		Absolute,
		AbsoluteX,
		AbsoluteY,
		IndirectX,
		IndirectY
	};

	void PowerOn();
	void Reset(MemoryHandler* memoryHandler);

	int ExecuteNextInstruction(); // Returns number of CPU cycles taken

private:
	uint8_t _stack[kStackSize];
	uint16_t _programCounter;
	uint8_t _stackPointer;
	uint8_t _status; // Flags: 1-Carry(C), 2-Zero(Z), 3-Interrupt disable(I), 4-Decimal mode(D), 5-Break(b), 6-Unused(1), 7-Overflow(V), 8-Negative(N)
	uint8_t _accumulator;
	uint8_t _x;
	uint8_t _y;
	std::map<uint8_t, std::function<int()>> _opcodes = {

		// LDA
		{0xA9, [this]() -> int { return LDA(AddressingMode::Immediate); } }
	};

	MemoryHandler* _memoryHandler = nullptr;

	uint8_t GetValueWithMode(AddressingMode mode, int& cycles);

	int LDA(AddressingMode mode);
};
