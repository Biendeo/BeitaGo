#pragma once

namespace BeitaGo {
	class Grid2 {
		public:
		using unit = int;

		Grid2();
		Grid2(unit v);
		Grid2(unit x, unit y);

		//TODO: Operators.
		Grid2 operator-() const;

		Grid2& operator+=(const Grid2& o);
		Grid2& operator-=(const Grid2& o);

		Grid2 operator+(const Grid2& o) const;
		Grid2 operator-(const Grid2& o) const;

		Grid2& operator*=(unit a);
		Grid2& operator/=(unit a);

		Grid2 operator*(unit a) const;
		Grid2 operator/(unit a) const;

		bool operator==(const Grid2& o) const;
		bool operator!=(const Grid2& o) const;

		unit X() const;
		unit Y() const;

		unit& X();
		unit& Y();

		unit& X(unit x);
		unit& Y(unit y);

		unit ManhattenDistance() const;
		float Magnitude() const;

		private:
		unit _x;
		unit _y;
	};
}