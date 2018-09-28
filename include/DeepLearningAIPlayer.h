#include "MonteCarloAIPlayer.h"

namespace BeitaGo {
	class DeepLearningAIPlayer : protected MonteCarloAIPlayer {
		public:
		DeepLearningAIPlayer(Engine& engine, Color color);
		DeepLearningAIPlayer(Engine& engine, Color color, const std::chrono::duration<double>& thinkingTime);
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
		static constexpr int EXPECTED_BOARD_SIZE = 19;

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

		/**
		 * Converts the current board state into an input vector for the network.
		 * @return
		 */
		std::array<bool, INPUT_VECTOR_SIZE> BoardToInputVector() const;
	};
}