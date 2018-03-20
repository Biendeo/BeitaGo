#pragma once

#include "AIPlayer.h"

namespace BeitaGo {
	/**
	 * Implements the AI with the dumbest AI that chooses a random valid space to place their piece.
	 */
	class DumbAIPlayer : public AIPlayer {
		public:
		DumbAIPlayer(Engine& engine, Color color);

		/**
		 * Returns a random space that this AI can place a piece.
		 * @return
		 */
		Grid2 MakeDecision() const override;

		private:

	};
}

