#include "MonteCarloNode.h"

#include <random>

namespace BeitaGo {
	MonteCarloNode::MonteCarloNode() : _board(Grid2(1, 1)), _lastMove(PASS), _valid(false), _totalWins(0), _totalSimulations(0) {
	
	}

	bool MonteCarloNode::RunSimulation() {
		std::default_random_engine randomEngine;
		randomEngine.seed(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
		Board b = _board;
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

		// Since the current board is for the other player's turn, this equation looks swapped.
		double score = b.Score();
		bool win = score < 0.0 && _board.GetWhoseTurn() == Color::White || score > 0.0 && _board.GetWhoseTurn() == Color::Black;
		_lock.lock();
		if (win) {
			++_totalWins;
		}
		++_totalSimulations;
		_lock.unlock();
		return win;
	}

	void MonteCarloNode::SetBoard(const Board& board, const Grid2& lastMove) {
		_board = board;
		_lastMove = lastMove;
		_validMoves = board.GetValidMoves(board.GetWhoseTurn());
	}

	Grid2 MonteCarloNode::LastMove() const {
		return _lastMove;
	}

	bool MonteCarloNode::IsValid() const {
		return _valid;
	}

	void MonteCarloNode::SetValid(bool validity) {
		_valid = validity;
	}

	int MonteCarloNode::TotalWins() const {
		return _totalWins;
	}

	int MonteCarloNode::TotalSimulations() const {
		return _totalSimulations;
	}
}