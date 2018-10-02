#include "Player.h"

#include "Engine.h"

namespace BeitaGo {
	Player::Player(Engine& engine, Color color) : _engine(engine), _color(color) {}

	Engine& Player::GetEngine() {
		return _engine;
	}

	const Engine& Player::GetEngine() const {
		return _engine;
	}

	Color Player::GetColor() const {
		return _color;
	}

	void Player::ActDecision(const Grid2& move) {
		GetEngine().GetBoard().PlacePiece(move, GetColor());
		GetEngine().GetBoard().NextTurn();
	}

	std::vector<Grid2> Player::GetValidMoves() const {
		return GetEngine().GetBoard().GetValidMoves(GetColor());
	}
}