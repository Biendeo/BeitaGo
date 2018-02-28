#include <gtest/gtest.h>

#include "Engine.h"

TEST(EngineTest, DummyTest) {
	// I don't have any tests, so this just passes.
	Engine e;
	ASSERT_EQ(1, 1);
}

int main(int argc, char* argv[]) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}