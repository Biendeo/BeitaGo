#pragma once

#include "Board.h"
#include "Player.h"

namespace BeitaGo {
	/**
	 * The main engine class of BeitaGo.
	 */
	class Engine {
		public:
		/**
		 * Constructs the engine. The board and players will start as null, so you must initialise
		 * them with a new game.
		 */
		Engine();
		~Engine();

		/**
		 * Starts a new game with a 19x19 board size, a human player as black, and a dumb AI as
		 * white.
		 */
		void NewGame();

		/**
		 * Returns the current board element.
		 * @return
		 */
		Board& GetBoard();

		/**
		 * Returns the current board element.
		 * @return
		 */
		const Board& GetBoard() const;

		/**
		 * Returns the current player.
		 * @return
		 */
		Player& GetCurrentPlayer();

		/**
		 * Returns the current player.
		 * @return
		 */
		const Player& GetCurrentPlayer() const;

		/**
		 * Returns player one.
		 * @return
		 */
		Player& GetPlayer1();

		/**
		 * Returns player one.
		 * @return
		 */
		const Player& GetPlayer1() const;

		/**
		 * Returns player two.
		 * @return
		 */
		Player& GetPlayer2();

		/**
		 * Returns player two.
		 * @return
		 */
		const Player& GetPlayer2() const;

		private:
		Board* _board;
		Player* _player1;
		Player* _player2;
	};
}

