from .board import Board
from .grid2 import Grid2
# from .player import Player

class Engine:
	def __init__(self):
		self._board = None
		self._player1 = None
		self._player2 = None
	
	def new_game(self, board_dimensions: Grid2, player1: "Player", player2: "Player"):
		self._board = Board(board_dimensions)
		self._player1 = player1
		self._player2 = player2

	def get_board(self) -> Board:
		return self._board

	def get_current_player(self) -> "Player":
		return self._player1 if self.get_board().get_whose_turn() == self._player1.get_color() else self._player2

	def get_player1(self) -> "Player":
		return self._player1

	def get_player2(self) -> "Player":
		return self._player2
