#include <gtest/gtest.h>

#include "math.hpp"

TEST(MathTest, round_to)
{
  EXPECT_EQ(Math::round_to(0.1f, 5), 0);
  EXPECT_EQ(Math::round_to(2.4f, 5), 0);
  EXPECT_EQ(Math::round_to(2.5f, 5), 5);
  EXPECT_EQ(Math::round_to(4.5f, 5), 5);
  EXPECT_EQ(Math::round_to(5.5f, 5), 5);
  EXPECT_EQ(Math::round_to(9.5f, 5), 10);
  EXPECT_EQ(Math::round_to(10.5f, 5), 10);

  EXPECT_EQ(Math::round_to(-0.1f, 5), 0);
  EXPECT_EQ(Math::round_to(-2.4f, 5), 0);
  EXPECT_EQ(Math::round_to(-2.5f, 5), -5);
  EXPECT_EQ(Math::round_to(-4.5f, 5), -5);
  EXPECT_EQ(Math::round_to(-5.5f, 5), -5);
  EXPECT_EQ(Math::round_to(-9.5f, 5), -10);
  EXPECT_EQ(Math::round_to(-10.5f, 5), -10);
}

TEST(MathTest, round_to_float)
{
  EXPECT_EQ(Math::round_to_float(0.1f, 5.5f), 0.0f);
  EXPECT_EQ(Math::round_to_float(2.4f, 5.5f), 0.0f);
  EXPECT_EQ(Math::round_to_float(2.85f, 5.5f), 5.5f);
  EXPECT_EQ(Math::round_to_float(4.5f, 5.5f), 5.5f);
  EXPECT_EQ(Math::round_to_float(5.5f, 5.5f), 5.5f);
  EXPECT_EQ(Math::round_to_float(9.5f, 5.5f), 11.0f);
  EXPECT_EQ(Math::round_to_float(10.5f, 5.5f), 11.0f);

  EXPECT_EQ(Math::round_to_float(-0.1f, 5.5f), 0.0f);
  EXPECT_EQ(Math::round_to_float(-2.4f, 5.5f), 0.0f);
  EXPECT_EQ(Math::round_to_float(-2.85f, 5.5f), -5.5f);
  EXPECT_EQ(Math::round_to_float(-4.5f, 5.5f), -5.5f);
  EXPECT_EQ(Math::round_to_float(-5.5f, 5.5f), -5.5f);
  EXPECT_EQ(Math::round_to_float(-9.5f, 5.5f), -11.0f);
  EXPECT_EQ(Math::round_to_float(-10.5f, 5.5f), -11.0f);
}

/* EOF */
