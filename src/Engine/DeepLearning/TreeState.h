#pragma once

#include <array>
#include <mutex>

#include "Board.h"
#include "DeepLearningAIPlayer.h"
#include "Grid2.h"

namespace BeitaGo {
	class TreeState {
		public:
		TreeState(const Board& currentBoard, TreeState* parent = nullptr, const Grid2& lastMove = PASS);
		~TreeState();

		/**
		 * Runs an iteration of the Monte Carlo Tree Search.
		 */
		void RunSimulation();

		/**
		 * Returns the depth of this node.
		 * @return
		 */
		int GetDepth() const;

		/**
		 * Runs n iterations of the Monte Carlo Tree Search spread among a given number of threads.
		 * @param n
		 * @param maxThreads
		 */
		void RunSimulations(int n, int maxThreads = std::thread::hardware_concurrency());

		/**
		 * Returns the move that was computed to have the best likelihood of victory.
		 * @return
		 */
		Grid2 GetMostLikelyMove() const;

		private:
		Board currentBoard;
		TreeState* parent;
		Grid2 lastMove;
		std::array<TreeState*, DeepLearningAIPlayer::EXPECTED_BOARD_SIZE * DeepLearningAIPlayer::EXPECTED_BOARD_SIZE + 1> children;
		int totalWins;
		int totalSimulations;
		std::mutex lock;
		int depth; // This can be easily computed, but I've cached it in the constructor.

		constexpr int PassIndex() const;
		void UpdateScore(bool win);
		void ComputeDepth();
	};
}