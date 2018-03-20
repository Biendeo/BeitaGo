#pragma once

#include <vector>

#include "Constants.h"
#include "Grid2.h"

namespace BeitaGo {
	class Engine;

	/**
	 * Defines functions that an individual player can perform.
	 */
	class Player {
		public:
		/**
		 * Constructs a new player object. The engine and color must be specified.
		 * @param engine
		 * @param color
		 */
		Player(Engine& engine, Color color);
		virtual ~Player() = default;

		/**
		 * Returns the attached engine.
		 * @return
		 */
		Engine& GetEngine();

		/**
		 * Returns the attached engine.
		 * @return
		 */
		const Engine& GetEngine() const;

		/**
		 * Returns the color of this Player.
		 * @return
		 */
		Color GetColor() const;

		/**
		 * Locks in a decision and performs it to the engine.
		 * @param move
		 */
		void ActDecision(const Grid2& move);

		/**
		 * Returns a vector containing all the positions of valid moves on this turn.
		 * @return
		 */
		std::vector<Grid2> GetValidMoves() const;

		private:
		Engine& _engine;
		Color _color;
	};

}
