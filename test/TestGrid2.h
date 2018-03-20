#include <gtest/gtest.h>
#include <chrono>
#include <random>

#include "Grid2.h"

using namespace BeitaGo;

TEST(Grid2Test, InitialiseTest) {
	Grid2 t1;
	ASSERT_EQ(t1.X(), 0);
	ASSERT_EQ(t1.Y(), 0);

	Grid2 t2(1000);
	ASSERT_EQ(t2.X(), 1000);
	ASSERT_EQ(t2.Y(), 1000);

	Grid2 t3(4000, 300);
	ASSERT_EQ(t3.X(), 4000);
	ASSERT_EQ(t3.Y(), 300);
}

TEST(Grid2Test, ModificationTest) {
	Grid2 t1;
	ASSERT_EQ(t1.X(), 0);
	ASSERT_EQ(t1.Y(), 0);

	t1.X(5);
	t1.Y(6);
	ASSERT_EQ(t1.X(), 5);
	ASSERT_EQ(t1.Y(), 6);
}

TEST(Grid2Test, AdditionTest) {
	Grid2 t1(5, 4);
	Grid2 t2(7, 7);
	t1 += t2;
	ASSERT_EQ(t1.X(), 12);
	ASSERT_EQ(t1.Y(), 11);

	Grid2 t3 = t1 + t2;
	ASSERT_EQ(t3.X(), 19);
	ASSERT_EQ(t3.Y(), 18);
}

TEST(Grid2Test, SubtractionTest) {
	Grid2 t1(47, 12);
	Grid2 t2(29, 2);
	t1 -= t2;
	ASSERT_EQ(t1.X(), 18);
	ASSERT_EQ(t1.Y(), 10);

	Grid2 t3 = t1 - t2;
	ASSERT_EQ(t3.X(), -11);
	ASSERT_EQ(t3.Y(), 8);
}

TEST(Grid2Test, MultiplicationTest) {
	Grid2 t1(13, 3);
	t1 *= 23;
	ASSERT_EQ(t1.X(), 299);
	ASSERT_EQ(t1.Y(), 69);

	Grid2 t3 = t1 * 5;
	ASSERT_EQ(t3.X(), 1495);
	ASSERT_EQ(t3.Y(), 345);
}

TEST(Grid2Test, DivisionTest) {
	Grid2 t1(1300, 500);
	t1 /= 2;
	ASSERT_EQ(t1.X(), 650);
	ASSERT_EQ(t1.Y(), 250);

	Grid2 t3 = t1 /= 10;
	ASSERT_EQ(t3.X(), 65);
	ASSERT_EQ(t3.Y(), 25);
}

TEST(Grid2Test, UnaryMinusTest) {
	Grid2 t1(50, 40);
	Grid2 t2 = -t1;
	ASSERT_EQ(t2.X(), -50);
	ASSERT_EQ(t2.Y(), -40);
}

TEST(Grid2Test, EqualityTest) {
	Grid2 t1(45, -25);
	Grid2 t2(45, -25);
	ASSERT_EQ(t1, t2);

	t1 += t2;
	ASSERT_NE(t1, t2);

	t1 = t2;
	ASSERT_EQ(t1, t2);
}

TEST(Grid2Test, ManhattenDistanceTest) {
	Grid2 t1(5, 3);
	ASSERT_EQ(t1.ManhattenDistance(), 8);
}

TEST(Grid2Test, RandomisedTest) {
	std::mt19937 rand(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));

	for (int i = 0; i < 10000; ++i) {
		int x1 = rand();
		Grid2 g1(x1);
		ASSERT_EQ(g1.X(), x1);
		ASSERT_EQ(g1.Y(), x1);

		int x2 = rand();
		int y2 = rand();
		Grid2 g2(x2, y2);
		ASSERT_EQ(g2.X(), x2);
		ASSERT_EQ(g2.Y(), y2);

		int x3 = rand();
		int y3 = rand();
		Grid2 g3;
		g3.X(x3);
		g3.Y(y3);
		ASSERT_EQ(g3.X(), x3);
		ASSERT_EQ(g3.Y(), y3);

		g3 += g2;
		ASSERT_EQ(g3.X(), x2 + x3);
		ASSERT_EQ(g3.Y(), y2 + y3);

		g3 -= g2;
		ASSERT_EQ(g3.X(), x3);
		ASSERT_EQ(g3.Y(), y3);

		Grid2 g4 = g3 + g2;
		ASSERT_EQ(g4.X(), x2 + x3);
		ASSERT_EQ(g4.Y(), y2 + y3);

		Grid2 g5 = g4 - g2;
		ASSERT_EQ(g5.X(), x3);
		ASSERT_EQ(g5.Y(), y3);

		Grid2 g6 = g3;
		ASSERT_EQ(g6.X(), x3);
		ASSERT_EQ(g6.Y(), y3);

		int r = rand();
		g6 *= r;
		ASSERT_EQ(g6.X(), x3 * r);
		ASSERT_EQ(g6.Y(), y3 * r);

		Grid2 g7 = g3 * r;
		ASSERT_EQ(g7.X(), x3 * r);
		ASSERT_EQ(g7.Y(), y3 * r);

		Grid2 g8 = g7;
		g8 /= r;
		ASSERT_EQ(g8.X(), g7.X() / r);
		ASSERT_EQ(g8.Y(), g7.Y() / r);

		g8 = g7 / r;
		ASSERT_EQ(g8.X(), g7.X() / r);
		ASSERT_EQ(g8.Y(), g7.Y() / r);
	}
}