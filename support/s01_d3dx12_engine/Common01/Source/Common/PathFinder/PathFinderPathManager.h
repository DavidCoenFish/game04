#pragma once

#include "Common/PathFinder/PathFinderLocation.h"
#include "Common/PathFinder/PathFinderPathTile.h"
#include "Common/PathFinder/PathFinderMap.h"

/*
get an array of Locations to get from one location to another
 - but how to know when it is invalidated? get a path instance which we move along?
 - if start and end locations are on the same region, then we just get the array of locations between
 - if start and end locations are NOT on the same region, move as far as we can in the start region

Region
	unique id (and we keep on incrementing on making each now region?)
	vector [ PathTile index, region id, change id ]
	best points for cardinal directions, to go to if we can't get to destination

how to get from a change to a tile, to correctly recalculating the region
how to calculate the regions~ flow across path tiles? get regions 
how to reduce region thrashing on small changes? may not be that bad? 
or do we have a PathTile region change id incremented if nb of regions or edge masks change?

Path
	region id
	vector [location]

make a 32bit flag for which edge cells hold a given region? ie, each PathTile region has a edge mask?
Region has a "set"/"map" of <path tile index, path tile region id> to know 
*/

namespace PathFinder
{
	template <
		typename CELL_DATA, 
		const bool (&FCELL_DATA_TRAVERSABLE)(const CELL_DATA&)
		>
	class PathManager
	{
	public:
		constexpr int s_dim = 8;
		typedef PathTile<s_dim, s_dim, char> TPathTile;
		typedef std::bitset< (((s_dim + 1) * (s_dim + 1)) - 1)> TBitArray;
		typedef Map<CELL_DATA> TMap;

		PathManager(TMap& map)
			: m_width(0)
			, m_height(0)
		{
			const int mapWidth = map.GetWidth();
			m_width = PadUpValue(mapWidth);
			const int mapHeight = map.GetHeight();
			m_height = PadUpValue(mapHeight);
			m_tileArray.reserve(m_width * m_height);

			map.AddChangeCallback([](PASS_LOCATION_CONST location, const CELL_DATA& data){
				Change(location, data);
				});

			for (int tileY = 0; tileY < m_height; ++tileY)
			{
				for (int tileX = 0; tileX < m_width; ++tileX)
				{
					TBitArray data;
					bool traversable = false;

					for (int traceY = 0; traceY <= s_dim; ++traceY)
					{
						for (int traceX = 0; traceX <= s_dim; ++traceX)
						{
							int offset = traceX + (traceY * (s_dim + 1));
							int locationX = (tileX * s_dim) + traceX;
							if (mapWidth < locationX)
							{
								continue;
							}
							int locationY = (tileY * s_dim) + traceY;
							if (mapHeight < locationY)
							{
								continue;
							}

							data[offset] = map.Query(PackLocation(locationX, locationY));
						}
					}

					m_tileArray.push_back(TPathTile(
						PackLocation(tileX * s_dim, tileY * s_dim),
						data
						));
				}
			}
		}


	private:
		void Change(PASS_LOCATION_CONST location, const CELL_DATA& data)
		{
			const int offset = GetTileOffset(location);
			const bool traversable = FCELL_DATA_TRAVERSABLE(data);
			m_tileArray[offset].Change(location, traversable);

			//deal with tile location overlap
			int overlapOffsetA = 0;
			int overlapOffsetB = 0;
			if (true == GetOverlapTileOffset(location, overlapOffsetA, overlapOffsetB))
			{
				if (-1 != overlapOffsetA)
				{
					m_tileArray[overlapOffsetA].Change(location, traversable);
				}
				if (-1 != overlapOffsetB)
				{
					m_tileArray[overlapOffsetB].Change(location, traversable);
				}
			}

			return;
		}

		const int PadUpValue(const int value)
		{
			int result = value / s_dim;
			if (0 != (value - (result * s_dim)))
			{
				result += 1;
			}
			return result;
		}

		const int GetTileOffset(PASS_LOCATION_CONST location)
		{
			short x = 0;
			short y = 0;
			UnPackLocation(location, x, y);
			const int tileX = x / s_dim;
			const int tileY = y / s_dim;
			DSC_ASSERT((0 <= x) && (x < m_width));
			DSC_ASSERT((0 <= y) && (y < m_height));
			const int offset = tileX + (tileY * m_width);
			return offset;
		}

		const bool GetOverlapTileOffset(PASS_LOCATION_CONST location, int& overlapOffsetA, int& overlapOffsetB)
		{
			short x = 0;
			short y = 0;
			UnPackLocation(location, x, y);
			const int tileX = x / s_dim;
			const int tileY = y / s_dim;
			bool result = false;
			overlapOffsetA = -1;
			overlapOffsetB = -1;
			if ((0 == (x % s_dim)) && (1 < tileX))
			{
				result = true;
				overlapOffsetA = (tileX - 1) + (tileY * m_width)
			}
			if ((0 == (y % s_dim)) && (1 < tileY))
			{
				result = true;
				overlapOffsetB = tileX + ((tileY - 1) * m_width);
			}

			return result;
		}

	private:
		std::vector< TPathTile > m_tileArray;
		int m_width;
		int m_height;

	};
};