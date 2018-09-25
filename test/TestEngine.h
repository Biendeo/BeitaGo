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

TEST(EngineTest, ScoringTest) {
	Engine e;
	e.NewGame(Grid2(9, 9), new HumanPlayer(e, Color::Black), new HumanPlayer(e, Color::White));

	Player& player1 = e.GetPlayer1();
	Player& player2 = e.GetPlayer2();

	for (int y = 0; y < e.GetBoard().GetDimensions().Y(); ++y) {
		for (int x = 0; x < e.GetBoard().GetDimensions().X(); ++x) {
			ASSERT_EQ(e.GetBoard().GetTile(Grid2(x, y)), Color::None);
		}
	}

	// This game is based on a match between an older version of the MCTS AI and GNU GO.
	// The values and moves are based on whatever Sabaki believes is correct.
	player1.ActDecision(Grid2(6, 5));
	player2.ActDecision(Grid2(2, 7));
	player1.ActDecision(Grid2(4, 5));
	player2.ActDecision(Grid2(7, 6));
	player1.ActDecision(Grid2(6, 7));
	player2.ActDecision(Grid2(6, 4));
	player1.ActDecision(Grid2(7, 5));
	player2.ActDecision(Grid2(2, 1));
	player1.ActDecision(Grid2(5, 4));
	player2.ActDecision(Grid2(5, 5));
	player1.ActDecision(Grid2(5, 3));
	player2.ActDecision(Grid2(3, 6));
	player1.ActDecision(Grid2(3, 3));
	player2.ActDecision(Grid2(1, 7));
	player1.ActDecision(Grid2(1, 2));
	player2.ActDecision(Grid2(5, 2));
	player1.ActDecision(Grid2(4, 2));
	player2.ActDecision(Grid2(2, 2));
	player1.ActDecision(Grid2(1, 3));
	player2.ActDecision(Grid2(4, 6));
	player1.ActDecision(Grid2(6, 2));
	player2.ActDecision(Grid2(5, 6));
	player1.ActDecision(Grid2(6, 6));
	player2.ActDecision(Grid2(2, 3));
	player1.ActDecision(Grid2(2, 4));
	player2.ActDecision(Grid2(1, 4));
	player1.ActDecision(Grid2(3, 4));
	player2.ActDecision(Grid2(7, 1));
	player1.ActDecision(Grid2(6, 1));
	player2.ActDecision(Grid2(5, 1));
	player1.ActDecision(Grid2(1, 1));
	player2.ActDecision(Grid2(6, 3));
	player1.ActDecision(Grid2(7, 2));
	player2.ActDecision(Grid2(2, 6));
	player1.ActDecision(Grid2(3, 8));
	player2.ActDecision(Grid2(8, 4));
	player1.ActDecision(Grid2(1, 5));
	player2.ActDecision(Grid2(5, 0));
	player1.ActDecision(Grid2(6, 0));
	player2.ActDecision(Grid2(8, 2));
	player1.ActDecision(Grid2(7, 3));
	player2.ActDecision(Grid2(7, 4));
	player1.ActDecision(Grid2(8, 5));
	player2.ActDecision(Grid2(7, 0));
	player1.ActDecision(Grid2(8, 3));
	player2.ActDecision(Grid2(0, 4));
	player1.ActDecision(Grid2(1, 6));
	player2.ActDecision(Grid2(2, 0));
	player1.ActDecision(Grid2(1, 0));
	player2.ActDecision(Grid2(2, 5));
	player1.ActDecision(Grid2(0, 3));
	player2.ActDecision(Grid2(0, 5));
	player1.ActDecision(Grid2(0, 6));
	player2.ActDecision(Grid2(1, 4));
	player1.ActDecision(Grid2(0, 4));
	player2.ActDecision(Grid2(5, 8));
	player1.ActDecision(Grid2(5, 7));
	player2.ActDecision(Grid2(4, 7));
	player1.ActDecision(Grid2(4, 8));
	player2.ActDecision(Grid2(6, 8));
	player1.ActDecision(Grid2(7, 8));
	player2.ActDecision(Grid2(5, 8));
	player1.ActDecision(Grid2(6, 8));
	player2.ActDecision(Grid2(4, 1));
	player1.ActDecision(Grid2(3, 2));
	player2.ActDecision(Grid2(4, 4));
	player1.ActDecision(Grid2(3, 5));
	player2.ActDecision(Grid2(0, 7));
	player1.ActDecision(Grid2(2, 8));
	player2.ActDecision(Grid2(3, 1));
	player1.ActDecision(Grid2(6, 4));
	player2.ActDecision(Grid2(1, 8));
	player1.ActDecision(Grid2(5, 8));
	player2.ActDecision(Grid2(8, 7));
	player1.ActDecision(PASS);
	player2.ActDecision(Grid2(7, 7));

	ASSERT_EQ(e.GetBoard().Score(), -6.5);
}
