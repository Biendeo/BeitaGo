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

	void MonteCarloAIPlayer::SetThinkingTime(const std::chrono::duration<double>& thinkingTime) {
		_thinkingTime = thinkingTime;
	}
}