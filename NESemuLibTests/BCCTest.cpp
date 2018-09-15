#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(BCCTest)
    {
    public:

        TEST_METHOD(BCC)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[MemoryMapper::kUnitTestPGRROMSize];
            rom[CPU::kResetVectorAddressL - MemoryMapper::kPGRROMStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - MemoryMapper::kPGRROMStartAddress] = MemoryMapper::kPGRROMStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            rom[codeIndex++] = 0x90; // BCC
            rom[codeIndex++] = 0x22; // Relative displacement (positive)
            rom[codeIndex++] = 0x90; // BCC
            rom[codeIndex++] = 0x80; // Relative displacement (negative)

            emu.Load(rom, MemoryMapper::kUnitTestPGRROMSize);

            // Branch not taken
            cpu.SetFlag(CPU::Flag::Carry, true);
            const int initialProgramCounter = cpu.GetProgramCounter();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(initialProgramCounter + 2, (int)cpu.GetProgramCounter());
            Assert::AreEqual(2, cycles);

            // Branch taken, positive, boundary not crossed
            cpu.SetProgramCounter(initialProgramCounter);
            cpu.SetFlag(CPU::Flag::Carry, false);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(initialProgramCounter + 2 + 0x22, (int)cpu.GetProgramCounter());
            Assert::AreEqual(3, cycles);

            // Branch taken, negative, boundary crossed
            cpu.SetProgramCounter(initialProgramCounter + 2);
            cpu.SetFlag(CPU::Flag::Carry, false);
            cycles = cpu.ExecuteNextInstruction();
            uint8_t relativeDisplacementInverted = ~0x80;
            Assert::AreEqual(initialProgramCounter + 4 - relativeDisplacementInverted - 1, (int)cpu.GetProgramCounter());
            Assert::AreEqual(4, cycles);
        }

    };
}