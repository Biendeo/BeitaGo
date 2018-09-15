import sys
from engine.board import Board
from engine.constants import Color, PASS
from engine.dumbaiplayer import DumbAIPlayer
from engine.engine import Engine
from engine.grid2 import Grid2
from engine.humanplayer import HumanPlayer


def main() -> None:
	print("This just is an ASCII version of the game to test it before using the UI.")

	e = Engine()
	human = HumanPlayer(e, Color.BLACK)
	ai = DumbAIPlayer(e, Color.WHITE)
	e.new_game(Grid2(9, 9), human, ai)

	print_board(e.get_board())
	while not e.get_board().is_game_over():
		human.act_decision(ask_for_human_move())
		print_board(e.get_board())
		ai.act_decision(ai.make_decision())
		print_board(e.get_board())


def print_board(board: Board) -> None:
	print("\n\n", end="")
	print("+", end="")
	for x in range(board.get_dimensions().x()):
		print("-", end="")
	print("+", end="\n")

	for y in range(board.get_dimensions().y()):
		print("|", end="")
		for x in range(board.get_dimensions().x()):
			if board.get_tile(Grid2(x, y)) == Color.NONE:
				print(".", end="")
			elif board.get_tile(Grid2(x, y)) == Color.BLACK:
				print("O", end="")
			else:
				print("X", end="")
		print("|", end="\n")

	print("+", end="")
	for x in range(board.get_dimensions().x()):
		print("-", end="")
	print("+", end="\n")


def ask_for_human_move() -> Grid2:
	print("Type in the co-ordinates: x y (or leave blank for pass.")
	input_line = sys.stdin.readline()
	try:
		x = int(input_line.split(' ')[0])
		y = int(input_line.split(' ')[1])
		print("You typed ({}, {})".format(x, y))
		return Grid2(x, y)
	except:
		print("You passed")
		return PASS


if __name__ == "__main__":
	main()
