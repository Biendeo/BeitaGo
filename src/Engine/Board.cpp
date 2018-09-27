#include "Board.h"

#include <functional>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>

#include <iostream>

namespace BeitaGo {
	Board::Board(const Grid2& dimensions) : _dimensions(dimensions) {
		_tiles = std::vector<std::vector<Color>>(dimensions.X(), std::vector<Color>(dimensions.Y(), Color::None));
		RecomputeGroupsAndLiberties();
		_whoseTurn = Color::Black;
		_blackPiecesTaken = 0;
		_whitePiecesTaken = 0;
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
		return _history.size() >= 2 && (_history.end() - 2)->GetPosition() == PASS && (_history.end() - 1)->GetPosition() == PASS;
	}

	void Board::PlacePiece(const Grid2& position, Color color) {
		if (IsMoveValid(position, color)) {
			if (position != PASS) {
				_tiles[position.X()][position.Y()] = color;
			}
			_history.emplace_back(position, color);
			// When placing a piece, only the neighbouring tiles' liberties are changed, so you
			// only need to check whether those should be removed.
			if (position != PASS) {
				//TODO: Can I do this smoother?
				RecomputeGroupsAndLiberties();
				auto piecesTakenLast = std::make_pair(GetBlackPiecesTaken(), GetWhitePiecesTaken());
				auto piecesTakenCurrent = std::make_pair(GetBlackPiecesTaken(), GetWhitePiecesTaken());
				Neighbors(position, [&](const Grid2& g) {
					if (GetTile(g) == (color == Color::Black ? Color::White : Color::Black)) {
						ClearPossibleTiles(g);
						piecesTakenCurrent = std::make_pair(GetBlackPiecesTaken(), GetWhitePiecesTaken());
						if (piecesTakenLast != piecesTakenCurrent) {
							RecomputeGroupsAndLiberties();
							piecesTakenLast = piecesTakenCurrent;
						}
					}
				});
				// If none of the other tiles were removed, this move could've been a suicide play,
				// so remove this group if it has zero liberties. It should only have zero liberties
				// if any of the other groups were not removed.
				piecesTakenCurrent = std::make_pair(GetBlackPiecesTaken(), GetWhitePiecesTaken());
				if (piecesTakenLast != piecesTakenCurrent) {
					RecomputeGroupsAndLiberties();
					piecesTakenLast = piecesTakenCurrent;
				}
				ClearPossibleTiles(position);
				piecesTakenCurrent = std::make_pair(GetBlackPiecesTaken(), GetWhitePiecesTaken());
				if (piecesTakenLast != piecesTakenCurrent) {
					RecomputeGroupsAndLiberties();
					piecesTakenLast = piecesTakenCurrent;
				}
			}
		} else {
			std::stringstream s;
			s << "Board tried to place a " << (color == Color::Black ? "BLACK" : "WHITE") << " piece at (" << position.X() << ", " << position.Y() << ") but it is invalid. Your AI or UI should check IsMoveValid() is true before calling PlacePiece() or ActDecision()!";
			throw std::runtime_error(s.str());
		}
	}

	bool Board::IsMoveValid(const Grid2& position, Color color) const {
		if (position == PASS || (IsWithinBoard(position) && GetTile(position) == Color::None && !IsMoveSuicidePlay(position, color))) {
			return true;
		} else {
			return false;
		}
	}

	bool Board::IsMoveSuicidePlay(const Grid2& position, Color color) const {
		int emptyNeighbors = 0;
		bool safeFriendlyGroup = false;
		Neighbors(position, [this, &emptyNeighbors, &safeFriendlyGroup, color](const Grid2& g) {
			if (GetTile(g) == Color::None) {
				++emptyNeighbors;
			} else if (GetTile(g) == color && _liberties[_groups[g.X()][g.Y()]] > 1 || _liberties[_groups[g.X()][g.Y()]] == 1) {
				safeFriendlyGroup = true;
			}
		});
		return emptyNeighbors == 0 && !safeFriendlyGroup;
	}

	bool Board::IsWithinBoard(const Grid2& position) const {
		return position.X() >= 0 && position.X() < GetDimensions().X() && position.Y() >= 0 && position.Y() < GetDimensions().Y();
	}

