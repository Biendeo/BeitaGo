#include "TreeState.h"

#include <iostream>
#include <random>
#include <string>

namespace BeitaGo {
	TreeState::TreeState(const Board& _currentBoard, TreeState* parent, const Grid2& lastMove) : _currentBoard(_currentBoard), _parent(parent), _lastMove(lastMove) {
		if (_currentBoard.GetDimensions().X() == _currentBoard.GetDimensions().Y() && _currentBoard.GetDimensions().X() == DeepLearningAIPlayer::EXPECTED_BOARD_SIZE) {
			_totalWins = 0;
			_totalSimulations = 0;
			for (int i = 0; i < _children.size(); ++i) {
				_children[i] = nullptr;
			}
			ComputeDepth();
		} else {
			throw std::invalid_argument("DeepLearningAIPlayer given a board of an unsupported size " + std::to_string(_currentBoard.GetDimensions().X()));
		}
	}
	TreeState::~TreeState() {
		for (TreeState* state : _children) {
			if (state != nullptr) {
				delete state;
			}
		}
	}

	void TreeState::RunSimulation() {
		auto validMoves = _currentBoard.GetValidMoves(_currentBoard.GetWhoseTurn());

		std::default_random_engine randomEngine;
		randomEngine.seed(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
		std::uniform_int_distribution<int> validMoveDistribution(0, static_cast<int>(validMoves.size()) - 1);
		int validMoveIndex = validMoveDistribution(randomEngine);

		int childrenIndex = PassIndex();
		if (validMoves[validMoveIndex] != PASS) {
			childrenIndex = Grid2ToIndex(validMoves[validMoveIndex]);
		}

		_lock.lock();
		if (_children[childrenIndex] == nullptr) {
			_children[childrenIndex] = new TreeState(_currentBoard, this, validMoves[validMoveIndex]);
			_children[childrenIndex]->_currentBoard.PlacePiece(validMoves[validMoveIndex], _currentBoard.GetWhoseTurn());
			_children[childrenIndex]->_currentBoard.NextTurn();
		}
		_lock.unlock();

		// Randomly play this board for a while.
		Board b = _children[childrenIndex]->_currentBoard;
		for (int i = 0; i < 100; ++i) {
			if (b.IsGameOver()) {
				break;
			}
			auto simValidMoves = b.GetValidMoves(b.GetWhoseTurn());
			std::uniform_int_distribution<int> simValidMoveDistribution(0, static_cast<int>(simValidMoves.size()) - 1);
			int simValidMoveIndex = simValidMoveDistribution(randomEngine);
			b.PlacePiece(simValidMoves[simValidMoveIndex], b.GetWhoseTurn());
			b.NextTurn();
		}

		_lock.lock();
		double score = b.Score();
		Color whoseTurn = _children[childrenIndex]->_currentBoard.GetWhoseTurn();
		_children[childrenIndex]->UpdateScore((score > 0.0 && whoseTurn == Color::White) || (score < 0.0 && whoseTurn == Color::Black));
		//std::cout < _totalWins << " / " << _totalSimulations << "(" << _totalWins / static_cast<double>(_totalSimulations) * 100.0 << "%)\n";
		_lock.unlock();
	}
	int TreeState::GetDepth() const {
		return _depth;
	}

	void TreeState::RunSimulations(int n, int maxThreads) {
		if (maxThreads == 1) {
			for (int x = 0; x < n; ++x) {
				RunSimulation();
			}
		} else {
			std::vector<std::thread> threads;
			for (int i = 0; i < maxThreads; ++i) {
				//TODO: This will cut off some desired executions.
				threads.emplace_back(std::thread([&]() { this->RunSimulations(n / maxThreads, 1); }));
			}
			for (int i = 0; i < maxThreads; ++i) {
				threads[i].join();
			}
		}
	}

	Grid2 TreeState::GetMostLikelyMove() const {
		auto bestMove = std::make_pair(0.0, PASS);
		for (TreeState* state : _children) {
			if (state != nullptr && state->_totalSimulations > 0) {
				//TODO: Make this heuristic better.
				double score = state->_totalWins / static_cast<double>(state->_totalSimulations) + state->_totalSimulations / 10.0;
				if (score > bestMove.first) {
					bestMove.first = score;
					bestMove.second = state->_lastMove;
				}
			}
		}
		return bestMove.second;
	}

	TreeState* TreeState::FindCurrentState(const std::vector<MoveHistoryEntry>& history) {
		if (_parent != nullptr) {
			return _parent->FindCurrentState(history);
		} else {
			TreeState* currentState = this;
			for (auto it = history.begin(); it != history.end(); ++it) {
				int nextIndex = PassIndex();
				Grid2 nextMove = PASS;
				if (currentState->_currentBoard.GetWhoseTurn() == it->GetColor()) {
					nextIndex = Grid2ToIndex(it->GetPosition());
					nextMove = it->GetPosition();
				} else {
					--it;
				}
				if (currentState->_children[nextIndex] == nullptr) {
					Board nextBoard = currentState->_currentBoard;
					nextBoard.PlacePiece(nextMove, nextBoard.GetWhoseTurn());
					nextBoard.NextTurn();
					currentState->_children[nextIndex] = new TreeState(nextBoard, currentState, nextMove);
				}
				currentState = currentState->_children[nextIndex];
			}
			return currentState;
		}
	}

	constexpr int TreeState::PassIndex() const {
		return DeepLearningAIPlayer::EXPECTED_BOARD_SIZE * DeepLearningAIPlayer::EXPECTED_BOARD_SIZE;
	}

	int TreeState::Grid2ToIndex(const Grid2& g) const {
		if (g == PASS) {
			return PassIndex();
		} else {
			return g.Y() * _currentBoard.GetDimensions().X() + g.X();
		}
	}

	void TreeState::UpdateScore(bool win) {
		++_totalSimulations;
		if (win) {
			++_totalWins;
		}
		if (_parent != nullptr) {
			_parent->UpdateScore(!win);
		}
	}

	void TreeState::ComputeDepth() {
		if (_parent == nullptr) {
			_depth = 0;
		} else {
			_depth = _parent->GetDepth() + 1;
		}
	}
}