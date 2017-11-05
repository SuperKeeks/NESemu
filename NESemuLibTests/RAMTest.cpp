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
			ram.WriteMem(0x0, 33);
		}

	};
}