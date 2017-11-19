#pragma once

#include <functional>
#include <map>
#include <stdint.h>

class MemoryHandler;

class CPU
{
public:
	static const int kStackSize = 256;
	static const uint16_t kStackAddressStart = 0x100;
	static const uint16_t kResetVectorAddressL = 0xFFFC;
	static const uint16_t kResetVectorAddressH = kResetVectorAddressL + 1;

	enum Flag
	{
		Carry = 0,          // C
		Zero,               // Z
		InterruptDisable,   // I
		DecimalMode,        // D
		Break,              // B
		Unused,             // 1
		Overflow,           // V
		Sign                // S (1 for Negative)
	};

	enum AddressingMode
	{
		Immediate,
		ZeroPage,
		ZeroPageX,
		ZeroPageY,
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
	uint16_t _programCounter;
	uint8_t _stackPointer;
	uint8_t _status;
	uint8_t _accumulator;
	uint8_t _x;
	uint8_t _y;
	std::map<uint8_t, std::function<int()>> _opcodes = {
		// LDA
		{ 0xA9, [this]() -> int { return LDA(AddressingMode::Immediate); } },
		{ 0xA5, [this]() -> int { return LDA(AddressingMode::ZeroPage); } },
		{ 0xB5, [this]() -> int { return LDA(AddressingMode::ZeroPageX); } },
		{ 0xAD, [this]() -> int { return LDA(AddressingMode::Absolute); } },
		{ 0xBD, [this]() -> int { return LDA(AddressingMode::AbsoluteX); } },
		{ 0xB9, [this]() -> int { return LDA(AddressingMode::AbsoluteY); } },
		{ 0xA1, [this]() -> int { return LDA(AddressingMode::IndirectX); } },
		{ 0xB1, [this]() -> int { return LDA(AddressingMode::IndirectY); } },

		// LDX
		{ 0xA2, [this]() -> int { return LDX(AddressingMode::Immediate); } },
		{ 0xA6, [this]() -> int { return LDX(AddressingMode::ZeroPage); } },
		{ 0xB6, [this]() -> int { return LDX(AddressingMode::ZeroPageY); } },
		{ 0xAE, [this]() -> int { return LDX(AddressingMode::Absolute); } },
		{ 0xBE, [this]() -> int { return LDX(AddressingMode::AbsoluteY); } },

		// LDY
		{ 0xA0, [this]() -> int { return LDY(AddressingMode::Immediate); } },
		{ 0xA4, [this]() -> int { return LDY(AddressingMode::ZeroPage); } },
		{ 0xB4, [this]() -> int { return LDY(AddressingMode::ZeroPageX); } },
		{ 0xAC, [this]() -> int { return LDY(AddressingMode::Absolute); } },
		{ 0xBC, [this]() -> int { return LDY(AddressingMode::AbsoluteX); } },
	};

	MemoryHandler* _memoryHandler = nullptr;

	void SetFlag(Flag flag, bool value);
	bool GetFlag(Flag flag);
	uint8_t GetValueWithMode(AddressingMode mode, int& cycles);

	int LDA(AddressingMode mode);
	int LDX(AddressingMode mode);
	int LDY(AddressingMode mode);
};
