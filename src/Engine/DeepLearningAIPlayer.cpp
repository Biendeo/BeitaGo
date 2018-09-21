#include "DeepLearningAIPlayer.h"

#include <array>
#include <chrono>
#include <exception>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <thread>


#include "Board.h"
#include "Engine.h"

namespace BeitaGo {
	class TreeState {
		public:
		TreeState(const Board& currentBoard, TreeState* parent = nullptr, const Grid2& lastMove = PASS) : currentBoard(currentBoard), parent(parent), lastMove(lastMove) {
			if (currentBoard.GetDimensions().X() == currentBoard.GetDimensions().Y() && currentBoard.GetDimensions().X() == DeepLearningAIPlayer::EXPECTED_BOARD_SIZE) {
				totalWins = 0;
				totalSimulations = 0;
				for (int i = 0; i < children.size(); ++i) {
					children[i] = nullptr;
				}
			} else {
				throw std::invalid_argument("DeepLearningAIPlayer given a board of an unsupported size " + std::to_string(currentBoard.GetDimensions().X()));
			}
		}

		~TreeState() {
			for (TreeState* state : children) {
				if (state != nullptr) {
					delete state;
				}
			}
		}

		/**
		 * Runs an iteration of the Monte Carlo Tree Search.
		 */
		void RunSimulation() {
			auto validMoves = currentBoard.GetValidMoves(currentBoard.GetWhoseTurn());

			std::default_random_engine randomEngine;
			randomEngine.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
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
			for (int i = 0; i < 200; ++i) {
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
			/*
			for (int y = b.GetDimensions().Y() - 1; y >= 0; --y) {
				std::cout << " ";
				for (int x = 0; x < b.GetDimensions().X(); ++x) {
					Color color = b.GetTile(BeitaGo::Grid2(x, y));
					if (color == Color::Black) {
						std::cout << "X";
					} else if (color == Color::White) {
						std::cout << "O";
					} else {
						std::cout << ".";
					}
				}
				std::cout << "\n";
			}
			*/
			double score = b.Score();
			Color whoseTurn = children[childrenIndex]->currentBoard.GetWhoseTurn();
			children[childrenIndex]->UpdateScore((score > 0.0 && whoseTurn == Color::White) || (score < 0.0 && whoseTurn == Color::Black));
			//std::cout << totalWins << " / " << totalSimulations << "(" << totalWins / static_cast<double>(totalSimulations) * 100.0 << "%)\n";
			lock.unlock();
		}

		/**
		 * Runs n iterations of the Monte Carlo Tree Search spread among a given number of threads.
		 * @param n
		 * @param maxThreads
		 */
		void RunSimulations(int n, int maxThreads = std::thread::hardware_concurrency()) {
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

		/**
		 * Returns the move that was computed to have the best likelihood of victory.
		 * @return
		 */
		Grid2 GetMostLikelyMove() const {
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

		private:
		Board currentBoard;
		TreeState* parent;
		Grid2 lastMove;
		std::array<TreeState*, DeepLearningAIPlayer::EXPECTED_BOARD_SIZE * DeepLearningAIPlayer::EXPECTED_BOARD_SIZE + 1> children;
		int totalWins;
		int totalSimulations;
		std::mutex lock;


		/**
		 * Returns the expected
		 * @return
		 */
		constexpr int PassIndex() const {
			return DeepLearningAIPlayer::EXPECTED_BOARD_SIZE * DeepLearningAIPlayer::EXPECTED_BOARD_SIZE;
		}

		void UpdateScore(bool win) {
			++totalSimulations;
			if (win) {
				++totalWins;
			}
			if (parent != nullptr) {
				parent->UpdateScore(!win);
			}
		}
	};

	DeepLearningAIPlayer::DeepLearningAIPlayer(BeitaGo::Engine& engine, BeitaGo::Color color) : AIPlayer(engine, color) {

	}

	DeepLearningAIPlayer::~DeepLearningAIPlayer() {

	}

	Grid2 DeepLearningAIPlayer::MakeDecision() const {
		TreeState start(GetEngine().GetBoard());
		//TODO: This value can be tweaked, probably played around time.
		start.RunSimulations(1000);
		return start.GetMostLikelyMove();
	}

	std::array<bool, DeepLearningAIPlayer::INPUT_VECTOR_SIZE> DeepLearningAIPlayer::BoardToInputVector() const {
		std::array<bool, INPUT_VECTOR_SIZE> inputVector;
		int index = 0;
		for (int i = 0; i < HISTORY_USED + 1; ++i) {
			for (int y = 0; y < GetEngine().GetBoard().GetDimensions().Y(); ++y) {
				for (int x = 0; x < GetEngine().GetBoard().GetDimensions().X(); ++x) {
					switch (i) {
						case 0:
							inputVector[index] = GetEngine().GetBoard().GetTile(Grid2(x, y)) == Color::Black;
							break;
						case 1:
							inputVector[index] = GetEngine().GetBoard().GetTile(Grid2(x, y)) == Color::White;
							break;
						case 2:
							inputVector[index] = GetEngine().GetBoard().GetWhoseTurn() == Color::Black;
							break;
						default:
							// I haven't handled history yet!
							break;
					}
					++index;
				}
			}
		}
		return inputVector;
	}
}