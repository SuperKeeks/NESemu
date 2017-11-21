#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(CLCTest)
    {
    public:

        TEST_METHOD(CLC)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[ROM::kMaxROMSize];
            rom[CPU::kResetVectorAddressL - ROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - ROM::kStartAddress] = ROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            // Force a carry to happen
            rom[codeIndex++] = 0x18; // CLC
            rom[codeIndex++] = 0xA9; // LDA (Immediate)
            rom[codeIndex++] = 0xFF; // Accumulator value
            rom[codeIndex++] = 0x69; // ADC (Immediate)
            rom[codeIndex++] = 0xFF; // Value to add

            // Clear carry
            rom[codeIndex++] = 0x18; // CLC

            emu.Load(rom, ROM::kMaxROMSize);

            // Force a carry to happen
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Carry));

            // Clear carry
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);
        }

    };
}