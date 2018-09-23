#pragma once

#include <array>
#include <mutex>
#include <vector>

#include "Board.h"
#include "MonteCarloAIPlayer.h"
#include "Grid2.h"

namespace BeitaGo {
	class TreeState {
		// Purely for debugging properties.
		friend class MonteCarloAIPlayer;

		public:
		TreeState(const Board& _currentBoard, TreeState* parent = nullptr, const Grid2& lastMove = PASS);

		/**
		 * Destroys this node and every child node from it. To delete the whole tree, delete the
		 * parent node.
		 */
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

		/**
		 * Navigates the tree to find the node that corresponds to the given history. If the node
		 * or any nodes along the path don't exist, they are created. If this is not the root node,
		 * it will navigate to the root first.
		 */
		TreeState* FindCurrentState(const std::vector<MoveHistoryEntry>& history);

		private:
		Board _currentBoard;
		TreeState* _parent;
		Grid2 _lastMove;
		std::array<TreeState*, MonteCarloAIPlayer::EXPECTED_BOARD_SIZE * MonteCarloAIPlayer::EXPECTED_BOARD_SIZE + 1> _children;
		int _totalWins;
		int _totalSimulations;
		std::mutex _lock;
		int _depth; // This can be easily computed, but I've cached it in the constructor.
		int _childrenExpanded;
		std::vector<Grid2> _validMoves;

		constexpr int PassIndex() const;
		int Grid2ToIndex(const Grid2& g) const;
		Grid2 IndexToGrid2(int index) const;
		int GetChildrenExpanded() const;
		bool IsFullyExpanded() const;
		void UpdateScore(bool win);
		void ComputeDepth();
		TreeState* CheckAndCreateChild(const Grid2& g);
		TreeState* CheckAndCreateChild(int index);
	};
}