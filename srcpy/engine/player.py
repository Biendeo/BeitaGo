import copy
from abc import ABC, abstractmethod
from .constants import Color, PASS
from .grid2 import Grid2


class Player(ABC):
	from .engine import Engine

	@abstractmethod
	def __init__(self, engine: Engine, color: Color):
		self._engine = engine
		self._color = color

	def get_engine(self) -> Engine:
		return self._engine

	def get_color(self) -> Color:
		return copy.copy(self._color)

	def act_decision(self, move: Grid2) -> None:
		if move is not PASS:
			self.get_engine().get_board().place_piece(move, self.get_color())
		self.get_engine().get_board().next_turn()

	def get_valid_moves(self) -> [Grid2]:
		v = list()
		for y in range(self.get_engine().get_board().get_dimensions().y()):
			for x in range(self.get_engine().get_board().get_dimensions().x()):
				if self.get_engine().get_board().is_move_valid(Grid2(x, y), self.get_color()):
					v.append(Grid2(x, y))

		return v
