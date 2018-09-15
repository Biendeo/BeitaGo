from .aiplayer import AIPlayer
from .board import Board
from .constants import Color, PASS
from .grid2 import Grid2

import copy
import multiprocessing
import random
import threading


class TreeState:
	def __init__(self, current_board: Board, parent=None, last_move=PASS):
		self.current_board = copy.deepcopy(current_board)
		self.parent = parent
		self.last_move = last_move
		self.children = [None] * (current_board.get_dimensions().x() * current_board.get_dimensions().y() + 1)
		self.total_wins = 0
		self.total_simulations = 0
		self.lock = threading.Lock()

	def run_simulation(self) -> None:
		# Decide an option.

		# I'm just going to get all available moves and randomly pick one to explore for 400 moves.
		valid_moves = self.current_board.get_valid_moves(self.current_board.get_whose_turn())
		r = random.randint(0, len(valid_moves) - 1)

		move_index = (self.current_board.get_dimensions().x() * self.current_board.get_dimensions().y()) + 1
		if valid_moves[r] != PASS:
			move_index = (self.current_board.get_dimensions().x() * valid_moves[r].x()) + valid_moves[r].y()

		self.lock.acquire()
		if not self.children[move_index]:
			self.children[move_index] = TreeState(self.current_board, self)
			self.children[move_index].current_board.place_piece(valid_moves[r], self.current_board.get_whose_turn())
			self.children[move_index].last_move = valid_moves[r]
			self.children[move_index].current_board.next_turn()
		self.lock.release()

		# Randomly play this match:
		playing_board = copy.deepcopy(self.children[move_index].current_board)
		for x in range(100):
			if playing_board.is_game_over():
				break
			moves = playing_board.get_valid_moves(playing_board.get_whose_turn())
			r = random.randint(0, len(moves) - 1)
			playing_board.place_piece(moves[r], playing_board.get_whose_turn())
			playing_board.next_turn()

		self.lock.acquire()
		self.children[move_index].update_score((playing_board.score() > 0 and self.children[move_index].current_board.get_whose_turn()) == Color.WHITE or playing_board.score() < 0 and self.children[move_index].current_board.get_whose_turn() == Color.BLACK)
		print("{} / {} ({}%)".format(self.total_wins, self.total_simulations, self.total_wins / self.total_simulations * 100))
		self.lock.release()

	def run_simulations(self, n: int, max_threads=multiprocessing.cpu_count()) -> None:
		if max_threads == 1:
			for x in range(n):
				self.run_simulation()
		else:
			# This gets limited apparently and only runs on one core.
			threads = []
			for x in range(max_threads):
				threads.append(threading.Thread(target=self.run_simulations, args=(n // 8, 1)))
				threads[-1].start()
			for x in range(max_threads):
				threads[x].join()

	def update_score(self, win: bool) -> None:
		self.total_simulations += 1
		if win:
			self.total_wins += 1
		if self.parent:
			self.parent.update_score(not win)

	def get_most_likely_move(self) -> Grid2:
		best_move = {"best_score": 0, "move": PASS}
		for child in self.children:
			if child is not None and child.total_simulations > 0:
				score = child.total_wins / child.total_simulations + child.total_simulations / 10
				if score > best_move["best_score"]:
					best_move["best_score"] = score
					best_move["move"] = child.last_move
		return best_move["move"]


class DeepLearningAIPlayer(AIPlayer):
	from .engine import Engine

	def __init__(self, engine: Engine, color: Color):
		super().__init__(engine, color)

	def make_decision(self) -> Grid2:
		starting_node = TreeState(self.get_engine().get_board())
		starting_node.run_simulations(500)
		return starting_node.get_most_likely_move()

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