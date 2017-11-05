#include "stdafx.h"
#include "CppUnitTest.h"

#include "RAM.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuTests
{		
	TEST_CLASS(RAMTest)
	{
	public:
		
		TEST_METHOD(ReadAndWrite)
		{
			RAM ram;
			ram.WriteMem(0x0, 33);
			Assert::AreEqual((int)ram.ReadMem(0x0), 33);
		}

	};
}