#pragma once

#include <vector>

#include "Constants.h"
#include "Grid2.h"

namespace BeitaGo {
	/**
	 * Stores the board state (and in turn the overall game state).
	 */
	class Board {
		public:
		/**
		 * Constructs a new board with the given dimensions.
		 * @param dimensions
		 */
		Board(const Grid2& dimensions);

		/**
		 * Returns this board's dimensions.
		 * @return
		 */
		Grid2 GetDimensions() const;

		/**
		 * Returns the tile placed at a given position. If the position is out of the board's range,
		 * it just returns None.
		 * @param position
		 * @return
		 */
		Color GetTile(const Grid2& position) const;

		/**
		 * Returns the Color of whoever's turn it is.
		 * @return
		 */
		Color GetWhoseTurn() const;

		/**
		 * Returns the current turn count. Games start at turn 1, and increment turn after each
		 * piece (so the player who goes first will play on odd-numbered turns, etc.).
		 * @return
		 */
		int GetTurnCount() const;

		/**
		 * Returns whether the game is currently over.
		 * @return
		 */
		bool IsGameOver() const;

		/**
		 * Places a piece at the given space with the given color. This throws if the move would be
		 * invalid. When you act a decision, run it by IsMoveValid first.
		 * @param position
		 * @param color
		 */
		void PlacePiece(const Grid2& position, Color color);

		/**
		 * Returns whether placing a piece at the given position with the given color would be
		 * valid or not.
		 * @param position
		 * @param color
		 * @return
		 */
		bool IsMoveValid(const Grid2& position, Color color) const;

		/**
		 * Tells the board that the turn is over.
		 */
		void NextTurn();

		private:
		Grid2 _dimensions;
		std::vector<std::vector<Color>> _tiles;

		Color _whoseTurn;
		int _turnCount;
	};
}


