#pragma once

#include "Player.h"

namespace BeitaGo {
	/**
	 * Defines an interface that AI players should follow.
	 */
	class AIPlayer : public Player {
		public:
		AIPlayer(Engine& engine, Color color);
		virtual ~AIPlayer() = default;

		/**
		 * Returns the move that this AI will do given the current board state.
		 *
		 * This should be implemented depending on the type of AI you want to use. This returns the
		 * chosen action, but doesn't act it out so the UI can detect the decision.
		 * @return
		 */
		virtual Grid2 MakeDecision() const = 0;

		private:

	};
}

