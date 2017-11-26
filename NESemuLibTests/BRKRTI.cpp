#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(BRKRTITest)
    {
    public:

        TEST_METHOD(BRKRTI)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[ROM::kMaxROMSize];
            rom[CPU::kResetVectorAddressL - ROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - ROM::kStartAddress] = ROM::kStartAddress >> 8;
            rom[CPU::kInterruptBreakVectorAddressL - ROM::kStartAddress] = 0x50;
            rom[CPU::kInterruptBreakVectorAddressH - ROM::kStartAddress] = ROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            rom[codeIndex++] = 0x00; // BRK
            rom[0x50] = 0x40;        // RTI

            emu.Load(rom, ROM::kMaxROMSize);

            // BRK
            const uint16_t pcBeforeBreak = cpu.GetProgramCounter();
            const uint8_t statusBeforeBreak = cpu.GetStatus();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(7, cycles);
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::InterruptDisable));
            const uint16_t pcAfterBreak = cpu.GetProgramCounter();
            const uint8_t flagsInStack = cpu.PeekStack(0);
            Assert::AreEqual((uint8_t)(statusBeforeBreak | (1 << CPU::Flag::Break) | (1 << CPU::Flag::Unused)), flagsInStack);

            // RTI
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(6, cycles);
            Assert::AreEqual(statusBeforeBreak, cpu.GetStatus());
            Assert::AreEqual((int)pcBeforeBreak + 2, (int)cpu.GetProgramCounter());
        }

    };
}