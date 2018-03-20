#include "Grid2.h"

#include <cmath>

namespace BeitaGo {
	Grid2::Grid2() : Grid2(0, 0) {}

	Grid2::Grid2(Grid2::unit v) : Grid2(v, v) {}

	Grid2::Grid2(Grid2::unit x, Grid2::unit y) : _x(x), _y(y) {}

	Grid2 Grid2::operator-() const {
		return Grid2(-X(), -Y());
	}

	Grid2& Grid2::operator+=(const Grid2& o) {
		X() += o.X();
		Y() += o.Y();
		return *this;
	}

	Grid2& Grid2::operator-=(const Grid2& o) {
		return (*this += (-o));
	}

	Grid2 Grid2::operator+(const Grid2& o) const {
		return Grid2(*this) += o;
	}

	Grid2 Grid2::operator-(const Grid2& o) const {
		return Grid2(*this) -= o;
	}

	Grid2& Grid2::operator*=(Grid2::unit a) {
		X() *= a;
		Y() *= a;
		return *this;
	}

	Grid2& Grid2::operator/=(Grid2::unit a) {
		X() /= a;
		Y() /= a;
		return *this;
	}

	Grid2 Grid2::operator*(Grid2::unit a) const {
		return Grid2(*this) *= a;
	}

	Grid2 Grid2::operator/(Grid2::unit a) const {
		return Grid2(*this) /= a;
	}

	bool Grid2::operator==(const Grid2& o) const {
		return X() == o.X() && Y() == o.Y();
	}

	bool Grid2::operator!=(const Grid2& o) const {
		return !(*this == o);
	}

	Grid2::unit Grid2::X() const {
		return _x;
	}

	Grid2::unit Grid2::Y() const {
		return _y;
	}

	Grid2::unit& Grid2::X() {
		return _x;
	}

	Grid2::unit& Grid2::Y() {
		return _y;
	}

	Grid2::unit& Grid2::X(Grid2::unit x) {
		return _x = x;
	}

	Grid2::unit& Grid2::Y(Grid2::unit y) {
		return _y = y;
	}

	Grid2::unit Grid2::ManhattenDistance() const {
		return (X() < 0 ? -X() : X()) + (Y() < 0 ? -Y() : Y());
	}

	float Grid2::Magnitude() const {
		return std::sqrt(static_cast<float>(X() * X() + Y() * Y()));
	}
}