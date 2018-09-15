import math
from typing import TypeVar

T = TypeVar("T")


class Grid2:
	def __init__(self, x: int, y: int):
		self._x = x
		self._y = y

	def __str__(self) -> str:
		return "({}, {})".format(self._x,self. _y)

	def __neg__(self) -> TypeVar:
		return Grid2(-self._x, -self._y)
	
	def __iadd__(self, other: T) -> T:
		self._x += other.x()
		self._y += other.y()
		return self

	def __isub__(self, other: T) -> T:
		self._x -= other.x()
		self._y -= other.y()
		return self

	def __add__(self, other: T) -> T:
		return Grid2(self._x + other.x(), self._y + other.y())

	def __sub__(self, other: T) -> T:
		return Grid2(self._x - other.x(), self._y - other.y())

	def __imul__(self, other: T) -> T:
		self._x *= other
		self._y *= other
		return self

	def __ifloordiv__(self, other: T) -> T:
		self._x //= other
		self._y //= other
		return self

	def __mul__(self, other: T) -> T:
		return Grid2(self._x * other.x(), self._x * other.y())

	def __floordiv__(self, other: T) -> T:
		return Grid2(self._x // other.x(), self._y // other.y())

	def __eq__(self, other: T) -> bool:
		return self._x == other.x() and self._y == other.y()

	def __ne__(self, other: T) -> bool:
		return not self == other

	def x(self) -> int:
		return self._x

	def y(self) -> int:
		return self._y

	def manhatten_distance(self) -> int:
		return abs(self._x) + abs(self._y)

	def magnitude(self) -> float:
		return math.sqrt(self._x * self._x + self._y * self._y)

	def __iter__(self):
		return self

	def __hash__(self):
		return self._x * 92346902346 + self._y