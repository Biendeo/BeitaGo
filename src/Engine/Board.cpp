#include "Board.h"

#include <functional>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>

#include <iostream>

namespace BeitaGo {
	Board::Board(const Grid2& dimensions) : _dimensions(dimensions), _groups{dimensions.Y(), std::vector<int>(dimensions.X(), -1)}, _liberties(dimensions.X() * dimensions.Y(), -1) {
		_tiles = std::vector<std::vector<Color>>(dimensions.X(), std::vector<Color>(dimensions.Y(), Color::None));
		_whoseTurn = Color::Black;
		_blackPiecesTaken = 0;
		_whitePiecesTaken = 0;
		_turnCount = 1;
		_komi = 6.5;
		_score = _komi;
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
			_lastMoves.push_back(_tiles);
			if (position != PASS) {
				_tiles[position.X()][position.Y()] = color;
				// Compute the number of liberties and neighbouring friendly tiles.
				int additionalLiberties = 0;
				int neighbouringFriendlies = 0;
				Neighbors(position, [&](const Grid2& g) {
					if (GetTile(g) == Color::None) {
						++additionalLiberties;
					} else if (GetTile(g) == color) {
						++neighbouringFriendlies;
					}
				});

				// Compute groups.
				int foundGroup = -1;
				Neighbors(position, [&](const Grid2& g) {
					if (GetTile(g) == color) {
						if (foundGroup == -1) {
							// If we haven't yet found a neighbouring group, then let's take their
							// group number. We can add the number of liberties of our tile to
							// theirs (minus one because the tile itself takes a liberty).
							foundGroup = _groups[g.X()][g.Y()];
							_groups[position.X()][position.Y()] = foundGroup;
							--additionalLiberties;
							_liberties[foundGroup] += additionalLiberties;
						} else if (_groups[g.X()][g.Y()] != foundGroup) {
							// If we have found a group before AND this one is of a different group
							// then we need to clear that group.
							int groupToClear = _groups[g.X()][g.Y()];
							int oldGroupLiberties = _liberties[groupToClear];
							for (int x = 0; x < GetDimensions().X(); ++x) {
								for (int y = 0; y < GetDimensions().Y(); ++y) {
									if (_groups[x][y] == groupToClear) {
										_groups[x][y] = foundGroup;
									}
								}
							}
							_liberties[foundGroup] += (oldGroupLiberties - 1);
							_liberties[groupToClear] = -1;
						} else {
							// If we have found a group before AND this is the same group, then we
							// cleared a liberty twice.
							--_liberties[foundGroup];
						}
					} else if (GetTile(g) == (color == Color::Black ? Color::White : Color::Black)) {
						// If the neighbouring tile is of another colour, their group will go down
						// by one liberty.
						int groupToSubtract = _groups[g.X()][g.Y()];
						--_liberties[groupToSubtract];
						// If their liberties is now zero
						if (_liberties[groupToSubtract] == 0) {
							int& points = (color == Color::Black ? _blackPiecesTaken : _whitePiecesTaken);
							for (int x = 0; x < GetDimensions().X(); ++x) {
								for (int y = 0; y < GetDimensions().Y(); ++y) {
									if (_groups[x][y] == groupToSubtract) {
										_tiles[x][y] = Color::None;
										_groups[x][y] = -1;
										++points;
										Neighbors(Grid2(x, y), [this, groupToSubtract, &additionalLiberties, &position](const Grid2& g) {
											if (_groups[g.X()][g.Y()] != -1 && _groups[g.X()][g.Y()] != groupToSubtract) {
												++_liberties[_groups[g.X()][g.Y()]];
											} else if (g == position) {
												++additionalLiberties;
											}
										});
									}
								}
							}
							_liberties[groupToSubtract] = -1;
						}
					}
				});

				if (foundGroup == -1) {
					for (int a = 0; a < _liberties.size(); ++a) {
						if (_liberties[a] == -1) {
							foundGroup = a;
							break;
						}
					}
					_groups[position.X()][position.Y()] = foundGroup;
					_liberties[foundGroup] = additionalLiberties;
				}

				if (_liberties[foundGroup] == 0) {
					int& points = (color == Color::Black ? _whitePiecesTaken : _blackPiecesTaken);
					for (int x = 0; x < GetDimensions().X(); ++x) {
						for (int y = 0; y < GetDimensions().Y(); ++y) {
							if (_groups[x][y] == foundGroup) {
								_tiles[x][y] = Color::None;
								_groups[x][y] = -1;
								++points;
								Neighbors(Grid2(x, y), [this, foundGroup](const Grid2& g) {
									if (_groups[g.X()][g.Y()] != -1 && _groups[g.X()][g.Y()] != foundGroup) {
										++_liberties[_groups[g.X()][g.Y()]];
									}
								});
							}
						}
					}
					_liberties[foundGroup] = -1;
				}
			}
			_history.emplace_back(position, color);
			ComputeScore();
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
		return _score;
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

	std::vector<std::vector<Color>> Board::GetPreviousLayout(int numTurns) const {
		if (numTurns <= 0 || numTurns >= _lastMoves.size()) {
			return _tiles;
		} else {
			return _lastMoves[_lastMoves.size() - numTurns - 1];
		}
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

	void Board::ComputeScore() {
		std::vector<std::vector<Color>> workingBoard(_tiles);
		std::vector<std::vector<bool>> seenTiles(GetDimensions().X(), std::vector<bool>(GetDimensions().Y(), false));
		int blackTiles = 0;
		int whiteTiles = 0;
		for (int x = 0; x < GetDimensions().X(); ++x) {
			for (int y = 0; y < GetDimensions().Y(); ++y) {
				if (!seenTiles[x][y]) {
					seenTiles[x][y] = true;
					if (workingBoard[x][y] == Color::None) {
						std::queue<Grid2> workingQueue;
						std::vector<Grid2> seenSpaces;
						workingQueue.push(Grid2(x, y));
						seenSpaces.emplace_back(x, y);
						bool seenBlack = false;
						bool seenWhite = false;
						while (!workingQueue.empty()) {
							Grid2 currentSpace = workingQueue.front();
							workingQueue.pop();
							Neighbors(currentSpace, [&workingBoard, &workingQueue, &seenSpaces, &seenTiles, &seenWhite, &seenBlack](const Grid2& g) {
								if (workingBoard[g.X()][g.Y()] == Color::None && !seenTiles[g.X()][g.Y()]) {
									workingQueue.push(g);
									seenTiles[g.X()][g.Y()] = true;
									seenSpaces.push_back(g);
								} else if (workingBoard[g.X()][g.Y()] == Color::White) {
									seenWhite = true;
								} else if (workingBoard[g.X()][g.Y()] == Color::Black) {
									seenBlack = true;
								}
							});
						}
						if (seenBlack && !seenWhite) {
							for (const Grid2& g : seenSpaces) {
								workingBoard[g.X()][g.Y()] = Color::Black;
								++blackTiles;
							}
						} else if (seenWhite && !seenBlack) {
							for (const Grid2& g : seenSpaces) {
								workingBoard[g.X()][g.Y()] = Color::White;
								++whiteTiles;
							}
						}
					} else if (workingBoard[x][y] == Color::Black) {
						++blackTiles;
					} else if (workingBoard[x][y] == Color::White) {
						++whiteTiles;
					}
				}
			}
		}
		_score = whiteTiles - blackTiles + GetKomi();
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