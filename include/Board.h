#pragma once

#include <vector>

#include "Constants.h"
#include "Grid2.h"

namespace BeitaGo {
	class Board {
		public:
		Board(const Grid2& dimensions);

		Grid2 GetDimensions() const;
		Color GetTile(const Grid2& position) const;
		Color GetWhoseTurn() const;
		int GetTurnCount() const;

		bool IsGameOver() const;

		void PlacePiece(const Grid2& position, Color color);
		void NextTurn();

		private:
		Grid2 _dimensions;
		std::vector<std::vector<Color>> _tiles;

		Color _whoseTurn;
		int _turnCount;
	};
}


