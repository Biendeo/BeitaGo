#pragma once

#include <array>
#include <chrono>

#include "AIPlayer.h"

namespace BeitaGo {
	/**
	 * Defines an AI player that uses the Monte Carlo Tree Search to compute its decisions.
	 */
	class MonteCarloAIPlayer : public AIPlayer {
		public:
		MonteCarloAIPlayer(Engine& engine, Color color);
		MonteCarloAIPlayer(Engine& engine, Color color, const std::chrono::duration<double>& thinkingTime);
		~MonteCarloAIPlayer();

		/**
		 * Returns the most ideal space for this AI to pick.
		 * @return
		 */
		virtual Grid2 MakeDecision() const override;

		/**
		 * Sets the amount of time this AI can take to make its move.
		 */
		void SetThinkingTime(const std::chrono::duration<double>& thinkingTime);

		protected:
		std::chrono::duration<double> _thinkingTime;
	};
}

