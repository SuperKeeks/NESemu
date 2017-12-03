#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(PHPPLPTest)
    {
    public:

        TEST_METHOD(PHPPLP)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[ROM::kMaxROMSize];
            rom[CPU::kResetVectorAddressL - ROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - ROM::kStartAddress] = ROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            rom[codeIndex++] = 0x08; // PHP
            rom[codeIndex++] = 0x28; // PLP

            emu.Load(rom, ROM::kMaxROMSize);

            cpu.SetAccumulator(0xFF);
            cpu.SetFlag(CPU::Flag::Carry, true);
            const uint8_t prevStatus = cpu.GetStatus();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(prevStatus, cpu.PeekStack(0));
            Assert::AreEqual(3, cycles);

            cpu.SetAccumulator(0x00);
            cpu.SetFlag(CPU::Flag::Carry, false);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(prevStatus, cpu.GetStatus());
            Assert::AreEqual(4, cycles);
        }

    };
}