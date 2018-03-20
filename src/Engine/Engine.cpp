#include "Engine.h"

#include "HumanPlayer.h"
#include "DumbAIPlayer.h"

namespace BeitaGo {
	Engine::Engine() {
		_board = nullptr;
		_player1 = nullptr;
		_player2 = nullptr;
	}

	Engine::~Engine() {
		//TODO: If C++ is nice, does delete just give a nullptr?
		delete _board;
		_board = nullptr;
		delete _player1;
		_player1 = nullptr;
		delete _player2;
	}

	void Engine::NewGame() {
		//TODO: Make these customisable.
		_board = new Board(Grid2(19, 19));
		_player1 = new HumanPlayer(*this, Color::Black);
		_player2 = new DumbAIPlayer(*this, Color::White);
	}

	Board& Engine::GetBoard() {
		return *_board;
	}

	const Board& Engine::GetBoard() const {
		return *_board;
	}

	Player& Engine::GetCurrentPlayer() {
		if (GetPlayer1().GetColor() == GetBoard().GetWhoseTurn()) {
			return GetPlayer1();
		} else {
			return GetPlayer2();
		}
	}

	const Player& Engine::GetCurrentPlayer() const {
		if (GetPlayer1().GetColor() == GetBoard().GetWhoseTurn()) {
			return GetPlayer1();
		} else {
			return GetPlayer2();
		}
	}

	Player& Engine::GetPlayer1() {
		return *_player1;
	}

	const Player& Engine::GetPlayer1() const {
		return *_player1;
	}

	Player& Engine::GetPlayer2() {
		return *_player2;
	}

	const Player& Engine::GetPlayer2() const {
		return *_player2;
	}
}