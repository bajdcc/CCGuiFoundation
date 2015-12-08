#include "stdafx.h"
#include "CppUnitTest.h"
#include <vector>
#include <list>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(TestBase)
	{
	public:
		
		TEST_METHOD(Iter)
		{
			Logger::WriteMessage("Iter test");
			using namespace std;

			vector<int> g = { 1, 2, 3, 4, 5, 6 };
			for (auto i = 5; i >= 0;i--)
			{
				g.erase(g.begin()+i);
				Logger::WriteMessage("*");
			}
		}
	};
}