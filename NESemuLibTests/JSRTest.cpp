#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(JSRTest)
    {
    public:

        TEST_METHOD(JSR)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[ROM::kMaxROMSize];
            rom[CPU::kResetVectorAddressL - ROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - ROM::kStartAddress] = ROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            rom[codeIndex++] = 0x20; // JSR Absolute
            rom[codeIndex++] = 0x34; // Absolute value low-byte
            rom[codeIndex++] = 0x12; // Absolute value high-byte

            emu.Load(rom, ROM::kMaxROMSize);

            uint16_t initialProgramCounter = cpu.GetProgramCounter();
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(0x1234, (int)cpu.GetProgramCounter());
            Assert::AreEqual(6, cycles);
            const uint16_t pushedAddress = cpu.PeekStack(0) + (cpu.PeekStack(1) << 8);
            Assert::AreEqual(initialProgramCounter + 2, (int)pushedAddress);
        }

    };
}