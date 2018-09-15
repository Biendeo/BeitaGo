from .aiplayer import AIPlayer
from .board import Board
from .constants import Color, PASS
from .grid2 import Grid2

class DeepLearningAIPlayer(AIPlayer):
	from .engine import Engine

	def __init__(self, engine: Engine, color: Color):
		super().__init__(engine, color)

	def make_decision(self) -> Grid2:
		return PASS

	def board_to_input_vector(self, board: Board) -> [bool]:
		# Not complete, implement a history.
		r = list()
		for y in range(self.get_engine().get_board().get_dimensions().y()):
			for x in range(self.get_engine().get_board().get_dimensions().x()):
				r.append(self.get_engine().get_board().get_tile(Grid2(x, y)) == Color.BLACK)
		for y in range(self.get_engine().get_board().get_dimensions().y()):
			for x in range(self.get_engine().get_board().get_dimensions().x()):
				r.append(self.get_engine().get_board().get_tile(Grid2(x, y)) == Color.WHITE)
		for y in range(self.get_engine().get_board().get_dimensions().y()):
			for x in range(self.get_engine().get_board().get_dimensions().x()):
				r.append(self.get_engine().get_current_player())
		return r