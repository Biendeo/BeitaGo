#pragma once

#include "Constants.h"
#include "Grid2.h"

namespace BeitaGo {
	class Engine;

	class Player {
		public:
		Player(Engine& engine, Color color);
		virtual ~Player() = default;

		Engine& GetEngine();
		Color GetColor();

		void ActDecision(const Grid2& move);

		// It doesn't matter what these values are, this is just so you can indicate a value for
		// passing.
		static const Grid2 PASS;

		private:
		Engine& _engine;
		Color _color;
	};

}
