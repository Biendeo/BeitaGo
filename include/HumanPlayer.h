#pragma once

#include "Player.h"

namespace BeitaGo {
	/**
	 * A player that is specifically controlled by a human.
	 *
	 * Currently there's no difference between a generic player and this, but it lets UIs detect
	 * whether a player is human or not so it can handle making a decision itself.
	 */
	class HumanPlayer : public Player {
		public:
		HumanPlayer(Engine& engine, Color color);

		private:

	};
}


