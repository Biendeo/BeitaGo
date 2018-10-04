#pragma once

#include "DeepLearningAIPlayer.h"
#include "MonteCarloTree.h"

#include <array>

#include "Board.h"

namespace BeitaGo {
	class NNMonteCarloTree : public MonteCarloTree {
		public:
		NNMonteCarloTree(const Board& board, DeepLearningAIPlayer::NetworkType network);

		/**
		 * Runs an iteration of the Monte Carlo Tree Search.
		 */
		virtual void RunSimulation() override;

		/**
		 * To set up the nodes, we run a fixed number of initial simulations on each one.
		 * @param maxThreads
		 */
		virtual void InitializeNodes(int n, int maxThreads = std::thread::hardware_concurrency()) override;

		/**
		 * Runs n iterations of the Monte Carlo Tree Search spread among a given number of threads.
		 * @param n
		 * @param maxThreads
		 */
		virtual void RunSimulations(int n, int maxThreads = std::thread::hardware_concurrency()) override;

		/**
		 * Runs as many iterations of the Monte Carlo Tree Search until the time is reached.
		 * @param n
		 * @param maxThreads
		 */
		virtual void RunSimulations(const std::chrono::high_resolution_clock::time_point& endTime, int maxThreads = std::thread::hardware_concurrency()) override;

		/**
		 * Returns the move that was computed to have the best likelihood of victory.
		 * @return
		 */
		virtual Grid2 GetMostLikelyMove() const override;

		/**
		 * Returns the value returned by the heuristic for the given move.
		 */
		double GetHeuristicValue(const Grid2& g, bool checkIfValid = true);

		/**
		 * Returns the value returned by the heuristic for every move.
		 */
		std::array<double, DeepLearningAIPlayer::OUTPUT_VECTOR_SIZE> GetAllHeuristicValues();

		/**
		 * Returns the value returned by the heuristic for every move, normalised so they all sum up
		 * to 1.
		 */
		std::array<double, DeepLearningAIPlayer::OUTPUT_VECTOR_SIZE> GetAllHeuristicValuesNormalised();

		/**
		 * Returns the total number of simulations this tree has run.
		 */
		int GetTotalSimulations() const;

		private:
		DeepLearningAIPlayer::NetworkType _network;
		std::array<double, DeepLearningAIPlayer::OUTPUT_VECTOR_SIZE> _networkValues;
	};
}