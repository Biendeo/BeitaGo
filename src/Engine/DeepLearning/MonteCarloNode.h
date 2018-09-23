#pragma once

#include <mutex>

#include "Board.h"
#include "Grid2.h"

namespace BeitaGo {
	/**
	 * Defines a child node in the MCTS node.
	 */
	class MonteCarloNode {
		public:
		/**
		 * Constructs a node that is disable.
		 */
		MonteCarloNode();

		/**
		 * Runs a random game from this node. Returns whether it was a win or not.
		 */
		bool RunSimulation();

		/**
		 * Sets the current board of this node. For performance purposes, this should be done
		 * only for valid boards.
		 */
		void SetBoard(const Board& board, const Grid2& lastMove);

		/**
		 * Gets the move that created this node. If it is PASS, it may most likely be an invalid
		 * node.
		 */
		Grid2 LastMove() const;

		/**
		 * Returns whether this node was from a valid move or not.
		 */
		bool IsValid() const;

		/**
		 * Sets whether this node should be a valid node or not.
		 * @param validity.
		 */
		void SetValid(bool validity);

		/**
		 * Returns the number of winning simulations run.
		 */
		int TotalWins() const;

		/**
		 * Returns the number of simulations run.
		 */
		int TotalSimulations() const;

		private:
		bool _valid;
		Board _board;
		Grid2 _lastMove;
		int _totalWins;
		int _totalSimulations;
		std::mutex _lock;
		std::vector<Grid2> _validMoves; // This is just for caching purposes.
	};
}