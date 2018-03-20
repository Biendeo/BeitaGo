#include "Board.h"

#include <sstream>
#include <stdexcept>

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
		if (IsMoveValid(position, color)) {
			_tiles[position.X()][position.Y()] = color;
		} else {
			std::stringstream s;
			s << "Board tried to place a " << (color == Color::Black ? "BLACK" : "WHITE") << " piece at (" << position.X() << ", " << position.Y() << ") but it is invalid. Your AI or UI should check IsMoveValid() is true before calling PlacePiece() or ActDecision()!";
			throw std::runtime_error(s.str());
		}
	}

	bool Board::IsMoveValid(const Grid2& position, Color color) const {
		if (position.X() >= 0 && position.X() < GetDimensions().X() && position.Y() >= 0 && position.Y() < GetDimensions().Y() && GetTile(position) == Color::None) {
			//TODO: Use greater scrutiny here.
			return true;
		} else {
			return false;
		}
	}

	void Board::NextTurn() {
		++_turnCount;
		_whoseTurn = _whoseTurn == Color::Black ? Color::White : Color::Black;
	}
}