import copy
from enum import Enum
import sys
from typing import TypeVar
from engine.constants import Color
from engine.dumbaiplayer import DumbAIPlayer
from engine.engine import Engine
from engine.grid2 import Grid2


def column_to_letter(col: int) -> str:
	if 0 <= col <= 24:
		c = col + ord("A")
		if c >= ord("I"):
			c += 1
		return chr(c)
	else:
		raise RuntimeError("column_to_letter() was given an invalid column number")


def letter_to_column(letter: str) -> int:
	lower = letter.lower()
	if ord("a") <= ord(lower) <= ord("z") and lower != "i":
		col = ord(lower) - ord("a") - (1 if ord(lower) > ord("i") else 0)
		return col
	else:
		raise RuntimeError("letter_to_column() was given an invalid letter")


class CommandType(Enum):
	UNKNOWN = 0
	PROTOCOL_VERSION = 1
	NAME = 2
	VERSION = 3
	KNOWN_COMMAND = 4
	LIST_COMMANDS = 5
	QUIT = 6
	BOARD_SIZE = 7
	CLEAR_BOARD = 8
	KOMI = 9
	FIXED_HANDICAP = 10
	PLACE_FREE_HANDICAP = 11
	SET_FREE_HANDICAP = 12
	PLAY = 13
	GEN_MOVE = 14
	UNDO = 15
	TIME_SETTINGS = 16
	TIME_LEFT = 17
	FINAL_SCORE = 18
	FINAL_STATUS_LIST = 19
	LOAD_SGF = 20
	REG_GEN_MOVE = 21
	SHOW_BOARD = 22


def str_to_command_type(s: str) -> CommandType:
	if s == "protocol_version":
		return CommandType.PROTOCOL_VERSION
	elif s == "name":
		return CommandType.NAME
	elif s == "version":
		return CommandType.VERSION
	elif s == "known_command":
		return CommandType.KNOWN_COMMAND
	elif s == "list_commands":
		return CommandType.LIST_COMMANDS
	elif s == "quit":
		return CommandType.QUIT
	elif s == "boardsize":
		return CommandType.BOARD_SIZE
	elif s == "clear_board":
		return CommandType.CLEAR_BOARD
	elif s == "komi":
		return CommandType.KOMI
	elif s == "fixed_handicap":
		return CommandType.FIXED_HANDICAP
	elif s == "place_free_handicap":
		return CommandType.PLACE_FREE_HANDICAP
	elif s == "set_free_handicap":
		return CommandType.SET_FREE_HANDICAP
	elif s == "play":
		return CommandType.PLAY
	elif s == "genmove":
		return CommandType.GEN_MOVE
	elif s == "undo":
		return CommandType.UNDO
	elif s == "time_settings":
		return CommandType.TIME_SETTINGS
	elif s == "time_left":
		return CommandType.TIME_LEFT
	elif s == "final_score":
		return CommandType.FINAL_SCORE
	elif s == "final_status_list":
		return CommandType.FINAL_STATUS_LIST
	elif s == "loadsgf":
		return CommandType.LOAD_SGF
	elif s == "reg_genmove":
		return CommandType.REG_GEN_MOVE
	elif s == "showboard":
		return CommandType.SHOW_BOARD
	else:
		return CommandType.UNKNOWN


def tokenize(s: str) -> [str]:
	r = list()
	current_string = ""
	i = 0
	while i < len(s):
		c = s[i]
		if c == " ":
			if current_string != "":
				r.append(current_string)
				current_string = ""
		elif c == "\"":
			if current_string == "":
				i += 1
				while s[i] != "\"":
					current_string += s[i]
					i += 1
				if current_string != "":
					r.append(current_string)
					current_string = ""
		else:
			current_string += c
		i += 1
	if current_string != "":
		r.append(current_string)
	return r


GTPVertexT = TypeVar("GTPVertexT", str, Grid2)


class GTPVertex:
	def __init__(self, s: GTPVertexT):
		if isinstance(s, str):
			self._val = Grid2(letter_to_column(s[0]), int(s[1:]) - 1)
		elif isinstance(s, Grid2):
			self._val = s

	def to_grid2(self) -> Grid2:
		return Grid2(self._val.x(), self._val.y())

	def __str__(self) -> str:
		return column_to_letter(self._val.x()) + str(self._val.y() + 1)


GTPColorT = TypeVar("GTPColorT", str, Color)


