#include "DeepLearningAIPlayer.h"

#include <array>
#include <chrono>
#include <exception>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <thread>


#include "Board.h"
#include "DeepLearning/TreeState.h"
#include "Engine.h"

namespace BeitaGo {
	DeepLearningAIPlayer::DeepLearningAIPlayer(BeitaGo::Engine& engine, BeitaGo::Color color) : AIPlayer(engine, color) {

	}

	DeepLearningAIPlayer::~DeepLearningAIPlayer() {

	}

	Grid2 DeepLearningAIPlayer::MakeDecision() const {
		TreeState start(GetEngine().GetBoard());
		//TODO: This value can be tweaked, probably played around time.
		start.RunSimulations(200);
		return start.GetMostLikelyMove();
	}

	std::array<bool, DeepLearningAIPlayer::INPUT_VECTOR_SIZE> DeepLearningAIPlayer::BoardToInputVector() const {
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