	std::vector<Grid2> Board::GetValidMoves(const Color& color) const {
		//? While this may not scale that well, I think it'll be satisfactory for even the larger
		//? board sizes people will use.
		std::vector<Grid2> v;
		v.reserve(GetDimensions().X() * GetDimensions().Y() + 1);

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
		std::vector<std::vector<Grid2>> noneTiles(_liberties.size());
		for (int y = 0; y < GetDimensions().Y(); ++y) {
			for (int x = 0; x < GetDimensions().X(); ++x) {
				if (GetTile(Grid2(x, y)) == Color::White) {
					++whiteTiles;
				} else if (GetTile(Grid2(x, y)) == Color::Black) {
					++blackTiles;
				} else {
					noneTiles[_groups[x][y]].emplace_back(x, y);
				}
			}
		}
		// Now to handle blank groups. If that group is surrounded exclusively by one color
		// then it belongs to that group. Otherwise it is not counted.
		int i = 0;
		for (const auto& group : noneTiles) {
			if (group.size() > 0) {
				bool seenWhite = false;
				bool seenBlack = false;
				for (const Grid2& b : group) {
					Neighbors(b, [this, &seenWhite, &seenBlack](const Grid2& g) {
						seenBlack |= (GetTile(g) == Color::Black);
						seenWhite |= (GetTile(g) == Color::White);
					});
					if (seenWhite && seenBlack) {
						break;
					}
				}
				if (seenWhite && !seenBlack) {
					whiteTiles += static_cast<int>(group.size());
				} else if (seenBlack && !seenWhite) {
					blackTiles += static_cast<int>(group.size());
				} else {
				}
			}
			++i;
		}
		return whiteTiles - blackTiles + GetKomi();
	}

	int Board::GetLiberties(const Grid2& position) const {
		if (!IsWithinBoard(position) || GetTile(position) == Color::None) {
			return -1;
		} else {
			return _liberties[_groups[position.X()][position.Y()]];
		}
	}

	std::vector<Grid2> Board::GetGroup(const Grid2& position) const {
		if (IsWithinBoard(position) && GetTile(position) != Color::None) {
			int groupIndex = _groups[position.X()][position.Y()];
			std::vector<Grid2> tiles;
			tiles.reserve(GetDimensions().X() * GetDimensions().Y());
			for (int x = 0; x < GetDimensions().X(); ++x) {
				for (int y = 0; y < GetDimensions().Y(); ++y) {
					if (_groups[x][y] == groupIndex) {
						tiles.emplace_back(x, y);
					}
				}
			}
			return tiles;
		} else {
			return std::vector<Grid2>();
		}
	}

	int Board::GetGroupLiberties(const std::vector<Grid2>& group) const {
		if (group.size() == 0) {
			return 0;
		} else {
			return GetLiberties(group[0]);
		}
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

	std::vector<MoveHistoryEntry> Board::GetHistory() const {
		return _history;
	}

	int Board::GetBlackPiecesTaken() const {
		return _blackPiecesTaken;
	}

	int Board::GetWhitePiecesTaken() const {
		return _whitePiecesTaken;
	}

	void Board::ClearPossibleTiles(const Grid2& position) {
		if (!IsWithinBoard(position)) {
			return;
		}
		if (_liberties[_groups[position.X()][position.Y()]] == 0) {
			for (const Grid2& space : GetGroup(position)) {
				if (_tiles[space.X()][space.Y()] == Color::White) {
					++_blackPiecesTaken;
				} else if (_tiles[space.X()][space.Y()] == Color::Black) {
					++_whitePiecesTaken;
				}
				_tiles[space.X()][space.Y()] = Color::None;
			}
		}
	}

	void Board::RecomputeGroupsAndLiberties() {
		_groups = std::vector<std::vector<int>>(_dimensions.X(), std::vector<int>(_dimensions.Y(), 0));
		int currentIndex = 1;
		for (int x = 0; x < _dimensions.X(); ++x) {
			for (int y = 0; y < _dimensions.Y(); ++y) {
				if (_groups[x][y] == 0) {
					_groups[x][y] = currentIndex;
					Color color = GetTile(Grid2(x, y));
					std::stack<Grid2> spacesToCheck;
					spacesToCheck.emplace(x, y);
					while (!spacesToCheck.empty()) {
						Grid2 currentSpace = spacesToCheck.top();
						spacesToCheck.pop();
						Neighbors(currentSpace, [this, &spacesToCheck, currentIndex, color](const Grid2& g) {
							if (_groups[g.X()][g.Y()] == 0 && GetTile(g) == color) {
								_groups[g.X()][g.Y()] = currentIndex;
								spacesToCheck.push(g);
							}
						});
					}
					++currentIndex;
				}
			}
		}

		_liberties = std::vector<int>(currentIndex, 0);
		for (int x = 0; x < _dimensions.X(); ++x) {
			for (int y = 0; y < _dimensions.Y(); ++y) {
				Color color = GetTile(Grid2(x, y));
				if (color == Color::None) {
					Neighbors(Grid2(x, y), [this](const Grid2& g) {
						++_liberties[_groups[g.X()][g.Y()]];
					});
				}
			}
		}
	}

	inline void Board::Neighbors(const Grid2& g, std::function<void(const Grid2&)> f) const {
		if (IsWithinBoard(g + Grid2(1, 0))) {
			f(g + Grid2(1, 0));
		}
		if (IsWithinBoard(g + Grid2(0, 1))) {
			f(g + Grid2(0, 1));
		}
		if (IsWithinBoard(g + Grid2(-1, 0))) {
			f(g + Grid2(-1, 0));
		}
		if (IsWithinBoard(g + Grid2(0, -1))) {
			f(g + Grid2(0, -1));
		}
	}
}