#include "gtest/gtest.h"

//--------------------------------------------------------------------------------------------------
class TestFixture : public ::testing::Test
{
public:
	TestFixture() {}
	virtual ~TestFixture() {}

	const int zero = 0;
	const int one = 1;
};
