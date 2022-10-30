#pragma once

#include "Common/PathTile/PathTileLocation.h"
#include "Common/Macro.h"

namespace PathTile
{
	/*
	we never directly see CELL_DATA, we just get told traverable or not traversable
	 a PathTile has an extra row and column of overlap, so we don't need to talk to other tiles directly?
	 */
	template <int WIDTH, int HEIGHT>
	class PathTile
	{
	public:
		typedef std::bitset< (((WIDTH + 1) * (HEIGHT + 1)) - 1)> TTraversalData;
		typedef std::array< int, (((WIDTH + 1) * (HEIGHT + 1)) - 1)> TIntData;

		explicit PathTile(PASS_LOCATION_CONST offset, const TTraversalData& traversalData)
			: m_offset(offset)
			, m_traversalData(traversalData)
			, m_dirty(true)
		{
			return;
		}

		void Change(PASS_LOCATION_CONST location, const bool traversable)
		{
			const int offset = GetDataOffset(location);
			const bool oldValue = m_traversalData[offset];
			if (oldValue == traversable)
			{
				return;
			}
			m_traversalData[offset] = traversable;
			m_dirty = true;
		}

		//accessors are not const as may need to trigger a lazy init on data

		const int GetRegion(PASS_LOCATION_CONST location)
		{
			Update();
			const int offset = GetDataOffset(location);
			const int region = m_regionData[offset];
			return region;
		}

		void VisitEdgeTraversingPaths(PASS_LOCATION_CONST location, const std::function< const bool(PASS_LOCATION_CONST dest, const int cost) >& visitor)
		{
			Update();
			const int region = GetRegion(location);
			for (int index = 0; index < WIDTH; ++index)
			{
				const Location dest = PackLocation(index, 0);
				if (false == VisitEdge(region, Location, dest, visitor))
				{
					return;
				}
			}
			for (int index = 1; index < HEIGHT; ++index)
			{
				const Location dest = PackLocation(0, index);
				if (false == VisitEdge(region, Location, dest, visitor))
				{
					return;
				}
			}
			for (int index = 0; index < WIDTH; ++index)
			{
				const Location dest = PackLocation(index, HEIGHT);
				if (false == VisitEdge(region, Location, dest, visitor))
				{
					return;
				}
			}
			for (int index = 0; index < HEIGHT; ++index)
			{
				const Location dest = PackLocation(WIDTH, index);
				if (false == VisitEdge(region, Location, dest, visitor))
				{
					return;
				}
			}
		}

		//assert that start and end in same region
		void AppendPath(PASS_LOCATION_CONST start, PASS_LOCATION_CONST end, std::vector< Location >& path)
		{
			Update();
			const int regionId = GetRegion(start);
			DSC_ASSERT(regionId == GetRegion(end));
			const int offset = GetDataOffset(start);
			UpdateDistanceMap(regionId, start);
			const auto& item = m_distanceMapData[offset];
			//const int offsetEnd = GetDataOffset(end);
			const int distanceToBeat = item.m_distanceMap[end];

			AppendPathTraceBack(item.m_distanceMap, end, start, path, distanceToBeat);
		}

	private:
		// we should never be addressing any location outside the tile width/height and the two overlap edges
		const int GetDataOffset(PASS_LOCATION_CONST location)
		{
			short x = 0;
			short y = 0;
			UnPackLocationMinus(location, m_offset, x, y);
			DSC_ASSERT((0 <= x) && (x <= WIDTH));
			DSC_ASSERT((0 <= y) && (y <= HEIGHT)) ;
			DSC_ASSERT(false == ((x == WIDTH) && (y == HEIGHT)));

			int dataOffset = x + (y * (WIDTH + 1));
			DSC_ASSERT(dataOffset < ((WIDTH + 1) * (HEIGHT + 1)));

			return dataOffset;
		}

		void Update()
		{
			if (true != m_dirty)
			{
				return;
			}
			m_dirty = false;
			UpdateRegion();
			ClearDistanceMaps();
		}

		void FloodFillRegionDelta(const int regionId, const int indexX, const int indexY)
		{
			if ((indexX < 0) ||
				(WIDTH < indexX) ||
				(indexY < 0) ||
				(HEIGHT < indexY) ||
				((WIDTH == indexX) && (HEIGHT == indexY)) //should not hit this case when moving orthoganally, but just in case
				)
			{
				return;
			}
			const int offset = GetDataOffset(PackLocation(indexX, indexY));
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
			FloodFillRegion(regionId, indexX, indexY);
		}

		void FloodFillRegion(const int regionId, const int indexX, const int indexY)
		{
			FloodFillRegionDelta(regionId, indexX - 1, indexY);
			FloodFillRegionDelta(regionId, indexX + 1, indexY);
			FloodFillRegionDelta(regionId, indexX, indexY - 1);
			FloodFillRegionDelta(regionId, indexX, indexY + 1);
		}

		void UpdateRegion()
		{
			m_regionData.fill(0);
			int regionId = 0;
#if 1
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
							regionId += 1;
							m_regionData[trace] = regionId;
							FloodFillRegion(regionId, indexX, indexY);
						}
					}

					trace += 1;
				}
			}
