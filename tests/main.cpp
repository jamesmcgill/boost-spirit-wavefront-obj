#include "lib.h"
#include "TestFixture.h"

#include "gtest/gtest.h"
#include <iostream>

//------------------------------------------------------------------------------
int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

//------------------------------------------------------------------------------
TEST_F(TestFixture, WithFixture) {
	EXPECT_EQ(0, zero);
	EXPECT_EQ(1, one);
}

//------------------------------------------------------------------------------
TEST(Test, WithoutFixture) {
	MyLibrary lib;

	EXPECT_EQ(2, lib.x);
	EXPECT_EQ(3, lib.y);
	EXPECT_EQ(10, lib.z());
}

//------------------------------------------------------------------------------