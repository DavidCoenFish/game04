#include "UnitTestPCH.h"
#include "Common/PathFinder/PathFinderPathTile.h"

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

#if 0
#define LOGGER(message) Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(message)
#else
#define LOGGER(message) (void)0
#endif


namespace CommonPathFinderPathTile
{
	TEST_CLASS(Basic)
	{

	public:

		TEST_METHOD(Region)
		{
			std::string traversableData( 
				"10101"
				"11111"
				"01010"
				"10010"
				"1011"
				);
			std::reverse(traversableData.begin(), traversableData.end());

			PathFinder::PathTile<4,4,char> pathTile(
				PathFinder::PackLocation(0,0),
				std::bitset<24>(traversableData)
				);

			//zero
			{
				const int regionId = pathTile.GetRegion(PathFinder::PackLocation(1,0));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, 0);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(3,0)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(0,2)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(2,2)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(4,2)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(1,3)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(2,3)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(4,3)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(1,4)));
			}

			{
				const int regionId = pathTile.GetRegion(PathFinder::PackLocation(0,0));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(0,0)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(2,0)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(4,0)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(0,1)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(1,1)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(2,1)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(3,1)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(4,1)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(1,2)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(3,2)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(3,3)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(2,4)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(3,4)) );
			}
			{
				const int regionId = pathTile.GetRegion(PathFinder::PackLocation(0,3));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(0,3)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(0,4)) );
			}
			{
				const int regionIdA = pathTile.GetRegion(PathFinder::PackLocation(1,0));
				const int regionIdB = pathTile.GetRegion(PathFinder::PackLocation(0,0));
				const int regionIdC = pathTile.GetRegion(PathFinder::PackLocation(0,3));

				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(regionIdA, regionIdB );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(regionIdA, regionIdC );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(regionIdB, regionIdC );
			}
		}

		TEST_METHOD(AppendPath)
		{
			std::string traversableData( 
				"10101"
				"11111"
				"01010"
				"10010"
				"1011"
				);
			std::reverse(traversableData.begin(), traversableData.end());

			PathFinder::PathTile<4,4,char> pathTile(
				PathFinder::PackLocation(0,0),
				std::bitset<24>(traversableData)
				);
			std::vector< PathFinder::Location > path;
			pathTile.AppendPath(
				PathFinder::PackLocation(2,4),
				PathFinder::PackLocation(2,0),
				path
				);

			Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(6, (int)path.size());
			{
				short x = 0;
				short y = 0;
				PathFinder::UnPackLocation(path[0], x, y);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)x, 3);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)y, 4);
			}
			{
				short x = 0;
				short y = 0;
				PathFinder::UnPackLocation(path[1], x, y);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)x, 3);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)y, 3);
			}
			{
				short x = 0;
				short y = 0;
				PathFinder::UnPackLocation(path[2], x, y);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)x, 3);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)y, 2);
			}
			{
				short x = 0;
				short y = 0;
				PathFinder::UnPackLocation(path[3], x, y);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)x, 3);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)y, 1);
			}
			{
				short x = 0;
				short y = 0;
				PathFinder::UnPackLocation(path[4], x, y);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)x, 2);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)y, 1);
			}
			{
				short x = 0;
				short y = 0;
				PathFinder::UnPackLocation(path[5], x, y);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)x, 2);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)y, 0);
			}

		}


		TEST_METHOD(RegionOffset)
		{
			std::string traversableData( 
				"10101"
				"11111"
				"01010"
				"10010"
				"1011"
				);
			std::reverse(traversableData.begin(), traversableData.end());

			const int offsetX = 256;
			const int offsetY = 128;

			PathFinder::PathTile<4,4,char> pathTile(
				PathFinder::PackLocation(offsetX,offsetY),
				std::bitset<24>(traversableData)
				);

			//zero
			{
				const int regionId = pathTile.GetRegion(PathFinder::PackLocation(offsetX + 1, offsetY + 0));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, 0);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(offsetX + 3, offsetY + 0)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(offsetX + 0, offsetY + 2)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(offsetX + 2, offsetY + 2)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(offsetX + 4, offsetY + 2)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(offsetX + 1, offsetY + 3)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(offsetX + 2, offsetY + 3)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(offsetX + 4, offsetY + 3)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathFinder::PackLocation(offsetX + 1, offsetY + 4)));
			}
			{
				const int regionIdA = pathTile.GetRegion(PathFinder::PackLocation(offsetX + 1, offsetY + 0));
				const int regionIdB = pathTile.GetRegion(PathFinder::PackLocation(offsetX + 0, offsetY + 0));
				const int regionIdC = pathTile.GetRegion(PathFinder::PackLocation(offsetX + 0, offsetY + 3));

				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(2, pathTile.GetRegionCount());
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(regionIdA, regionIdB );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(regionIdA, regionIdC );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(regionIdB, regionIdC );
			}
		}

	};
}
