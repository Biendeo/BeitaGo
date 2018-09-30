#pragma once

#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

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
		virtual void RunSimulation();

		/**
		 * To set up the nodes, we run a fixed number of initial simulations on each one.
		 * @param maxThreads
		 */
		virtual void InitializeNodes(int n, int maxThreads = std::thread::hardware_concurrency());

		/**
		 * Runs n iterations of the Monte Carlo Tree Search spread among a given number of threads.
		 * @param n
		 * @param maxThreads
		 */
		virtual void RunSimulations(int n, int maxThreads = std::thread::hardware_concurrency());

		/**
		 * Runs as many iterations of the Monte Carlo Tree Search until the time is reached.
		 * @param n
		 * @param maxThreads
		 */
		virtual void RunSimulations(const std::chrono::high_resolution_clock::time_point& endTime, int maxThreads = std::thread::hardware_concurrency());

		/**
		 * Returns the move that was computed to have the best likelihood of victory.
		 * @return
		 */
		virtual Grid2 GetMostLikelyMove() const;

		protected:
		Board _board;
		std::vector<MonteCarloNode> _children;
		int _totalWins;
		int _totalSimulations;
		std::mutex _lock;
		const std::vector<Grid2> _validMoves; // This is just for caching purposes.

		int PassIndex() const;
		int Grid2ToIndex(const Grid2& g) const;
		Grid2 IndexToGrid2(int index) const;
	};
}