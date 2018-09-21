#pragma once

#include <array>

#include "AIPlayer.h"


namespace BeitaGo {
	/**
	 * Defines an AI player that uses deep learning to compute its decisions.
	 */
	class DeepLearningAIPlayer : public AIPlayer {
		public:
		DeepLearningAIPlayer(Engine& engine, Color color);
		~DeepLearningAIPlayer();

		/**
		 * Returns the most ideal space for this AI to pick.
		 * @return
		 */
		Grid2 MakeDecision() const override;

		//TODO: This could probably be dynamic at runtime to have different networks for different sizes.
		/**
		 * The deep learning AI can only expect a board of a specific size to use with its network.
		 */
		static constexpr int EXPECTED_BOARD_SIZE = 9;

		/**
		 * The number of previous turns used to handle the decision making.
		 */
		static constexpr int HISTORY_USED = 1;

		/**
		 * The network works by representing the board as a flattened 3D array of bools, two dimensions being the map
		 * dimensions and the third being a number of layers. A number of previous turns are used, and there are two
		 * layers used per turn to represent all the white tiles and black tiles. An extra layer is necessary to
		 * indicate whose turn it currently is.
		 */
		static constexpr int INPUT_VECTOR_SIZE = EXPECTED_BOARD_SIZE * EXPECTED_BOARD_SIZE * (2 * HISTORY_USED) + 1; //! Obviously needs history.

		/**
		 * Converts the current board state into an input vector for the network.
		 * @return
		 */
		std::array<bool, INPUT_VECTOR_SIZE> BoardToInputVector() const;

		private:
	};
}

