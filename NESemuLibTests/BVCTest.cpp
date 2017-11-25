#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(BVCTest)
    {
    public:

        TEST_METHOD(BVC)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[ROM::kMaxROMSize];
            rom[CPU::kResetVectorAddressL - ROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - ROM::kStartAddress] = ROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            rom[codeIndex++] = 0x50; // BVC
            rom[codeIndex++] = 0x22; // Relative displacement (positive)
            rom[codeIndex++] = 0x50; // BVC
            rom[codeIndex++] = 0x80; // Relative displacement (negative)

            emu.Load(rom, ROM::kMaxROMSize);

            // Branch not taken
            cpu.SetFlag(CPU::Flag::Overflow, true);
            const int initialProgramCounter = cpu.GetProgramCounter();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(initialProgramCounter + 2, (int)cpu.GetProgramCounter());
            Assert::AreEqual(2, cycles);

            // Branch taken, positive, boundary not crossed
            cpu.SetProgramCounter(initialProgramCounter);
            cpu.SetFlag(CPU::Flag::Overflow, false);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(initialProgramCounter + 2 + 0x22, (int)cpu.GetProgramCounter());
            Assert::AreEqual(3, cycles);

            // Branch taken, negative, boundary crossed
            cpu.SetProgramCounter(initialProgramCounter + 2);
            cpu.SetFlag(CPU::Flag::Overflow, false);
            cycles = cpu.ExecuteNextInstruction();
            uint8_t relativeDisplacementInverted = ~0x80;
            Assert::AreEqual(initialProgramCounter + 4 - relativeDisplacementInverted - 1, (int)cpu.GetProgramCounter());
            Assert::AreEqual(4, cycles);
        }

    };
}