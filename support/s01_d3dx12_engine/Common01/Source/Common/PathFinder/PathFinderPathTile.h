#pragma once

#include "Common/PathFinder/PathFinderLocation.h"
#include "Common/Macro.h"

namespace PathFinder
{
	/*
	we never directly see CELL_DATA, we just get told traverable or not traversable
	 a PathTile has an extra row and column of overlap, so we don't need to talk to other tiles directly?
	DATA_TYPE == char, for WIDTH == 8 and HEIGHT == 8, needs to be big enough to hold max regions and max distance
	 */
	template <int WIDTH, int HEIGHT, typename DATA_TYPE>
	class PathTile
	{
	public:
		typedef std::bitset< (((WIDTH + 1) * (HEIGHT + 1)) - 1)> TBitArray;
		typedef std::array< DATA_TYPE, (((WIDTH + 1) * (HEIGHT + 1)) - 1)> TDataArray;
		typedef std::array< int, (((WIDTH + 1) * (HEIGHT + 1)) - 1)> TIntArray;
		typedef std::array< int, (WIDTH + HEIGHT) * 2> TIntEdgeArray;

		explicit PathTile(PASS_LOCATION_CONST offset, const TBitArray& traversalData)
			: m_offset(offset)
			, m_traversalData(traversalData)
			, m_traversalChangeID(1)
			, m_regionData()
			, m_regionDataChangeID(0)
			, m_regionCount(0)
			, m_regionEdgeMaskChangeId(0)
			, m_regionEdgeMask()
		{
			for (auto& iter : m_distanceMapData)
			{
				iter.m_distanceMapChangeId = 0;
			}

			return;
		}

		const bool Change(PASS_LOCATION_CONST location, const bool traversable)
		{
			const int offset = GetDataOffset(location);
			const bool oldValue = m_traversalData[offset];
			if (oldValue == traversable)
			{
				return false;
			}
			m_traversalData[offset] = traversable;
			m_traversalChangeID += 1;
			return true;
		}

		//accessors are not const as may need to trigger a lazy init on data

		const int GetRegion(PASS_LOCATION_CONST location)
		{
			UpdateRegion();
			const int offset = GetDataOffset(location);
			const int region = m_regionData[offset];
			return region;
		}
		const int GetRegionCount()
		{
			UpdateRegion();
			return m_regionCount;
		}
		const int GetRegionMask(const int regionIndex)
		{
			UpdateRegion();
			return m_regionEdgeMask[regionIndex];
		}

		void VisitEdgeTraversingPaths(PASS_LOCATION_CONST location, const std::function< const bool(PASS_LOCATION_CONST dest, const int cost) >& visitor)
		{
			short dx = 0;
			short dy = 0;
			UnPackLocation(m_offset, dx, dy);

			const int region = GetRegion(location);
			if (0 == region)
			{
				//assert? what is happening
				return;
			}
			for (int index = 0; index < WIDTH; ++index)
			{
				const Location dest = PackLocation(index + dx, 0 + dy);
				if (false == VisitEdge(region, location, dest, visitor))
				{
					return;
				}
			}
			for (int index = 1; index < HEIGHT; ++index)
			{
				const Location dest = PackLocation(0 + dx, index + dy);
				if (false == VisitEdge(region, Location, dest, visitor))
				{
					return;
				}
			}
			for (int index = 0; index < WIDTH; ++index)
			{
				const Location dest = PackLocation(index + dx, HEIGHT + dy);
				if (false == VisitEdge(region, Location, dest, visitor))
				{
					return;
				}
			}
			for (int index = 0; index < HEIGHT; ++index)
			{
				const Location dest = PackLocation(WIDTH + dx, index + dy);
				if (false == VisitEdge(region, Location, dest, visitor))
				{
					return;
				}
			}
		}

