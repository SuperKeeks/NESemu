#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(CLDTest)
    {
    public:

        TEST_METHOD(CLD)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[MemoryMapper::kUnitTestPGRROMSize];
            rom[CPU::kResetVectorAddressL - MemoryMapper::kPGRROMStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - MemoryMapper::kPGRROMStartAddress] = MemoryMapper::kPGRROMStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            rom[codeIndex++] = 0x78; // CLD

            emu.Load(rom, MemoryMapper::kUnitTestPGRROMSize);

            // Force a carry to happen
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual(false, cpu.GetFlag(CPU::Flag::DecimalMode));
            Assert::AreEqual(2, cycles);
        }

    };
}