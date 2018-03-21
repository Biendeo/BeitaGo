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
		 * Starts a new game with the given board dimensions and player objects.
		 * @param boardDimensions
		 * @param player1
		 * @param player2
		 */
		void NewGame(const Grid2& boardDimensions, Player* player1, Player* player2);

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