		//assert that start and end in same region, add to path the series of Location needed to travers to get from start to end (not including start), ie, start is not added to path
		void AppendPath(PASS_LOCATION_CONST start, PASS_LOCATION_CONST end, std::vector< Location >& path)
		{
			DSC_ASSERT(GetRegion(start) == GetRegion(end));

			const TDataArray& distanceMap = GetDistanceMap(end);
			auto trace = start;
			short x = 0;
			short y = 0;
			UnPackLocation(trace, x, y);
			short dx = 0;
			short dy = 0;
			UnPackLocation(m_offset, dx, dy);
			int dataOffset = (x - dx) + ((y - dy) * (WIDTH + 1));
			DATA_TYPE traceDistance = distanceMap[dataOffset];
			DSC_ASSERT(0 != traceDistance);
			Location newLocation;

			while (trace != end)
			{
				traceDistance -= 1;
				if (true == ValidAndDistanceMatch(distanceMap, traceDistance, PackLocation(x + 1, y), newLocation))
				{
					path.push_back(newLocation);
					trace = newLocation;
				}
				else if (true == ValidAndDistanceMatch(distanceMap, traceDistance, PackLocation(x - 1, y), newLocation))
				{
					path.push_back(newLocation);
					trace = newLocation;
				}
				else if (true == ValidAndDistanceMatch(distanceMap, traceDistance, PackLocation(x, y + 1), newLocation))
				{
					path.push_back(newLocation);
					trace = newLocation;
				}
				else if (true == ValidAndDistanceMatch(distanceMap, traceDistance, PackLocation(x, y - 1), newLocation))
				{
					path.push_back(newLocation);
					trace = newLocation;
				}
				else
				{
					DSC_ASSERT_ALWAYS;
				}
				UnPackLocation(trace, x, y);
			}
		}
		Location MakeLocationForBit(const int bitIndex)
		{
			short dx = 0;
			short dy = 0;
			UnPackLocation(m_offset, dx, dy);
			int trace = bitIndex;
			if (trace < WIDTH)
			{
				return PackLocation(trace + dx, dy);
			}
			trace -= WIDTH;
			if (trace < HEIGHT)
			{
				return PackLocation(dx, trace + dy);
			}
			trace -= HEIGHT;
			if (trace < WIDTH)
			{
				return PackLocation(trace + dx, HEIGHT + dy);
			}
			trace -= WIDTH;
			if (trace < HEIGHT)
			{
				return PackLocation(WIDTH + dx, trace + dy);
			}
			DSC_ASSERT_ALWAYS;
			return 0;
		}
	private:
		const TDataArray& GetDistanceMap(PASS_LOCATION_CONST location)
		{
			const int dataOffset = GetDataOffset(location);
			auto& data = m_distanceMapData[dataOffset];
			if (data.m_distanceMapChangeId != m_traversalChangeID)
			{
				data.m_distanceMapChangeId = m_traversalChangeID;
				data.m_distanceMap.fill(0);

				data.m_distanceMap[dataOffset] = 1;
				short x = 0;
				short y = 0;
				UnPackLocationMinus(location, m_offset, x, y);

				UpdateDistanceMapDelta(data.m_distanceMap, x, y, 2);
			}
			return data.m_distanceMap;
		}

		const bool ValidAndDistanceMatch(const TDataArray& distanceMap, const DATA_TYPE matchDistance, PASS_LOCATION_CONST location, Location& out_location)
		{
			short x = 0;
			short y = 0;
			UnPackLocationMinus(location, m_offset, x, y);
			if (false == TestCoordsValid(x, y))
			{
				return false;
			}
			int dataOffset = x + (y * (WIDTH + 1));
			if (matchDistance != distanceMap[dataOffset])
			{
				return false;
			}
			out_location = location;
			return true;
		}

		// we should never be addressing any location outside the tile width/height and the two overlap edges
		const int GetDataOffset(PASS_LOCATION_CONST location)
		{
			short x = 0;
			short y = 0;
			UnPackLocationMinus(location, m_offset, x, y);
			DSC_ASSERT((0 <= x) && (x <= WIDTH));
			DSC_ASSERT((0 <= y) && (y <= HEIGHT));
			DSC_ASSERT(false == ((x == WIDTH) && (y == HEIGHT)));

			int dataOffset = BuildDataOffset(x, y);
			DSC_ASSERT(dataOffset < ((WIDTH + 1) * (HEIGHT + 1)));

			return dataOffset;
		}
		const int BuildDataOffset(const int x, const int y)
		{
			const int dataOffset = x + (y * (WIDTH + 1));
			return dataOffset;
		}

