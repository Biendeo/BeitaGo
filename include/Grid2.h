#pragma once

namespace BeitaGo {
	/**
	 * Stores a integerial XY position, and can be operated on.
	 */
	class Grid2 {
		public:
		/**
		 * Grid2s are using ints as their underlying unit, and thus all calculations and returns
		 * should be with this unit.
		 */
		using unit = int;

		/**
		 * Constructs a Grid2 with (0, 0) parameters.
		 */
		Grid2();

		/**
		 * Constructs a Grid2 with (v, v) parameters.
		 * @param v
		 */
		Grid2(unit v);

		/**
		 * Constructs a Grid2 with (x, y) parameters.
		 * @param x
		 * @param y
		 */
		Grid2(unit x, unit y);

		/**
		 * Returns a copy of this Grid2 with (-x, -y) parameters).
		 * @return
		 */
		Grid2 operator-() const;

		/**
		 * Adds o's values piece-wise to this Grid2.
		 * @param o
		 * @return
		 */
		Grid2& operator+=(const Grid2& o);

		/**
		 * Subtracts o's values piece-wise from this Grid2.
		 * @param o
		 * @return
		 */
		Grid2& operator-=(const Grid2& o);

		/**
		 * Adds o's values piece-wise to a new Grid2.
		 * @param o
		 * @return
		 */
		Grid2 operator+(const Grid2& o) const;

		/**
		 * Subtracts o's values piece-wise to a new Grid2.
		 * @param o
		 * @return
		 */
		Grid2 operator-(const Grid2& o) const;

		/**
		 * Multiplies this Grid2 by a scalar value.
		 * @param a
		 * @return
		 */
		Grid2& operator*=(unit a);

		/**
		 * Divides this Grid2 by a scalar value (this can experience integer rounding errors).
		 * @param a
		 * @return
		 */
		Grid2& operator/=(unit a);

		/**
		 * Multiplies a new Grid2 by a scalar value.
		 * @param a
		 * @return
		 */
		Grid2 operator*(unit a) const;

		/**
		 * Divides a new Grid2 by a scalar value (this can experience integer rounding errors).
		 * @param a
		 * @return
		 */
		Grid2 operator/(unit a) const;

		/**
		 * Checks whether two Grid2s are equal.
		 * @param o
		 * @return
		 */
		bool operator==(const Grid2& o) const;

		/**
		 * Checks whether two Grid2s are not equal.
		 * @param o
		 * @return
		 */
		bool operator!=(const Grid2& o) const;

		/**
		 * Returns the value of the X field.
		 * @return
		 */
		unit X() const;

		/**
		 * Returns the value of the Y field.
		 * @return
		 */
		unit Y() const;

		/**
		 * Returns a modifiable reference to the X field.
		 * @return
		 */
		unit& X();

		/**
		 * Returns a modifiable reference to the Y field.
		 * @return
		 */
		unit& Y();

		/**
		 * Sets the X field and returns a modifiable reference to it.
		 * @param x
		 * @return
		 */
		unit& X(unit x);

		/**
		 * Sets the Y field and returns a modifiable reference to it.
		 * @param y
		 * @return
		 */
		unit& Y(unit y);

		/**
		 * Returns the Manhatten distance (x + y) of this Grid2.
		 * @return
		 */
		unit ManhattenDistance() const;

		/**
		 * Returns the magnitude (sqrt(x * x + y * y)) of this Grid2.
		 * @return
		 */
		float Magnitude() const;

		private:
		unit _x;
		unit _y;
	};
}