#include "Board.h"

#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>

#include <iostream>

namespace BeitaGo {
	Board::Board(const Grid2& dimensions) : _dimensions(dimensions) {
		_tiles = std::vector<std::vector<Color>>(dimensions.X());
		for (auto& v : _tiles) {
			//TODO: Double check that this actually makes a blank field, and not anything else.
			v = std::vector<Color>(dimensions.Y());
		}
		_whoseTurn = Color::Black;
		_turnCount = 1;
		_komi = 6.5;
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
			if (position != PASS) {
				_tiles[position.X()][position.Y()] = color;
			}
			_history.emplace_back(position, color);
			//TODO: Double check if you can solve this by just checking the neighbouring tiles.
			// When placing a piece, only the neighbouring tiles' liberties are changed, so you
			// only need to check whether those should be removed.
			if (position != PASS) {
				ClearPossibleTiles(position + Grid2(1, 0));
				ClearPossibleTiles(position + Grid2(0, 1));
				ClearPossibleTiles(position + Grid2(-1, 0));
				ClearPossibleTiles(position + Grid2(0, -1));
				// If none of the other tiles were removed, this move could've been a suicide play,
				// so remove this group if it has zero liberties. It should only have zero liberties
				// if any of the other groups were not removed.
				ClearPossibleTiles(position);
			}
		} else {
			std::stringstream s;
			s << "Board tried to place a " << (color == Color::Black ? "BLACK" : "WHITE") << " piece at (" << position.X() << ", " << position.Y() << ") but it is invalid. Your AI or UI should check IsMoveValid() is true before calling PlacePiece() or ActDecision()!";
			throw std::runtime_error(s.str());
		}
	}

	bool Board::IsMoveValid(const Grid2& position, Color color) const {
		if (position == PASS || (IsWithinBoard(position) && GetTile(position) == Color::None)) {
			//TODO: Use greater scrutiny here.
			return true;
		} else {
			return false;
		}
	}

	bool Board::IsWithinBoard(const Grid2& position) const {
		return position.X() >= 0 && position.X() < GetDimensions().X() && position.Y() >= 0 && position.Y() < GetDimensions().Y();
	}

	std::vector<Grid2> Board::GetValidMoves(const Color& color) const {
		//? While this may not scale that well, I think it'll be satisfactory for even the larger
		//? board sizes people will use.
		std::vector<Grid2> v;

		for (int y = 0; y < GetDimensions().Y(); ++y) {
			for (int x = 0; x < GetDimensions().X(); ++x) {
				if (IsMoveValid(Grid2(x, y), color)) {
					v.emplace_back(x, y);
				}
			}
		}

		v.push_back(PASS);

		return v;
	}

	double Board::GetKomi() const {
		return _komi;
	}

	void Board::SetKomi(double komi) {
		_komi = komi;
	}

	double Board::Score() const {
		return ScoreArea();
	}

	double Board::ScoreArea() const {
		//TODO: This is not accurate!
		int whiteTiles = 0;
		int blackTiles = 0;
		for (int y = 0; y < GetDimensions().Y(); ++y) {
			for (int x = 0; x < GetDimensions().X(); ++x) {
				if (GetTile(Grid2(x, y)) == Color::White) {
					++whiteTiles;
				} else if (GetTile(Grid2(x, y)) == Color::Black) {
					++blackTiles;
				}
			}
		}
		return whiteTiles - blackTiles + GetKomi();
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

	std::vector<Grid2> Board::GetGroup(const Grid2& position) const {
		if (IsWithinBoard(position) && GetTile(position) != Color::None) {
			Color color = GetTile(position);
			auto setComp = [](const Grid2& a, const Grid2& b) { return a.X() < b.X() || (a.X() == b.X() && a.Y() < b.Y()); };
			std::set<Grid2, decltype(setComp)> checkedSpaces(setComp);
			std::set<Grid2, decltype(setComp)> connectedSpaces(setComp);
			std::queue<Grid2> spacesToCheck;

			spacesToCheck.push(position);
			checkedSpaces.insert(position);

			while (!spacesToCheck.empty()) {
				Grid2 currentSpace = spacesToCheck.front();
				spacesToCheck.pop();
				// Any tile outside of the board can be ignored, as they'll still border the territory.
				if (IsWithinBoard(currentSpace) && GetTile(currentSpace) == color) {
					connectedSpaces.insert(currentSpace);
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
			}
			return std::vector<Grid2>(connectedSpaces.begin(), connectedSpaces.end());
		} else {
			return std::vector<Grid2>();
		}
	}

	int Board::GetGroupLiberties(const std::vector<Grid2>& group) const {
		if (group.size() == 0) {
			return 0;
		}
		Color color = GetTile(group[0]);
		auto setComp = [](const Grid2& a, const Grid2& b) { return a.X() < b.X() || (a.X() == b.X() && a.Y() < b.Y()); };
		std::set<Grid2, decltype(setComp)> checkedSpaces(setComp);
		std::set<Grid2, decltype(setComp)> connectedLiberties(setComp);

		for (const Grid2& space : group) {
			std::vector<Grid2> neighbouringSpaces;
			neighbouringSpaces.emplace_back(space + Grid2(1, 0));
			neighbouringSpaces.emplace_back(space + Grid2(0, 1));
			neighbouringSpaces.emplace_back(space + Grid2(-1, 0));
			neighbouringSpaces.emplace_back(space + Grid2(0, -1));

			for (const Grid2& neighbouringSpace : neighbouringSpaces) {
				if (IsWithinBoard(neighbouringSpace) && GetTile(neighbouringSpace) == Color::None) {
					connectedLiberties.insert(neighbouringSpace);
				}
			}
		}

		return static_cast<int>(connectedLiberties.size());
	}

	void Board::NextTurn() {
		++_turnCount;
		_whoseTurn = _whoseTurn == Color::Black ? Color::White : Color::Black;
	}

	void Board::RewindBoard(int numTurns) {
		Board newBoard = GetPreviousState(numTurns);
		this->_tiles = newBoard._tiles;
		this->_history = newBoard._history;
		this->_turnCount = newBoard._turnCount;
		this->_whoseTurn = _whoseTurn;
	}

	Board Board::GetPreviousState(int numTurns) const {
		// For constant rewinds, this may not actually be efficient and the history should store
		// the board placement instead.
		if (numTurns <= 0) {
			return *this;
		}
		Board newBoard(GetDimensions());
		if (numTurns < _history.size()) {

			for (auto it = _history.begin(); it != _history.begin() + (_history.size() - numTurns); ++it) {
				newBoard.PlacePiece(it->GetPosition(), it->GetColor());
				newBoard.NextTurn();
			}
		}
		return newBoard;
	}

	void Board::ClearPossibleTiles(const Grid2& position) {
		if (!IsWithinBoard(position)) {
			return;
		}
		//! This is the biggest sink to computation time; can this be faster?
		std::vector<Grid2> group = GetGroup(position);
		if (GetGroupLiberties(group) == 0) {
			for (const Grid2& space : group) {
				_tiles[space.X()][space.Y()] = Color::None;
			}
		}
	}
}