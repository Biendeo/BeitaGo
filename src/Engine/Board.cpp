#include "Board.h"

#include <queue>
#include <set>
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
			_history.emplace_back(position, color);
			//TODO: Double check if you can solve this by just checking the neighbouring tiles.
			for (int y = 0; y < GetDimensions().Y(); ++y) {
				for (int x = 0; x < GetDimensions().X(); ++x) {
					ClearPossibleTiles(Grid2(x, y));
				}
			}
		} else {
			std::stringstream s;
			s << "Board tried to place a " << (color == Color::Black ? "BLACK" : "WHITE") << " piece at (" << position.X() << ", " << position.Y() << ") but it is invalid. Your AI or UI should check IsMoveValid() is true before calling PlacePiece() or ActDecision()!";
			throw std::runtime_error(s.str());
		}
	}

	bool Board::IsMoveValid(const Grid2& position, Color color) const {
		if (IsWithinBoard(position) && GetTile(position) == Color::None) {
			//TODO: Use greater scrutiny here.
			return true;
		} else {
			return false;
		}
	}

	int Board::GetLiberties(const Grid2& position) const {
		if (!IsWithinBoard(position) || GetTile(position) == Color::None) {
			return -1;
		} else {
			int liberties = 0;
			if (IsWithinBoard(position + Grid2(1, 0)) && GetTile(position + Grid2(1, 0)) == Color::None) {
				++liberties;
			}
			if (IsWithinBoard(position + Grid2(0, 1)) && GetTile(position + Grid2(0, 1)) == Color::None) {
				++liberties;
			}
			if (IsWithinBoard(position + Grid2(-1, 0)) && GetTile(position + Grid2(-1, 0)) == Color::None) {
				++liberties;
			}
			if (IsWithinBoard(position + Grid2(0, -1)) && GetTile(position + Grid2(0, -1)) == Color::None) {
				++liberties;
			}
			return liberties;
		}
	}

	bool Board::IsWithinBoard(const Grid2& position) const {
		return position.X() >= 0 && position.X() < GetDimensions().X() && position.Y() >= 0 && position.Y() < GetDimensions().Y();
	}

	void Board::NextTurn() {
		++_turnCount;
		_whoseTurn = _whoseTurn == Color::Black ? Color::White : Color::Black;
	}

	void Board::ClearPossibleTiles(const Grid2& position) {
		// Since we know the last move based on the history, just do a breadth-first search to see
		// if it's trapped.
		auto setComp = [](const Grid2& a, const Grid2& b) { return a.X() < b.X() || (a.X() == b.X() && a.Y() < b.Y()); };
		std::set<Grid2, decltype(setComp)> checkedSpaces(setComp);
		std::set<Grid2, decltype(setComp)> connectedSpaces(setComp);
		std::queue<Grid2> spacesToCheck;

		Color color = GetTile(position);
		if (color == Color::None) {
			return;
		}
		Color opponentColor = color == Color::Black ? Color::White : Color::Black;

		spacesToCheck.push(position);
		checkedSpaces.insert(position);
		while (!spacesToCheck.empty()) {
			Grid2 currentSpace = spacesToCheck.front();
			spacesToCheck.pop();
			// Any tile outside of the board can be ignored, as they'll still border the territory.
			if (IsWithinBoard(currentSpace)) {
				if (GetTile(currentSpace) == color) {
					connectedSpaces.insert(currentSpace);
					if (GetLiberties(currentSpace) > 0) {
						// If a same color tile has liberties, the block won't be removed.
						return;
					} else {
						if (checkedSpaces.find(currentSpace + Grid2(1, 0)) == checkedSpaces.end()) {
							spacesToCheck.push(currentSpace + Grid2(1, 0));
							checkedSpaces.insert(currentSpace + Grid2(1, 0));
						}
						if (checkedSpaces.find(currentSpace + Grid2(0, 1)) == checkedSpaces.end()) {
							spacesToCheck.push(currentSpace + Grid2(0, 1));
							checkedSpaces.insert(currentSpace + Grid2(0, 1));
						}
						if (checkedSpaces.find(currentSpace + Grid2(-1, 0)) == checkedSpaces.end()) {
							spacesToCheck.push(currentSpace + Grid2(-1, 0));
							checkedSpaces.insert(currentSpace + Grid2(-1, 0));
						}
						if (checkedSpaces.find(currentSpace + Grid2(0, -1)) == checkedSpaces.end()) {
							spacesToCheck.push(currentSpace + Grid2(0, -1));
							checkedSpaces.insert(currentSpace + Grid2(0, -1));
						}
					}
				} else if (GetTile(currentSpace) == opponentColor) {
					// Do nothing here, this is how we know we're surrounded (similar to off the
					// board spaces.
				} else {
					// This shouldn't appear (just because liberties are checked already, but just
					// return.
					return;
				}
			}
		}

		// If the function is up to here, then we didn't find any tiles with liberties, and thus
		// they need to be removed.
		for (const auto& space : connectedSpaces) {
			_tiles[space.X()][space.Y()] = Color::None;
		}
	}
}