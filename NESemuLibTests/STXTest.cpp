#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(STXTest)
    {
    public:

        TEST_METHOD(STX)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[MemoryMapper::kUnitTestPGRROMSize];
            rom[CPU::kResetVectorAddressL - MemoryMapper::kPGRROMStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - MemoryMapper::kPGRROMStartAddress] = MemoryMapper::kPGRROMStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            // Zero Page
            rom[codeIndex++] = 0xA2; // LDX (Immediate)
            rom[codeIndex++] = 0x33; // #$33
            rom[codeIndex++] = 0x86; // STX (Zero Page)
            rom[codeIndex++] = 0x44; // $44

            // Zero Page, Y
            rom[codeIndex++] = 0xA2; // LDX (Immediate)
            rom[codeIndex++] = 0x66; // #$66
            rom[codeIndex++] = 0xA0; // LDY (Immediate)
            rom[codeIndex++] = 0x04; // #$4
            rom[codeIndex++] = 0x96; // STX (Zero Page, Y)
            rom[codeIndex++] = 0x30; // $30

            // Absolute
            rom[codeIndex++] = 0xA2; // LDX (Immediate)
            rom[codeIndex++] = 0x11; // #$11
            rom[codeIndex++] = 0x8E; // STX (Absolute)
            rom[codeIndex++] = 0x34; // Low part of address
            rom[codeIndex++] = 0x12; // High part of address

            emu.Load(rom, MemoryMapper::kUnitTestPGRROMSize);

            ////////////
            // Zero Page
            ////////////
            cpu.ExecuteNextInstruction(); // LDA
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x33, emu.ReadMem(0x44));
            Assert::AreEqual(3, cycles);

            ///////////////
            // Zero Page, Y
            ///////////////
            cpu.ExecuteNextInstruction(); // LDX
            cpu.ExecuteNextInstruction(); // LDY
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x66, emu.ReadMem(0x34));
            Assert::AreEqual(4, cycles);

            ///////////
            // Absolute
            ///////////
            cpu.ExecuteNextInstruction(); // LDA
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x11, emu.ReadMem(0x1234));
            Assert::AreEqual(4, cycles);
        }

    };
}