#include "TreeState.h"

#include <iostream>
#include <random>
#include <string>

namespace BeitaGo {
	TreeState::TreeState(const Board& currentBoard, TreeState* parent, const Grid2& lastMove) : currentBoard(currentBoard), parent(parent), lastMove(lastMove) {
		if (currentBoard.GetDimensions().X() == currentBoard.GetDimensions().Y() && currentBoard.GetDimensions().X() == DeepLearningAIPlayer::EXPECTED_BOARD_SIZE) {
			totalWins = 0;
			totalSimulations = 0;
			for (int i = 0; i < children.size(); ++i) {
				children[i] = nullptr;
			}
			ComputeDepth();
		} else {
			throw std::invalid_argument("DeepLearningAIPlayer given a board of an unsupported size " + std::to_string(currentBoard.GetDimensions().X()));
		}
	}
	TreeState::~TreeState() {
		for (TreeState* state : children) {
			if (state != nullptr) {
				delete state;
			}
		}
	}

	void TreeState::RunSimulation() {
		auto validMoves = currentBoard.GetValidMoves(currentBoard.GetWhoseTurn());

		std::default_random_engine randomEngine;
		randomEngine.seed(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
		std::uniform_int_distribution<int> validMoveDistribution(0, static_cast<int>(validMoves.size()) - 1);
		int validMoveIndex = validMoveDistribution(randomEngine);

		int childrenIndex = PassIndex();
		if (validMoves[validMoveIndex] != PASS) {
			childrenIndex = validMoves[validMoveIndex].Y() * currentBoard.GetDimensions().X() + validMoves[validMoveIndex].X();
		}

		lock.lock();
		if (children[childrenIndex] == nullptr) {
			children[childrenIndex] = new TreeState(currentBoard, this, validMoves[validMoveIndex]);
			children[childrenIndex]->currentBoard.PlacePiece(validMoves[validMoveIndex], currentBoard.GetWhoseTurn());
			children[childrenIndex]->currentBoard.NextTurn();
		}
		lock.unlock();

		// Randomly play this board for a while.
		Board b = children[childrenIndex]->currentBoard;
		for (int i = 0; i < 100; ++i) {
			try {
				if (b.IsGameOver()) {
					break;
				}
				auto simValidMoves = b.GetValidMoves(b.GetWhoseTurn());
				std::uniform_int_distribution<int> simValidMoveDistribution(0, static_cast<int>(simValidMoves.size()) - 1);
				int simValidMoveIndex = simValidMoveDistribution(randomEngine);
				b.PlacePiece(simValidMoves[simValidMoveIndex], b.GetWhoseTurn());
				b.NextTurn();
			} catch (std::exception& e) {
				std::cout << e.what() << "\n";
			}
		}

		lock.lock();
		double score = b.Score();
		Color whoseTurn = children[childrenIndex]->currentBoard.GetWhoseTurn();
		children[childrenIndex]->UpdateScore((score > 0.0 && whoseTurn == Color::White) || (score < 0.0 && whoseTurn == Color::Black));
		//std::cout << totalWins << " / " << totalSimulations << "(" << totalWins / static_cast<double>(totalSimulations) * 100.0 << "%)\n";
		lock.unlock();
	}
	int TreeState::GetDepth() const {
		return depth;
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
		for (TreeState* state : children) {
			if (state != nullptr && state->totalSimulations > 0) {
				double score = state->totalWins / static_cast<double>(state->totalSimulations) + state->totalSimulations / 10.0;
				if (score > bestMove.first) {
					bestMove.first = score;
					bestMove.second = state->lastMove;
				}
			}
		}
		return bestMove.second;
	}

	constexpr int TreeState::PassIndex() const {
		return DeepLearningAIPlayer::EXPECTED_BOARD_SIZE * DeepLearningAIPlayer::EXPECTED_BOARD_SIZE;
	}

	void TreeState::UpdateScore(bool win) {
		++totalSimulations;
		if (win) {
			++totalWins;
		}
		if (parent != nullptr) {
			parent->UpdateScore(!win);
		}
	}

	void TreeState::ComputeDepth() {
		if (parent == nullptr) {
			depth = 0;
		} else {
			depth = parent->GetDepth() + 1;
		}
	}
}