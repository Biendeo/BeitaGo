#include "MonteCarloAIPlayer.h"

#include "Board.h"
#include "DeepLearning/MonteCarloTree.h"
#include "Engine.h"

#include <iostream>

namespace BeitaGo {
	MonteCarloAIPlayer::MonteCarloAIPlayer(BeitaGo::Engine& engine, BeitaGo::Color color) : MonteCarloAIPlayer(engine, color, DEFAULT_THINKING_TIME) {}

	MonteCarloAIPlayer::MonteCarloAIPlayer(BeitaGo::Engine& engine, BeitaGo::Color color, const std::chrono::duration<double>& thinkingTime) : AIPlayer(engine, color), _thinkingTime(thinkingTime) {}

	MonteCarloAIPlayer::~MonteCarloAIPlayer() {}

	Grid2 MonteCarloAIPlayer::MakeDecision() const {
		MonteCarloTree tree(GetEngine().GetBoard());
		std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now() + std::chrono::duration_cast<std::chrono::seconds>(_thinkingTime);
		tree.InitializeNodes(5);
		tree.RunSimulations(endTime);
		return tree.GetMostLikelyMove();
	}

	std::array<bool, MonteCarloAIPlayer::INPUT_VECTOR_SIZE> MonteCarloAIPlayer::BoardToInputVector() const {
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

	void MonteCarloAIPlayer::SetThinkingTime(const std::chrono::duration<double>& thinkingTime) {
		_thinkingTime = thinkingTime;
	}
}