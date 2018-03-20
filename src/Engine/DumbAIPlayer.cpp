#include "DumbAIPlayer.h"

#include <chrono>
#include <random>

namespace BeitaGo {
	DumbAIPlayer::DumbAIPlayer(Engine& engine, Color color) : AIPlayer(engine, color) {}

	Grid2 DumbAIPlayer::MakeDecision() const {
		auto validMoves = GetValidMoves();
		if (validMoves.empty()) {
			return PASS;
		} else {
			std::mt19937 rand(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
			std::uniform_int_distribution<int> distribution(0, validMoves.size() - 1);
			return validMoves[distribution(rand)];
		}
	}
}