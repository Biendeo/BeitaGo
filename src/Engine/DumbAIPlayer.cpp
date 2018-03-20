#include "DumbAIPlayer.h"

namespace BeitaGo {
	DumbAIPlayer::DumbAIPlayer(Engine& engine, Color color) : AIPlayer(engine, color) {}

	Grid2 DumbAIPlayer::MakeDecision() const {
		//TODO: Pick an actual valid space, even if it's not a good space.
		return Grid2(0, 0);
	}
}