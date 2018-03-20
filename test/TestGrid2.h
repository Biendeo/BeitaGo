#include <gtest/gtest.h>

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