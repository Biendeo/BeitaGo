#include <iostream>

#include "AIPlayer.h"
#include "HumanPlayer.h"
#include "Engine.h"

using namespace BeitaGo;

void PrintBoard(const Board& board);
Grid2 AskForHumanMove();

int main(int argc, char* argv[]) {
	std::cout << "This just is an ASCII version of the game to test it before using the UI.\n";

	Engine e;
	e.NewGame();

	HumanPlayer& humanPlayer = dynamic_cast<HumanPlayer&>(e.GetPlayer1());
	AIPlayer& aiPlayer = dynamic_cast<AIPlayer&>(e.GetPlayer2());

	// I know that player 1 is human and player 2 is a dumb AI.
	PrintBoard(e.GetBoard());
	while (!e.GetBoard().IsGameOver()) {
		humanPlayer.ActDecision(AskForHumanMove());
		PrintBoard(e.GetBoard());
		aiPlayer.ActDecision(aiPlayer.MakeDecision());
		PrintBoard(e.GetBoard());
	}

	return 0;
}

void PrintBoard(const Board& board) {
	// Let's make some space for the board.
	std::cout << "\n\n";

	// First, print the top row.
	std::cout << "+";
	for (int x = 0; x < board.GetDimensions().X(); ++x) {
		std::cout << "-";
	}
	std::cout << "+\n";

	// Now, for each row, print the sides and any tiles.
	for (int y = 0; y < board.GetDimensions().Y(); ++y) {
		std::cout << "|";
		for (int x = 0; x < board.GetDimensions().X(); ++x) {
			if (board.GetTile(Grid2(x, y)) == Color::None) {
				std::cout << ".";
			} else if (board.GetTile(Grid2(x, y)) == Color::Black) {
				std::cout << "O";
			} else {
				std::cout << "X";
			}
		}
		std::cout << "|\n";
	}

	// Finally print the bottom row.
	std::cout << "+";
	for (int x = 0; x < board.GetDimensions().X(); ++x) {
		std::cout << "-";
	}
	std::cout << "+\n";
}

Grid2 AskForHumanMove() {
	std::cout << "Type in the co-ordinates: x y (or leave blank for pass).\n";
	std::string inputString;
	std::getline(std::cin, inputString);

	int x = -1;
	int y = -1;
	sscanf_s(inputString.c_str(), "%d %d", &x, &y);
	if (x == -1 || y == -1) {
		return Player::PASS;
	} else {
		return Grid2(x, y);
	}
}