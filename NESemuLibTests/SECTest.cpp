#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(SECTest)
    {
    public:

        TEST_METHOD(SEC)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[ROM::kMaxROMSize];
            rom[CPU::kResetVectorAddressL - ROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - ROM::kStartAddress] = ROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            // Set carry
            rom[codeIndex++] = 0x18; // CLC
            rom[codeIndex++] = 0x38; // SEC

            emu.Load(rom, ROM::kMaxROMSize);

            cpu.ExecuteNextInstruction();
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::Carry));
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(true, cpu.GetFlag(CPU::Flag::Carry));
            Assert::AreEqual(2, cycles);
        }

    };
}