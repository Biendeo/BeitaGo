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
		//TODO: The UI should check that the move is valid, otherwise the board will throw on this.
		if (move != PASS) {
			GetEngine().GetBoard().PlacePiece(move, GetColor());
		}
		GetEngine().GetBoard().NextTurn();
	}

	std::vector<Grid2> Player::GetValidMoves() const {
		//? While this may not scale that well, I think it'll be satisfactory for even the larger
		//? board sizes people will use.
		std::vector<Grid2> v;

		for (int y = 0; y < GetEngine().GetBoard().GetDimensions().Y(); ++y) {
			for (int x = 0; x < GetEngine().GetBoard().GetDimensions().X(); ++x) {
				if (GetEngine().GetBoard().IsMoveValid(Grid2(x, y), GetColor())) {
					v.emplace_back(x, y);
				}
			}
		}

		return v;
	}
}