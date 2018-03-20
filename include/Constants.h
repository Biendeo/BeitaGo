#pragma once

#include <cstdint>

#include "Grid2.h"

namespace BeitaGo {
	/**
	 * Differentiates between different piece colors.
	 */
	enum class Color : uint8_t {
		None,
		Black,
		White
	};

	/**
	* A useful term to indicate whether the player will pass their turn. Use this in
	* ActDecision.
	*/
	const Grid2 PASS = Grid2(-6723, -2356);
};