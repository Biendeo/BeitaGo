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
		std::array<bool, INPUT_VECTOR_SIZE> inputVector;
		int index = 0;
		for (int i = 0; i < 2 * HISTORY_USED; ++i) {
			for (int y = 0; y < GetEngine().GetBoard().GetDimensions().Y(); ++y) {
				for (int x = 0; x < GetEngine().GetBoard().GetDimensions().X(); ++x) {
					if (i % 2 == 0) {
						if (i == 0) {
							inputVector[index] = GetEngine().GetBoard().GetTile(Grid2(x, y)) == Color::Black;
						} else {
							Board historyBoard = GetEngine().GetBoard().GetPreviousState(i / 2);
							inputVector[index] = historyBoard.GetTile(Grid2(x, y)) == Color::Black;
						}
					} else {
						if (i == 0) {
							inputVector[index] = GetEngine().GetBoard().GetTile(Grid2(x, y)) == Color::White;
						} else {
							Board historyBoard = GetEngine().GetBoard().GetPreviousState(i / 2);
							inputVector[index] = historyBoard.GetTile(Grid2(x, y)) == Color::White;
						}
					}
					++index;
				}
			}
		}
		for (int y = 0; y < GetEngine().GetBoard().GetDimensions().Y(); ++y) {
			for (int x = 0; x < GetEngine().GetBoard().GetDimensions().X(); ++x) {
				inputVector[index] = GetEngine().GetBoard().GetWhoseTurn() == Color::Black;
				++index;
			}
		}
		return inputVector;
	}
}