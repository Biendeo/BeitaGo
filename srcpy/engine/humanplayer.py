from .constants import Color
from .engine import Engine
from .player import Player


class HumanPlayer(Player):
	def __init__(self, engine: Engine, color: Color):
		super().__init__(engine, color)
