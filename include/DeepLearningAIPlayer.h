#pragma once

#include "MonteCarloAIPlayer.h"

#include <array>

#include <dlib/dnn.h>
#include <dlib/data_io.h>

#include "Board.h"

namespace BeitaGo {
	class DeepLearningAIPlayer : public MonteCarloAIPlayer {
		public:
		//TODO: This could probably be dynamic at runtime to have different networks for different sizes.
		/**
		 * The deep learning AI can only expect a board of a specific size to use with its network.
		 */
		static constexpr int EXPECTED_BOARD_SIZE = 9;

		/**
		 * The number of previous turns used to handle the decision making.
		 */
		static constexpr int HISTORY_USED = 4;

		/**
		 * The network works by representing the board as a flattened 3D array of bools, two dimensions being the map
		 * dimensions and the third being a number of layers. A number of previous turns are used, and there are two
		 * layers used per turn to represent all the white tiles and black tiles. An extra layer is necessary to
		 * indicate whose turn it currently is.
		 */
		static constexpr int INPUT_VECTOR_SIZE = EXPECTED_BOARD_SIZE * EXPECTED_BOARD_SIZE * (2 * HISTORY_USED + 1);

		/**
		 * The network represents the output as just a vector of all the moves that can be done.
		 */
		static constexpr int OUTPUT_VECTOR_SIZE = EXPECTED_BOARD_SIZE * EXPECTED_BOARD_SIZE + 1;
		using NetworkType = dlib::loss_multiclass_log<dlib::fc<OUTPUT_VECTOR_SIZE, dlib::relu<dlib::fc<100, dlib::relu<dlib::input<dlib::matrix<unsigned char>>>>>>>;


		/**
		 * This value is used by the heuristic.
		 */
		static constexpr double c = 1.500;

		DeepLearningAIPlayer(Engine& engine, Color color);
		DeepLearningAIPlayer(Engine& engine, Color color, const std::chrono::duration<double>& thinkingTime);
		DeepLearningAIPlayer(Engine& engine, Color color, const std::chrono::duration<double>& thinkingTime, const std::string& networkFilePath);
		DeepLearningAIPlayer(Engine& engine, Color color, const std::chrono::duration<double>& thinkingTime, NetworkType& network);
		~DeepLearningAIPlayer();

		/**
		 * Returns the most ideal space for this AI to pick.
		 * @return
		 */
		Grid2 MakeDecision() const override;

		/**
		 * Converts the current board state into an input vector for the network.
		 * @return
		 */
		std::array<bool, INPUT_VECTOR_SIZE> BoardToInputVector() const;

		/**
		 * Converts a given board state into an input vector for the network.
		 * @return
		 */
		static std::array<bool, INPUT_VECTOR_SIZE> BoardToInputVector(const Board& board);

		/**
		 * Converts the current board state into a dlib matrix.
		 * @return
		 */
		dlib::matrix<unsigned char, INPUT_VECTOR_SIZE, 1> BoardToDlibMatrix() const;

		/**
		 * Converts a given board state into a dlib matrix.
		 * @return
		 */
		static dlib::matrix<unsigned char, INPUT_VECTOR_SIZE, 1> BoardToDlibMatrix(const Board& board);

		/**
		 * Returns all the heuristic values computed by the last run of the MakeDecision() call.
		 */
		std::array<double, OUTPUT_VECTOR_SIZE> GetAllHeuristicValues() const;

		/**
		 * Returns the total number of simulations run by the last run of tthe MakeDecision() call.
		 */
		int GetTotalSimulations() const;

		private:
		NetworkType _network;
		mutable std::array<double, OUTPUT_VECTOR_SIZE> _heuristicValues;
		mutable int _totalSimulations;
	};
}