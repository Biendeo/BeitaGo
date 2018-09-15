from abc import abstractmethod
from .constants import Color
from .grid2 import Grid2
from .player import Player


class AIPlayer(Player):
	from .engine import Engine

	def __init__(self, engine: Engine, color: Color):
		super().__init__(engine, color)

	@abstractmethod
	def make_decision(self) -> Grid2:
		pass
