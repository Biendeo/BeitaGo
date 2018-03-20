#pragma once

#include "Player.h"

namespace BeitaGo {
	class AIPlayer : public Player {
		public:
		AIPlayer(Engine& engine, Color color);
		virtual ~AIPlayer() = default;

		virtual Grid2 MakeDecision() const = 0;

		private:

	};
}

