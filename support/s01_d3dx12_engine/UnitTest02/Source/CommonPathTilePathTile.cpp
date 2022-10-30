#include "UnitTestPCH.h"
#include "Common/PathTile/PathTilePathTile.h"

//Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull( pResultType );
//Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(9, pResultType->Get<int>() );

#if 0
#define LOGGER(message) Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(message)
#else
#define LOGGER(message) (void)0
#endif


namespace CommonPathTilePathTile
{
	TEST_CLASS(Basic)
	{

	public:

		TEST_METHOD(Sanity)
		{
			std::string traversableData( 
				"10101"
				"11111"
				"01010"
				"10010"
				"1011"
				);
			std::reverse(traversableData.begin(), traversableData.end());

			PathTile::PathTile<4,4> pathTile(
				PathTile::PackLocation(0,0),
				std::bitset<24>(traversableData)
				);

			//zero
			{
				const int regionId = pathTile.GetRegion(PathTile::PackLocation(1,0));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, 0);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(3,0)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(0,2)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(2,2)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(4,2)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(1,3)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(2,3)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(4,3)));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(1,4)));
			}

			{
				const int regionId = pathTile.GetRegion(PathTile::PackLocation(0,0));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(0,0)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(2,0)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(4,0)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(0,1)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(1,1)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(2,1)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(3,1)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(4,1)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(1,2)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(3,2)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(3,3)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(2,4)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(3,4)) );
			}
			{
				const int regionId = pathTile.GetRegion(PathTile::PackLocation(0,3));
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(0,3)) );
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(regionId, pathTile.GetRegion(PathTile::PackLocation(0,4)) );
			}
			{
				const int regionIdA = pathTile.GetRegion(PathTile::PackLocation(1,0));
				const int regionIdB = pathTile.GetRegion(PathTile::PackLocation(0,0));
				const int regionIdC = pathTile.GetRegion(PathTile::PackLocation(0,3));

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

			PathTile::PathTile<4,4> pathTile(
				PathTile::PackLocation(0,0),
				std::bitset<24>(traversableData)
				);
			std::vector< PathTile::Location > path;
			pathTile.AppendPath(
				PathTile::PackLocation(2,4),
				PathTile::PackLocation(2,0),
				path
				);

			Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(6, (int)path.size());
			//{
			//	short x = 0;
			//	short y = 0;
			//	PathTile::UnPackLocation(path[0], x, y);
			//	Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)x, 2);
			//	Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)y, 4);
			//}
			{
				short x = 0;
				short y = 0;
				PathTile::UnPackLocation(path[0], x, y);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)x, 3);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)y, 4);
			}
			{
				short x = 0;
				short y = 0;
				PathTile::UnPackLocation(path[1], x, y);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)x, 3);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)y, 3);
			}
			{
				short x = 0;
				short y = 0;
				PathTile::UnPackLocation(path[2], x, y);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)x, 3);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)y, 2);
			}
			{
				short x = 0;
				short y = 0;
				PathTile::UnPackLocation(path[3], x, y);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)x, 3);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)y, 1);
			}
			{
				short x = 0;
				short y = 0;
				PathTile::UnPackLocation(path[4], x, y);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)x, 2);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)y, 1);
			}
			{
				short x = 0;
				short y = 0;
				PathTile::UnPackLocation(path[5], x, y);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)x, 2);
				Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual((int)y, 0);
			}

		}

	};
}