#else
			bool bContinuity = false;
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
						if (false == bContinuity)
						{
							regionId += 1;
							bContinuity = true;
						}
						m_regionData[trace] = regionId;

						// when past first row, does the cell in the row above us have a region, if so repaint that region
						if (0 < indexY)
						{
							const int traceAbove = trace - (WIDTH + 1);
							const int regionAbove = m_regionData[traceAbove];
							const int traceAbove = trace - (WIDTH + 1);
							const int regionAbove = m_regionData[traceAbove];
							if (0 != regionAbove)
							{
								for (int index = 0; index < trace; ++index)
								{
									if (regionAbove == m_regionData[index])
									{
										m_regionData[index] = regionId;
									}
								}
							}
						}
					}
					else
					{
						bContinuity = false;
					}

					trace += 1;
				}
				bContinuity = false;
			}
#endif
		}

		void ClearDistanceMaps()
		{
			for (auto& iter : m_distanceMapData)
			{
				//if (true != iter.m_valid)
				//{
				//	continue;
				//}
				iter.m_valid = false;
				//iter.m_distanceMap.fill(0);
			}
		}

		//void VisitEdgeTraversingPaths(PASS_LOCATION_CONST location, const std::function< const bool(PASS_LOCATION_CONST dest, const int cost) >& visitor)

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
			UpdateDistanceMap(region, start);
			const int offset = GetDataOffset(start);
			const int offsetEnd = GetDataOffset(end);
			const int distance = m_distanceMapData[offset].m_distanceMap[offsetEnd] - 1;
			if (false == visitor(end, distance))
			{
				return false;
			}
			return true;
		}
		void UpdateDistanceMap(const int region, PASS_LOCATION_CONST location)
		{
			const int offset = GetDataOffset(location);
			auto& item = m_distanceMapData[offset];
			if (true == item.m_valid)
			{
				return;
			}
			item.m_distanceMap.fill(0);

			item.m_distanceMap[offset] = 1;
			short x = 0;
			short y = 0;
			UnPackLocation(location, x, y);
			UpdateDistanceMapDelta(item.m_distanceMap, region, x, y, 2);
		}
		void UpdateDistanceMapDelta(TIntData& dataMap, const int regionId, const int indexX, const int indexY, const int distance)
		{
			FillDistanceMapDelta(dataMap, regionId, indexX - 1, indexY, distance);
			FillDistanceMapDelta(dataMap, regionId, indexX + 1, indexY, distance);
			FillDistanceMapDelta(dataMap, regionId, indexX, indexY - 1, distance);
			FillDistanceMapDelta(dataMap, regionId, indexX, indexY + 1, distance);
		}
		void FillDistanceMapDelta(TIntData& dataMap, const int regionId, const int indexX, const int indexY, const int distance)
		{
			if ((indexX < 0) ||
				(WIDTH < indexX) ||
				(indexY < 0) ||
				(HEIGHT < indexY) ||
				((WIDTH == indexX) && (HEIGHT == indexY)) //should not hit this case when moving orthoganally, but just in case
				)
			{
				return;
			}
			const int offset = GetDataOffset(PackLocation(indexX, indexY));
			const bool traversable = m_traversalData[offset];
			if (false == traversable)
			{
				return;
			}
			const int currentDistance = dataMap[offset];
			if ((0 != currentDistance) && (currentDistance < distance))
			{
				return;
			}

			dataMap[offset] = distance;
			UpdateDistanceMapDelta(dataMap, regionId, indexX, indexY, distance + 1);
		}

		const bool AppendPathTraceBackDeta(const TIntData& dataMap, const int indexX, const int indexY, PASS_LOCATION_CONST start, std::vector< Location >& path, const int distanceToBeat)
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

			Location trace = PackLocation(indexX, indexY);
			if (trace == start)
			{
				//path.push_back(trace);
				return true;
			}
			const int offset = GetDataOffset(trace);
			const bool traversable = m_traversalData[offset];
			if (false == traversable)
			{
				return false;
			}
			const int newDistance = dataMap[offset];
			if (distanceToBeat <= newDistance)
			{
				return false;
			}

			if (true == AppendPathTraceBack(dataMap, trace, start, path, newDistance))
			{
				return true;
			}

			return false;
		}

		const bool AppendPathTraceBack(const TIntData& dataMap, PASS_LOCATION_CONST trace, PASS_LOCATION_CONST start, std::vector< Location >& path, const int distanceToBeat)
		{
			//const int offsetTrace = GetDataOffset(trace);
			//const int distance = dataMap[offsetTrace];
			short x = 0;
			short y = 0;
			UnPackLocation(trace, x, y);
			if (true == AppendPathTraceBackDeta(dataMap, x + 1, y, start, path, distanceToBeat))
			{
				path.push_back(trace);
				return true;
			}
			if (true == AppendPathTraceBackDeta(dataMap, x - 1, y, start, path, distanceToBeat))
			{
				path.push_back(trace);
				return true;
			}
			if (true == AppendPathTraceBackDeta(dataMap, x, y + 1, start, path, distanceToBeat))
			{
				path.push_back(trace);
				return true;
			}
			if (true == AppendPathTraceBackDeta(dataMap, x, y - 1, start, path, distanceToBeat))
			{
				path.push_back(trace);
				return true;
			}
			return false;
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
		TTraversalData m_traversalData;

		bool m_dirty;
		TIntData m_regionData;

		//keep a cache of distances from a given cell to all other cells of the same region, lazy init
		// todo: is a monilithic block of ints better performance than nxn std::array?
		struct DistanceMapData
		{
			bool m_valid;
			TIntData m_distanceMap; //hold a flood filed distance map from one given cell
		};
		std::array< DistanceMapData, (((WIDTH + 1) * (HEIGHT + 1)) - 1)> m_distanceMapData;

	};
};