		const bool TestCoordsValid(const int indexX, const int indexY)
		{
			if ((indexX < 0) ||
				(WIDTH < indexX) ||
				(indexY < 0) ||
				(HEIGHT < indexY) ||
				((WIDTH == indexX) && (HEIGHT == indexY)) //should not hit this case when moving orthoganally, but just in case
				)
			{
				return false;
			}
			return true;
		}
		void FloodFillRegion(const int regionId, const int indexX, const int indexY)
		{
			if (false == TestCoordsValid(indexX, indexY))
			{
				return;
			}
			const int offset = indexX + (indexY * (WIDTH + 1)); //GetDataOffset(PackLocation(indexX, indexY));
			const bool traversable = m_traversalData[offset];
			if (false == traversable)
			{
				return;
			}
			const int currentRegionId = m_regionData[offset];
			if (0 != currentRegionId)
			{
				return;
			}
			m_regionData[offset] = regionId;
			FloodFillRegionDelta(regionId, indexX, indexY);
		}
		void FloodFillRegionDelta(const int regionId, const int indexX, const int indexY)
		{
			FloodFillRegion(regionId, indexX - 1, indexY);
			FloodFillRegion(regionId, indexX + 1, indexY);
			FloodFillRegion(regionId, indexX, indexY - 1);
			FloodFillRegion(regionId, indexX, indexY + 1);
		}
		void UpdateRegion()
		{
			if (m_traversalChangeID == m_regionDataChangeID)
			{
				return;
			}
			m_regionDataChangeID = m_traversalChangeID;

			//update region map
			m_regionData.fill(0);
			m_regionCount = 0;
			int trace = 0;
			for (int indexY = 0; indexY <= HEIGHT; ++indexY)
			{
				for (int indexX = 0; indexX <= WIDTH; ++indexX)
				{
					if ((indexY == HEIGHT) && (indexX == WIDTH))
					{
						break;
					}
					const bool traversable = m_traversalData[trace];
					if (true == traversable)
					{
						const int currentRegionId = m_regionData[trace];
						if (0 == currentRegionId)
						{
							m_regionCount += 1;
							const int regionId = m_regionCount;
							m_regionData[trace] = regionId;
							FloodFillRegionDelta(regionId, indexX, indexY);
						}
					}
					trace += 1;
				}
			}

			//update region edge map
			TIntEdgeArray newEdgeArray;
			for (int index = 1; index < m_regionCount; ++index) //region zero is for non traversable and not tracked
			{
				newEdgeArray[index] = MakeEdgeMask(index);
			}
			if (m_regionEdgeMask != newEdgeArray)
			{
				m_regionEdgeMask = newEdgeArray;
				m_regionEdgeMaskChangeId += 1;
			}
		}
		const bool VisitEdge(const int region, PASS_LOCATION_CONST start, PASS_LOCATION_CONST end, const std::function< const bool(PASS_LOCATION_CONST dest, const int cost) >& visitor)
		{
			if (start == end)
			{
				return true;
			}
			const int endRegion = GetRegion(end);
			if (start != end)
			{
				return true;
			}
			const auto& distanceMap = GetDistanceMap(start);
			const int dataOffset = GetDataOffset(end);
			const int distance = distanceMap[dataOffset] - 1;
			if (false == visitor(end, distance))
			{
				return false;
			}
			return true;
		}
		void UpdateDistanceMap(TDataArray& dataMap, const int x, const int y, const int distance)
		{
			if (false == TestCoordsValid(x, y))
			{
				return;
			}
			const int dataOffset = x + (y * (WIDTH + 1));
			if (false == m_traversalData[dataOffset])
			{
				return;
			}
			const auto oldValue = dataMap[dataOffset];
			if ((0 != oldValue) && (oldValue < distance))
			{
				return;
			}

			dataMap[dataOffset] = distance;
			UpdateDistanceMapDelta(dataMap, x, y, distance + 1);
		}
		void UpdateDistanceMapDelta(TDataArray& dataMap, const int x, const int y, const int distance)
		{
			UpdateDistanceMap(dataMap, x - 1, y, distance);
			UpdateDistanceMap(dataMap, x + 1, y, distance);
			UpdateDistanceMap(dataMap, x, y - 1, distance);
			UpdateDistanceMap(dataMap, x, y + 1, distance);
		}
		const int MakeEdgeMask(const int regionId)
		{
			int result = 0;
			int trace = 0;
			for (int index = 0; index < WIDTH; ++index)
			{
				const int dataOffset = BuildDataOffset(index, 0);
				const bool value = (regionId == m_regionData[dataOffset]);
				if (true == value)
				{
					result |= (1<<trace);
				}
				trace += 1;
			}
			for (int index = 0; index < HEIGHT; ++index)
			{
				const int dataOffset = BuildDataOffset(0, index);
				const bool value = (regionId == m_regionData[dataOffset]);
				if (true == value)
				{
					result |= (1<<trace);
				}
				trace += 1;
			}
			for (int index = 0; index < WIDTH; ++index)
			{
				const int dataOffset = BuildDataOffset(index, HEIGHT);
				const bool value = (regionId == m_regionData[dataOffset]);
				if (true == value)
				{
					result |= (1<<trace);
				}
				trace += 1;
			}
			for (int index = 0; index < HEIGHT; ++index)
			{
				const int dataOffset = BuildDataOffset(index, WIDTH);
				const bool value = (regionId == m_regionData[dataOffset]);
				if (true == value)
				{
					result |= (1<<trace);
				}
				trace += 1;
			}

			return result;
		}

	private:
		const Location m_offset;

		/* 4x2 would look like
		
		m_traversalMain       m_traversalOverlapColumn
		 0,0  1,0  2,0  3,0    4,0
		 0,1  1,1  2,1  3,1    4,1

		m_traversalOverlapRow
		 0,2  1,2  2,2  3,2

		*/
		//std::bitset< (WIDTH * HEIGHT)> m_traversalMain;
		//std::bitset< HEIGHT > m_traversalOverlapColumn;
		//std::bitset< WIDTH > m_traversalOverlapRow;
		TBitArray m_traversalData;
		int m_traversalChangeID;

		TDataArray m_regionData;
		int m_regionDataChangeID;//copy of the m_traversalChangeID that m_regionData was generated from
		int m_regionCount;

		//keep a cache of distances from a given cell to all other cells of the same region, lazy init
		// todo: is a monilithic block of ints better performance than nxn std::array?
		struct DistanceMapData
		{
			int m_distanceMapChangeId; //copy of the m_traversalChangeID this was generated from
			TDataArray m_distanceMap; //hold a flood filed distance map from one given cell
		};
		std::array< DistanceMapData, (((WIDTH + 1) * (HEIGHT + 1)) - 1)> m_distanceMapData;

		int m_regionEdgeMaskChangeId;
		TIntEdgeArray m_regionEdgeMask;

	};
};