#include "MonteCarloAIPlayer.h"

#include "Board.h"
#include "DeepLearning/TreeState.h"
#include "Engine.h"


#include <iostream>

namespace BeitaGo {
	MonteCarloAIPlayer::MonteCarloAIPlayer(BeitaGo::Engine& engine, BeitaGo::Color color) : AIPlayer(engine, color) {
		rootState = nullptr;
		currentState = nullptr;
	}

	MonteCarloAIPlayer::~MonteCarloAIPlayer() {
		if (rootState != nullptr) {
			delete rootState;
		}
	}

	Grid2 MonteCarloAIPlayer::MakeDecision() const {
		if (rootState == nullptr) {
			rootState = new TreeState(Board(GetEngine().GetBoard().GetDimensions()));
			currentState = rootState;
		}
		currentState = rootState->FindCurrentState(GetEngine().GetBoard().GetHistory());
		//TODO: This value can be tweaked, probably played around time.
		currentState->RunSimulations(1000);
		return currentState->GetMostLikelyMove();
	}

	std::array<bool, MonteCarloAIPlayer::INPUT_VECTOR_SIZE> MonteCarloAIPlayer::BoardToInputVector() const {
		std::array<bool, INPUT_VECTOR_SIZE> inputVector;
		int index = 0;
		for (int i = 0; i < HISTORY_USED + 1; ++i) {
			for (int y = 0; y < GetEngine().GetBoard().GetDimensions().Y(); ++y) {
				for (int x = 0; x < GetEngine().GetBoard().GetDimensions().X(); ++x) {
					if (i == HISTORY_USED) {
						inputVector[index] = GetEngine().GetBoard().GetWhoseTurn() == Color::Black;
					} else if (i % 2 == 0) {
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
		return inputVector;
	}
}