#pragma once

#include "Common/PathFinder/PathFinderLocation.h"
#include "Common/PathFinder/PathFinderLocation.h"
#include "Common/Macro.h"

namespace PathFinder
{
	template <typename CELL_DATA>
	class Map
	{
	public:
		typedef std::function<void(PASS_LOCATION_CONST location, const CELL_DATA& data)> TChangeCallback;

		Map(const int width,
			const int height,
			const std::vector< CELL_DATA >& data,
			const CELL_DATA& invalidCellData
			)
		: m_width(width)
		, m_height(height)
		, m_invalidCellData(invalidCellData)
		, m_data(data)
		{
			return;
		}

		const CELL_DATA& Query(PASS_LOCATION_CONST location) const
		{
			const int offset = GetOffset(location);
			return m_data[offset];
		}
		const std::vector< CELL_DATA >& QueryData() const { return m_data; }

		void QueueModification(PASS_LOCATION_CONST location, const CELL_DATA& data)
		{
			m_modificationQueue.push_back(std::pair< Location, CELL_DATA >(location, data));
		}
		void ProcessModification()
		{
			for( const auto& item : m_modificationQueue)
			{
				const int offset = GetOffset(item.left);
				if (m_data[offset] != item.right)
				{
					m_data[offset] == item.right;
					NotifyChange(item.left, item.right);
				}
			}
			m_modificationQueue.clear();
		}

		void AddChangeCallback(const TChangeCallback& callback)
		{
			m_changeCallbackList.push_back(callback);
		}

		const int GetWidth() const { return m_width; }
		const int GetHeight() const { return m_height; }

	private:
		const int GetOffset(PASS_LOCATION_CONST location)
		{
			short x = 0;
			short y = 0;
			UnPackLocation(location, x, y);
			DSC_ASSERT((0 <= x) && (x < m_width));
			DSC_ASSERT((0 <= y) && (y < m_height));
			return (x + (y * m_width));
		}
		void NotifyChange(PASS_LOCATION_CONST location, const CELL_DATA& data)
		{
			for (const auto& item: m_changeCallbackList)
			{
				item(location, data);
			}
		}

	private:
		const int m_width;
		const int m_height;
		const CELL_DATA m_invalidCellData;
		std::vector< CELL_DATA > m_data;

		std::vector< TChangeCallback > m_changeCallbackList;
		std::vector< std::pair< Location, CELL_DATA > > m_modificationQueue;
	};
};