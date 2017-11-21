#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(CLISEITest)
    {
    public:

        TEST_METHOD(CLISEI)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[ROM::kMaxROMSize];
            rom[CPU::kResetVectorAddressL - ROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - ROM::kStartAddress] = ROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            rom[codeIndex++] = 0x78; // SEI
            rom[codeIndex++] = 0x58; // CLI
            rom[codeIndex++] = 0x78; // SEI

            emu.Load(rom, ROM::kMaxROMSize);

            // Force a carry to happen
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::InterruptDisable));
            Assert::AreEqual(2, cycles);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::InterruptDisable));
            Assert::AreEqual(2, cycles);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::InterruptDisable));
            Assert::AreEqual(2, cycles);
        }

    };
}