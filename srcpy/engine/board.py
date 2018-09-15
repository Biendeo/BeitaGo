import copy
import queue
from .constants import Color
from .grid2 import Grid2


class Board:
	def __init__(self, dimensions: Grid2):
		self._dimensions = dimensions
		self._tiles = [[Color.NONE for x in range(dimensions.x())] for y in range(dimensions.y())]
		self._whoseTurn = Color.BLACK
		self._turnCount = 1
		self._komi = 6.5

	def get_dimensions(self) -> Grid2:
		return copy.copy(self._dimensions)

	def get_tile(self, position: Grid2) -> Color:
		return copy.copy(self._tiles[position.x()][position.y()])

	def get_whose_turn(self) -> Color:
		return copy.copy(self._whoseTurn)

	def get_turn_count(self) -> int:
		return self._turnCount

	def is_game_over(self) -> bool:
		return False

	def get_komi(self) -> float:
		return self._komi

	def set_komi(self, komi: float) -> None:
		self._komi = komi

	def place_piece(self, position: Grid2, color: Color) -> None:
		if self.is_move_valid(position, color):
			self._tiles[position.x()][position.y()] = color
			self.__clear_possible_tiles__(position + Grid2(1, 0))
			self.__clear_possible_tiles__(position + Grid2(0, 1))
			self.__clear_possible_tiles__(position + Grid2(-1, 0))
			self.__clear_possible_tiles__(position + Grid2(0, 1))
			self.__clear_possible_tiles__(position)
		else:
			raise RuntimeError("Board tried to place a {} piece at {} but it is invalid. Your AI or UI should check is_move_valid() is true before calling place_piece() or act_decision()!".format("BLACK" if color == Color.BLACK else "WHITE", position))
			pass

	def is_move_valid(self, position: Grid2, color: Color) -> bool:
		if self.is_within_board(position) and self.get_tile(position) == Color.NONE:
			return True
		else:
			return False

	def is_within_board(self, position: Grid2) -> bool:
		return 0 <= position.x() < self.get_dimensions().x() and 0 <= position.y() < self.get_dimensions().y()

	def get_liberties(self, position: Grid2) -> int:
		if not self.is_within_board(position) or self.get_tile(position) == Color.NONE:
			return -1
		else:
			liberties = 0
			if self.is_within_board(position + Grid2(1, 0)) and self.get_tile(position + Grid2(1, 0)) == Color.NONE:
				liberties += 1
			if self.is_within_board(position + Grid2(0, 1)) and self.get_tile(position + Grid2(0, 1)) == Color.NONE:
				liberties += 1
			if self.is_within_board(position + Grid2(-1, 0)) and self.get_tile(position + Grid2(-1, 0)) == Color.NONE:
				liberties += 1
			if self.is_within_board(position + Grid2(0, -1)) and self.get_tile(position + Grid2(0, -1)) == Color.NONE:
				liberties += 1
			return liberties

	def get_group(self, position: Grid2) -> [Grid2]:
		if self.is_within_board(position) and self.get_tile(position) != Color.NONE:
			color = self.get_tile(position)
			checked_spaces = set()
			connected_spaces = set()
			spaces_to_check = queue.Queue()

			spaces_to_check.put(position)
			checked_spaces.add(position)

			while not spaces_to_check.empty():
				current_space = spaces_to_check.get()
				if self.is_within_board(current_space) and self.get_tile(current_space) == color:
					connected_spaces.add(current_space)
					if (current_space + Grid2(1, 0)) not in checked_spaces:
						spaces_to_check.put(current_space + Grid2(1, 0))
						checked_spaces.add(current_space + Grid2(1, 0))
					if (current_space + Grid2(0, 1)) not in checked_spaces:
						spaces_to_check.put(current_space + Grid2(0, -1))
						checked_spaces.add(current_space + Grid2(0, -1))
					if (current_space + Grid2(-1, 0)) not in checked_spaces:
						spaces_to_check.put(current_space + Grid2(-1, 0))
						checked_spaces.add(current_space + Grid2(-1, 0))
					if (current_space + Grid2(0, -1)) not in checked_spaces:
						spaces_to_check.put(current_space + Grid2(0, -1))
						checked_spaces.add(current_space + Grid2(0, -1))

			return list(connected_spaces)
		else:
			return []

	def get_group_liberties(self, group: [Grid2]) -> int:
		if len(group) == 0:
			return 0
		color = self.get_tile(group[0])
		checked_spaces = set()
		connected_liberties = set()

		for g in group:
			neighbouringSpaces = list()
			neighbouringSpaces.append(g + Grid2(1, 0))
			neighbouringSpaces.append(g + Grid2(0, 1))
			neighbouringSpaces.append(g + Grid2(-1, 0))
			neighbouringSpaces.append(g + Grid2(0, -1))

			for neighbour in neighbouringSpaces:
				if self.is_within_board(neighbour) and self.get_tile(neighbour) == Color.NONE:
					connected_liberties.add(neighbour)

		return len(connected_liberties)

	def next_turn(self) -> None:
		self._turnCount += 1
		self._whoseTurn = Color.WHITE if self._whoseTurn == Color.BLACK else Color.BLACK

	def get_valid_moves(self, color: Color) -> [Grid2]:
		v = list()
		for y in range(self.get_dimensions().y()):
			for x in range(self.get_dimensions().x()):
				if self.is_move_valid(Grid2(x, y), color):
					v.append(Grid2(x, y))

		return v

	def score(self) -> float:
		return self.score_area()

	def score_area(self) -> float:
		white_tiles = 0
		black_tiles = 0
		for y in range(self.get_dimensions().y()):
			for x in range(self.get_dimensions().x()):
				if self.get_tile(Grid2(x, y)) == Color.WHITE:
					white_tiles += 1
				elif self.get_tile(Grid2(x, y)) == Color.BLACK:
					black_tiles += 1

		return white_tiles - black_tiles + self.get_komi()

	def __clear_possible_tiles__(self, position: Grid2) -> None:
		if self.is_within_board(position):
			group = self.get_group(position)
			if self.get_group_liberties(group) == 0:
				for space in group:
					self._tiles[space.x()][space.y()] = Color.NONE

