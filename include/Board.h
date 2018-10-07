#pragma once

#include <functional>
#include <vector>

#include "Constants.h"
#include "Grid2.h"
#include "MoveHistoryEntry.h"

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
		 * Returns whether placing a piece at the given position with the given color would be
		 * a suicide play or not.
		 * @param position
		 * @param color
		 * @return
		 */
		bool IsMoveSuicidePlay(const Grid2& position, Color color) const;

		/**
		 * Returns whether the given position is within the grounds of the board.
		 * @param position
		 * @return
		 */
		bool IsWithinBoard(const Grid2& position) const;

		/**
		 * Returns the moves that a specific color can place on this turn.
		 * @param color
		 * @return
		 */
		std::vector<Grid2> GetValidMoves(const Color& color) const;

		/**
		 * Gets the current komi for the white player.
		 * @return
		 */
		double GetKomi() const;

		/**
		 * Sets the current komi for the white player for this board.
		 * @param komi
		 * @return
		 */
		void SetKomi(double komi);

		/**
		 * Gets the current score for this board. Positive is in favor of white, negative is in favor of black.
		 * @return
		 */
		double Score() const;

		/**
		 * Gets the current score using the area method. Positive is in favor of white, negative is in favor of black.
		 * @return
		 */
		double ScoreArea() const;

		/**
			* Returns how many liberties a tile has, or -1 if the tile is None or off the board.
			* @param position
			* @return
			*/
		int GetLiberties(const Grid2& position) const;

		/**
		 * Returns all the tiles that are of the same color and are adjacent to this tile, or
		 * nothing if this doesn't point to a tile.
		 * @param position
		 * @return
		 */
		std::vector<Grid2> GetGroup(const Grid2& position) const;

		/**
		 * Returns how many liberties a group has.
		 * @param group
		 * @return
		 */
		int GetGroupLiberties(const std::vector<Grid2>& group) const;

		/**
		 * Tells the board that the turn is over.
		 */
		void NextTurn();

		/**
		 * Moves the board back the set number of plays.
		 */
		void RewindBoard(int numTurns);

		/**
		 * Returns a board that has been rewinded a certain number of moves.
		 * This is identical to copying the board and rewinding it.
		 */
		Board GetPreviousState(int numTurns) const;

		/**
		 * Returns a board layout that has been rewinded a certain number of moves.
		 * Unlike the other function, this function doesn't copy any other details, which is useful
		 * for creating screens for the history. This is in [x][y] format.
		 */
		std::vector<std::vector<Color>> GetPreviousLayout(int numTurns) const;

		/**
		 * Returns the history of all the moves played by the game.
		 */
		std::vector<MoveHistoryEntry> GetHistory() const;

		/**
		 * Returns the number of pieces the black player has captured.
		 */
		int GetBlackPiecesTaken() const;

		/**
		 * Returns the number of pieces the white player has captured.
		 */
		int GetWhitePiecesTaken() const;


		private:
		/**
		 * After placing a move, this clears any possible tiles that can be removed.
		 */
		void ClearPossibleTiles(const Grid2& position);

		/**
		 * Gets the direct neighbors to this Grid2 and runs a function on each one.
		 * Automatically culls positions that are outside of the board.
		 */
		inline void Neighbors(const Grid2& g, std::function<void(const Grid2&)> f) const;

		Grid2 _dimensions;
		std::vector<std::vector<Color>> _tiles;
		std::vector<std::vector<int>> _groups;
		std::vector<int> _liberties;
		std::vector<MoveHistoryEntry> _history;
		std::vector<std::vector<std::vector<Color>>> _lastMoves;

		Color _whoseTurn;
		int _blackPiecesTaken;
		int _whitePiecesTaken;
		int _turnCount;
		double _komi;
	};
}


