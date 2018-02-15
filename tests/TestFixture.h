//#include "lib.h"
//#include "gtest/gtest.h"
//#include "gmock/gmock.h"

//------------------------------------------------------------------------------
class TestFixture : public ::testing::Test
{
public:
  TestFixture() {}
  virtual ~TestFixture() {}

  const int zero = 0;
  const int one  = 1;
};

//------------------------------------------------------------------------------