class GTPColor:
	def __init__(self, s: GTPColorT):
		if isinstance(s, str):
			lower_case = s.lower()
			self._val = Color.WHITE if lower_case == "white" or lower_case == "w" else Color.BLACK
		elif isinstance(s, Color):
			self._val = s

	def to_color(self) -> Color:
		return copy.copy(self._val)

	def __str__(self) -> str:
		return "white" if self._val == Color.WHITE else "black"


class GTPMove:
	def __init__(self, s: str):
		self._color = GTPColor(s.split(" ")[0])
		self._vertex = GTPVertex(s.split(" ")[1])

	def get_color(self) -> GTPColor:
		return copy.copy(self._color)

	def get_vertex(self) -> GTPVertex:
		return copy.copy(self._vertex)

	def __str__(self) -> str:
		return str(self._color) + " " + str(self._vertex)


class GTPBoolean:
	def __init__(self, b: bool):
		self._val = b

	def __str__(self) -> str:
		return "true" if self._val else "false"

	def __bool__(self) -> bool:
		return self._val


class GTPEngine:
	def __init__(self):
		self._engine = Engine()
		self._exiting = False
		self.PROTOCOL_VERSION = 2
		self.NAME = "BeitaGo Dumb AI"
		self.VERSION = "0.0.2"
		self.MAX_SIZE = 25

		self._engine.new_game(Grid2(19, 19), DumbAIPlayer(self._engine, Color.BLACK), DumbAIPlayer(self._engine, Color.WHITE))

	def start(self) -> None:
		self._exiting = False
		self.main_loop()

	def print_splash_message(self) -> None:
		print("{}\n{}\n".format(self.NAME, self.VERSION), end="")

	def main_loop(self) -> None:
		while not self._exiting:
			self.handle_input(sys.stdin.readline())

	def handle_input(self, s: str) -> None:
		cleaned_input = self.preprocess_input(s)
		tokenized_input = tokenize(cleaned_input)

		if len(tokenized_input) >= 1:
			using_id = False
			id = -1
			try:
				id = int(tokenized_input[0])
				if id >= 0:
					using_id = True
				else:
					id = -1
			except:
				pass

			if len(tokenized_input) >= 2 if using_id else 1:
				type = str_to_command_type(tokenized_input[1 if using_id else 0])
				arguments = tokenized_input[(2 if using_id else 1):]
				if type == CommandType.PROTOCOL_VERSION:
					self.print_protocol_version(id, arguments)
				elif type == CommandType.NAME:
					self.print_name(id, arguments)
				elif type == CommandType.VERSION:
					self.print_version(id, arguments)
				elif type == CommandType.KNOWN_COMMAND:
					self.print_known_command(id, arguments)
				elif type == CommandType.LIST_COMMANDS:
					self.print_list_commands(id, arguments)
				elif type == CommandType.QUIT:
					self._exiting = True
				elif type == CommandType.BOARD_SIZE:
					self.board_size(id, arguments)
				elif type == CommandType.CLEAR_BOARD:
					self.clear_board(id, arguments)
				elif type == CommandType.KOMI:
					self.komi(id, arguments)
				elif type == CommandType.PLAY:
					self.play(id, arguments)
				elif type == CommandType.GEN_MOVE:
					self.gen_move(id, arguments)
				elif type == CommandType.SHOW_BOARD:
					self.show_board(id, arguments)
				else:
					self.print_failure_response(id, "Unknown command supplied")
			else:
				self.print_failure_response(id, "Unknown command supplied")
		else:
			self.print_failure_response(-1, "Unknown command supplied")

	def print_protocol_version(self, id: int, arguments: [str]) -> None:
		self.print_success_response(id, str(self.PROTOCOL_VERSION))

	def print_name(self, id: int, arguments: [str]) -> None:
		self.print_success_response(id, str(self.NAME))

	def print_version(self, id: int, arguments: [str]) -> None:
		self.print_success_response(id, str(self.VERSION))

	def print_known_command(self, id: int, arguments: [str]) -> None:
		if len(arguments) >= 1:
			type = str_to_command_type(arguments[0])
			if type != CommandType.UNKNOWN:
				self.print_success_response(id, str(GTPBoolean(True)))
			else:
				self.print_success_response(id, str(GTPBoolean(False)))
		else:
			self.print_failure_response(id, str(GTPBoolean(False)))

	def print_list_commands(self, id: int, arguments: [str]) -> None:
		s = ""
		s += "protocol_version" + "\n"
		s += "name" + "\n"
		s += "version" + "\n"
		s += "known_command" + "\n"
		s += "list_commands" + "\n"
		s += "quit" + "\n"
		s += "boardsize" + "\n"
		s += "clear_board" + "\n"
		s += "komi" + "\n"
		s += "play" + "\n"
		s += "genmove" + "\n"
		s += "showboard"
		self.print_success_response(id, s)

	def board_size(self, id: int, arguments: [str]) -> None:
		if len(arguments) >= 1:
			try:
				size = int(arguments[0])
				if size > self.MAX_SIZE:
					self.print_failure_response(id, "unacceptable size")
				else:
					self._engine.new_game(Grid2(size, size), DumbAIPlayer(self._engine, Color.BLACK), DumbAIPlayer(self._engine, Color.WHITE))
					self.print_success_response(id, "")
			except:
				self.print_failure_response(id, "syntax error")
		else:
			self.print_failure_response(id, "syntax error")

	def clear_board(self, id: int, arguments: [str]) -> None:
		self._engine.new_game(Grid2(self._engine.get_board().get_dimensions().x(), self._engine.get_board().get_dimensions().y()), DumbAIPlayer(self._engine, Color.BLACK), DumbAIPlayer(self._engine, Color.WHITE))
		self.print_success_response(id, "")

	def komi(self, id: int, arguments: [str]) -> None:
		if len(arguments) >= 1:
			try:
				komi = float(arguments[0])
				# Something...implement this in the engine!
				self.print_success_response(id, "")
			except:
				self.print_failure_response(id, "syntax error")
		else:
			self.print_failure_response(id, "syntax error")

	def play(self, id: int, arguments: [str]) -> None:
		if len(arguments) == 1:
			try:
				move = GTPMove(arguments[0])
				if self._engine.get_board().is_move_valid(move.get_vertex().to_grid2(), move.get_color().to_color()):
					self._engine.get_board().place_piece(move.get_vertex().to_grid2(), move.get_color().to_color())
				self.print_success_response(id, "")
			except:
				self.print_failure_response(id, "syntax error")
		elif len(arguments) >= 2:
			try:
				move = GTPMove(arguments[0] + " " + arguments[1])
				if self._engine.get_board().is_move_valid(move.get_vertex().to_grid2(), move.get_color().to_color()):
					self._engine.get_board().place_piece(move.get_vertex().to_grid2(), move.get_color().to_color())
				self.print_success_response(id, "")
			except:
				self.print_failure_response(id, "syntax error")
		else:
			self.print_failure_response(id, "syntax error")

	def gen_move(self, id: int, arguments: [str]) -> None:
		if len(arguments) >= 1:
			try:
				color = GTPColor(arguments[0])
				if color.to_color() == Color.BLACK:
					move = GTPVertex(self._engine.get_player1().make_decision())
					self._engine.get_player1().act_decision(move.to_grid2())
					self.print_success_response(id, str(move))
				else:
					move = GTPVertex(self._engine.get_player2().make_decision())
					self._engine.get_player2().act_decision(move.to_grid2())
					self.print_success_response(id, str(move))
			except:
				self.print_failure_response(id, "syntax error")
		else:
			self.print_failure_response(id, "syntax error")

	def show_board(self, id: int, arguments: [str]) -> None:
		print("=", end="\n")
		print("00\nBoard state:", end="\n")
		self.print_board()
		print(end="\n")

	def print_success_response(self, id: int, message: str) -> None:
		print("=", end="")
		if id != -1:
			print(id, end="")
		print(" " + message + "\n", end="\n")

	def print_failure_response(self, id: int, message: str) -> None:
		print("?", end="")
		if id != -1:
			print(id, end="")
		print(" " + message + "\n", end="\n")

	def preprocess_input(self, s: str) -> str:
		new_string = ""
		in_comment = False
		for c in s:
			if in_comment and c == "\n":
				in_comment = False
				new_string += c
			elif not in_comment and (ord(c) < ord(" ") or c == "\t" or c == "\n"):
				pass
			elif not in_comment and c == "\t":
				new_string += " "
			elif not in_comment and c == "#":
				in_comment = True
			elif not in_comment:
				new_string += c
		return new_string

	def print_board(self) -> None:
		for inverse_y in range(self._engine.get_board().get_dimensions().y()):
			y = self._engine.get_board().get_dimensions().y() - inverse_y - 1
			print(" ", end="")
			for x in range(self._engine.get_board().get_dimensions().x()):
				color = self._engine.get_board().get_tile(Grid2(x, y))
				if color == Color.BLACK:
					print("X", end="")
				elif color == Color.WHITE:
					print("O", end="")
				else:
					print(".", end="")
			print(end="\n")


if __name__ == "__main__":
	e = GTPEngine()
	e.start()
