#include "Player.h"

#include "Engine.h"

namespace BeitaGo {
	const Grid2 Player::PASS = Grid2(-6723, -2356);

	Player::Player(Engine& engine, Color color) : _engine(engine), _color(color) {}

	Engine& Player::GetEngine() {
		return _engine;
	}

	Color Player::GetColor() {
		return _color;
	}

	void Player::ActDecision(const Grid2& move) {
		//TODO: The UI should check that the move is valid, otherwise the board will throw on this.
		if (move != PASS) {
			GetEngine().GetBoard().PlacePiece(move, GetColor());
		}
		GetEngine().GetBoard().NextTurn();
	}
}