#include "CppUnitTest.h"

#include "RAM.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{		
	TEST_CLASS(RAMTest)
	{
	public:
		
		TEST_METHOD(WriteMem)
		{
			RAM ram;
            ram.Reset();

			ram.WriteMem(0x0, 64);
            Assert::AreEqual((int)ram.ReadMem(0x0), 64);
            Assert::AreEqual((int)ram.ReadMem(0x800), 64);
            Assert::AreEqual((int)ram.ReadMem(0x1000), 64);
            Assert::AreEqual((int)ram.ReadMem(0x1800), 64);
            Assert::AreNotEqual((int)ram.ReadMem(0x0), 128);
            Assert::AreNotEqual((int)ram.ReadMem(0x800), 128);
            Assert::AreNotEqual((int)ram.ReadMem(0x1000), 128);
            Assert::AreNotEqual((int)ram.ReadMem(0x1800), 128);

            ram.WriteMem(0x1800, 128);
            Assert::AreEqual((int)ram.ReadMem(0x0), 128);
            Assert::AreEqual((int)ram.ReadMem(0x800), 128);
            Assert::AreEqual((int)ram.ReadMem(0x1000), 128);
            Assert::AreEqual((int)ram.ReadMem(0x1800), 128);
            Assert::AreNotEqual((int)ram.ReadMem(0x0), 64);
            Assert::AreNotEqual((int)ram.ReadMem(0x800), 64);
            Assert::AreNotEqual((int)ram.ReadMem(0x1000), 64);
            Assert::AreNotEqual((int)ram.ReadMem(0x1800), 64);
		}

	};
}