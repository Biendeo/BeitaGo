#include "Board.h"

namespace BeitaGo {
	Board::Board(const Grid2& dimensions) : _dimensions(dimensions) {
		_tiles = std::vector<std::vector<Color>>(dimensions.X());
		for (auto& v : _tiles) {
			//TODO: Double check that this actually makes a blank field, and not anything else.
			v = std::vector<Color>(dimensions.Y());
		}
		_whoseTurn = Color::Black;
		_turnCount = 1;
	}

	Grid2 Board::GetDimensions() const {
		return _dimensions;
	}

	Color Board::GetTile(const Grid2& position) const {
		//TODO: Error checking?
		return _tiles[position.X()][position.Y()];
	}

	Color Board::GetWhoseTurn() const {
		return _whoseTurn;
	}

	int Board::GetTurnCount() const {
		return _turnCount;
	}

	bool Board::IsGameOver() const {
		return false;
	}

	void Board::PlacePiece(const Grid2& position, Color color) {
		//TODO: This should be a valid move, but throw here if this is bad.
		_tiles[position.X()][position.Y()] = color;
	}

	void Board::NextTurn() {
		++_turnCount;
		_whoseTurn = _whoseTurn == Color::Black ? Color::White : Color::Black;
	}
}