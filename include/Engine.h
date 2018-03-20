#pragma once

#include "Board.h"
#include "Player.h"

namespace BeitaGo {
	class Engine {
		public:
		Engine();
		~Engine();

		void NewGame();

		Board& GetBoard();
		Player& GetCurrentPlayer();
		Player& GetPlayer1();
		Player& GetPlayer2();

		private:
		Board* _board;
		Player* _player1;
		Player* _player2;
	};
}

