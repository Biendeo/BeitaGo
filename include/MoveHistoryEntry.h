#pragma once

#include "Constants.h"
#include "Grid2.h"

namespace BeitaGo {
	/**
	 * Stores the history of a move.
	 */
	class MoveHistoryEntry {
		public:
		/**
		 * Constructs an entry with the given position and color.
		 */
		MoveHistoryEntry(const Grid2& position, Color color);

		/**
		 * Returns the position of the move.
		 * @return
		 */
		const Grid2& GetPosition() const;

		/**
		 * Returns the color of the move.
		 * @return
		 */
		const Color GetColor() const;

		private:
		Grid2 _position;
		Color _color;
	};
}
