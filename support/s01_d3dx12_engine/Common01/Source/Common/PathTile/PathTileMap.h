#pragma once

#include "Common/PathTile/PathTileLocation.h"

namespace PathTile
{
	template <typename CELL_DATA>
	class Map
	{
	public:
		typedef std::function<void(const Location& location, const CELL_DATA& data)> TChangeCallback;

		Map(const int width,
			const int height,
			const std::vector< CELL_DATA >& data,
			const CELL_DATA& invalidCellData
			);

		const CELL_DATA& Query(const Location& location) const;
		const std::vector< CELL_DATA >& QueryData() const { return m_data; }

		void QueueModification(const Location& location, const CELL_DATA& data);
		void ProcessModification();

		void AddChangeCallback(const TChangeCallback& callback);

	private:
		const int m_width;
		const int m_height;
		const CELL_DATA m_invalidCellData;
		std::vector< CELL_DATA > m_data;

		std::vector< TChangeCallback > m_changeCallbackList;
		std::vector< std::pair< Location, CELL_DATA > > m_modificationQueue;
	};
};