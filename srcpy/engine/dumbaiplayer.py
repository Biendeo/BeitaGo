import random
from .aiplayer import AIPlayer
from .constants import Color, PASS
from .grid2 import Grid2


class DumbAIPlayer(AIPlayer):
	from .engine import Engine

	def __init__(self, engine: Engine, color: Color):
		super().__init__(engine, color)

	def make_decision(self) -> Grid2:
		valid_moves = self.get_valid_moves()
		if len(valid_moves) == 0:
			return PASS
		else:
			return valid_moves[random.randint(0, len(valid_moves) - 1)]
