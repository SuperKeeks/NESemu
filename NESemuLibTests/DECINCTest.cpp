#include "CppUnitTest.h"

#include "NESemu.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(DECINCTest)
    {
    public:

        TEST_METHOD(DECINC)
        {
            NESemu emu;
            CPU& cpu = *emu.GetCPU();
            uint8_t rom[ROM::kMaxROMSize];
            rom[CPU::kResetVectorAddressL - ROM::kStartAddress] = 0;
            rom[CPU::kResetVectorAddressH - ROM::kStartAddress] = ROM::kStartAddress >> 8;
            int cycles;
            int codeIndex = 0;

            rom[codeIndex++] = 0xA9; // LDA Immediate
            rom[codeIndex++] = 0x00; // Value to load
            rom[codeIndex++] = 0x85; // STA Zero Page
            rom[codeIndex++] = 0x33; // Zero Page to store in
            rom[codeIndex++] = 0xE6; // INC Zero Page
            rom[codeIndex++] = 0x33; // Zero Page address to increment
            rom[codeIndex++] = 0xC6; // DEC Zero Page
            rom[codeIndex++] = 0x33; // Zero Page address to decrement
            rom[codeIndex++] = 0xC6; // DEC Zero Page
            rom[codeIndex++] = 0x33; // Zero Page address to decrement
            rom[codeIndex++] = 0xE6; // INC Zero Page
            rom[codeIndex++] = 0x33; // Zero Page address to increment

            emu.Load(rom, ROM::kMaxROMSize);

            cpu.ExecuteNextInstruction();
            cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x00, emu.ReadMem(0x33));
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x01, emu.ReadMem(0x33));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(5, cycles);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x00, emu.ReadMem(0x33));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(5, cycles);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0xFF, emu.ReadMem(0x33));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(5, cycles);
            cycles = cpu.ExecuteNextInstruction();
            Assert::AreEqual((uint8_t)0x00, emu.ReadMem(0x33));
            Assert::AreEqual(false, cpu.GetFlag(CPU::Sign));
            Assert::AreEqual(true, cpu.GetFlag(CPU::Zero));
            Assert::AreEqual(5, cycles);
        }

    };
}