#include "UnitTestPCH.h"

namespace LanguageLambdaSmartpointer
{
	std::atomic_int s_flag;

	class Scope
	{
	public:
		Scope()
		{
			s_flag = 1;
		}
		~Scope()
		{
			s_flag = 2;
		}
	};

	static void SimpleThread(std::shared_ptr<Scope> pScope)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(1, s_flag.load());
		pScope = nullptr;
		Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(2, s_flag.load());
	}

	TEST_CLASS(LambdaTakingSmartpointerOwnership)
	{
	public: 
		TEST_METHOD(Join)
		{
			s_flag = 0;
			std::thread thread;
			{
				auto pScope = std::make_shared< Scope >();
				thread = std::thread(SimpleThread, pScope);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(1, s_flag.load());
			}
			thread.join();
			Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(2, s_flag.load());
		}
	};


}
