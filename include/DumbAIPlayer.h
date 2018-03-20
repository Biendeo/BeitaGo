#pragma once

#include "AIPlayer.h"

namespace BeitaGo {
	class DumbAIPlayer : public AIPlayer {
		public:
		DumbAIPlayer(Engine& engine, Color color);

		Grid2 MakeDecision() const override;

		private:

	};
}

