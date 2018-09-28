#include "DeepLearningAIPlayer.h"

#include "Engine.h"

namespace BeitaGo {
	DeepLearningAIPlayer::DeepLearningAIPlayer(Engine& engine, Color color) : DeepLearningAIPlayer(engine, color, DEFAULT_THINKING_TIME) {}

	DeepLearningAIPlayer::DeepLearningAIPlayer(Engine& engine, Color color, const std::chrono::duration<double>& thinkingTime) : MonteCarloAIPlayer(engine, color, thinkingTime) {}

	DeepLearningAIPlayer::~DeepLearningAIPlayer() {}

	Grid2 DeepLearningAIPlayer::MakeDecision() const {
		return MonteCarloAIPlayer::MakeDecision();
	}
	
	std::array<bool, DeepLearningAIPlayer::INPUT_VECTOR_SIZE> DeepLearningAIPlayer::BoardToInputVector() const {
		return BoardToInputVector(GetEngine().GetBoard());
	}
	
	std::array<bool, DeepLearningAIPlayer::INPUT_VECTOR_SIZE> DeepLearningAIPlayer::BoardToInputVector(const Board& board) {
		std::array<bool, INPUT_VECTOR_SIZE> inputVector;
		int index = 0;
		for (int i = 0; i < 2 * HISTORY_USED; ++i) {
			for (int y = 0; y < board.GetDimensions().Y(); ++y) {
				for (int x = 0; x < board.GetDimensions().X(); ++x) {
					if (i % 2 == 0) {
						if (i == 0) {
							inputVector[index] = board.GetTile(Grid2(x, y)) == Color::Black;
						} else {
							Board historyBoard = board.GetPreviousState(i / 2);
							inputVector[index] = historyBoard.GetTile(Grid2(x, y)) == Color::Black;
						}
					} else {
						if (i == 0) {
							inputVector[index] = board.GetTile(Grid2(x, y)) == Color::White;
						} else {
							Board historyBoard = board.GetPreviousState(i / 2);
							inputVector[index] = historyBoard.GetTile(Grid2(x, y)) == Color::White;
						}
					}
					++index;
				}
			}
		}
		for (int y = 0; y < board.GetDimensions().Y(); ++y) {
			for (int x = 0; x < board.GetDimensions().X(); ++x) {
				inputVector[index] = board.GetWhoseTurn() == Color::Black;
				++index;
			}
		}
		return inputVector;
	}
}