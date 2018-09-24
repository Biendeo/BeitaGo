#include <gtest/gtest.h>

#include <iostream>

#include "Constants.h"
#include "Engine.h"
#include "HumanPlayer.h"

using namespace BeitaGo;

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
	for (int y = board.GetDimensions().Y() - 1; y >= 0; --y) {
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

TEST(EngineTest, RegionDestroyTest) {
	Engine e;
	e.NewGame(Grid2(5, 5), new HumanPlayer(e, Color::Black), new HumanPlayer(e, Color::White));

	Player& player1 = e.GetPlayer1();
	Player& player2 = e.GetPlayer2();

	for (int y = 0; y < e.GetBoard().GetDimensions().Y(); ++y) {
		for (int x = 0; x < e.GetBoard().GetDimensions().X(); ++x) {
			ASSERT_EQ(e.GetBoard().GetTile(Grid2(x, y)), Color::None);
		}
	}

	player1.ActDecision(Grid2(2, 2));
	player2.ActDecision(Grid2(2, 3));
	player1.ActDecision(PASS);
	player2.ActDecision(Grid2(3, 2));
	player1.ActDecision(PASS);
	player2.ActDecision(Grid2(2, 1));
	player1.ActDecision(PASS);
	player2.ActDecision(Grid2(1, 2));

	ASSERT_EQ(e.GetBoard().GetTile(Grid2(2, 2)), Color::None);
	ASSERT_EQ(e.GetBoard().GetTile(Grid2(2, 3)), Color::White);
	ASSERT_EQ(e.GetBoard().GetTile(Grid2(3, 2)), Color::White);
	ASSERT_EQ(e.GetBoard().GetTile(Grid2(2, 1)), Color::White);
	ASSERT_EQ(e.GetBoard().GetTile(Grid2(1, 2)), Color::White);
}
