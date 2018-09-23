#pragma once

#include <array>
#include <mutex>

#include "Board.h"
#include "Grid2.h"
#include "MonteCarloAIPlayer.h"
#include "MonteCarloNode.h"

namespace BeitaGo {

	/**
	 * Defines the head of the tree that computes a MCTS for the board.
	 */
	class MonteCarloTree {
		// Purely for debugging properties.
		friend class MonteCarloAIPlayer;

		public:
		MonteCarloTree(const Board& board);

		/**
		 * Runs an iteration of the Monte Carlo Tree Search.
		 */
		void RunSimulation();

		/**
		 * To set up the nodes, we run a fixed number of initial simulations on each one.
		 * @param maxThreads
		 */
		void InitializeNodes(int n, int maxThreads = std::thread::hardware_concurrency());

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
		Board _board;
		std::array<MonteCarloNode, MonteCarloAIPlayer::EXPECTED_BOARD_SIZE * MonteCarloAIPlayer::EXPECTED_BOARD_SIZE + 1> _children;
		int _totalWins;
		int _totalSimulations;
		std::mutex _lock;
		const std::vector<Grid2> _validMoves; // This is just for caching purposes.

		constexpr int PassIndex() const;
		int Grid2ToIndex(const Grid2& g) const;
		Grid2 IndexToGrid2(int index) const;
	};
}