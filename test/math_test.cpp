#include <gtest/gtest.h>

#include "math.hpp"

TEST(MathTest, round_to)
{
  // zero
  EXPECT_EQ(Math::round_to(0.1f, 0), 0);
  EXPECT_EQ(Math::round_to(2.4f, 0), 2);
  EXPECT_EQ(Math::round_to(2.5f, 0), 3);
  EXPECT_EQ(Math::round_to(4.5f, 0), 5);
  EXPECT_EQ(Math::round_to(5.5f, 0), 6);
  EXPECT_EQ(Math::round_to(9.5f, 0), 10);
  EXPECT_EQ(Math::round_to(10.5f, 0), 11);

  EXPECT_EQ(Math::round_to(-0.1f, 0), 0);
  EXPECT_EQ(Math::round_to(-2.4f, 0), -2);
  EXPECT_EQ(Math::round_to(-2.5f, 0), -3);
  EXPECT_EQ(Math::round_to(-4.5f, 0), -5);
  EXPECT_EQ(Math::round_to(-5.5f, 0), -6);
  EXPECT_EQ(Math::round_to(-9.5f, 0), -10);
  EXPECT_EQ(Math::round_to(-10.5f, 0), -11);

  // positive
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

  // negative
  EXPECT_EQ(Math::round_to(0.1f, -5), 0);
  EXPECT_EQ(Math::round_to(2.4f, -5), 0);
  EXPECT_EQ(Math::round_to(2.5f, -5), 5);
  EXPECT_EQ(Math::round_to(4.5f, -5), 5);
  EXPECT_EQ(Math::round_to(5.5f, -5), 5);
  EXPECT_EQ(Math::round_to(9.5f, -5), 10);
  EXPECT_EQ(Math::round_to(10.5f, -5), 10);

  EXPECT_EQ(Math::round_to(-0.1f, -5), 0);
  EXPECT_EQ(Math::round_to(-2.4f, -5), 0);
  EXPECT_EQ(Math::round_to(-2.5f, -5), -5);
  EXPECT_EQ(Math::round_to(-4.5f, -5), -5);
  EXPECT_EQ(Math::round_to(-5.5f, -5), -5);
  EXPECT_EQ(Math::round_to(-9.5f, -5), -10);
  EXPECT_EQ(Math::round_to(-10.5f, -5), -10);
}

TEST(MathTest, round_to_float)
{
  // zero
  EXPECT_EQ(Math::round_to_float(0.1f, 0.0f), 0.1f);
  EXPECT_EQ(Math::round_to_float(2.4f, 0.0f), 2.4f);
  EXPECT_EQ(Math::round_to_float(2.85f, 0.0f), 2.85f);
  EXPECT_EQ(Math::round_to_float(4.5f, 0.0f), 4.5f);
  EXPECT_EQ(Math::round_to_float(5.5f, 0.0f), 5.5f);
  EXPECT_EQ(Math::round_to_float(9.5f, 0.0f), 9.5f);
  EXPECT_EQ(Math::round_to_float(10.5f, 0.0f), 10.5f);

  EXPECT_EQ(Math::round_to_float(-0.1f, 0.0f), -0.1f);
  EXPECT_EQ(Math::round_to_float(-2.4f, 0.0f), -2.4f);
  EXPECT_EQ(Math::round_to_float(-2.85f, 0.0f), -2.85f);
  EXPECT_EQ(Math::round_to_float(-4.5f, 0.0f), -4.5f);
  EXPECT_EQ(Math::round_to_float(-5.5f, 0.0f), -5.5f);
  EXPECT_EQ(Math::round_to_float(-9.5f, 0.0f), -9.5f);
  EXPECT_EQ(Math::round_to_float(-10.5f, 0.0f), -10.5f);

  // positive
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

  // negative
  EXPECT_EQ(Math::round_to_float(0.1f, -5.5f), 0.0f);
  EXPECT_EQ(Math::round_to_float(2.4f, -5.5f), 0.0f);
  EXPECT_EQ(Math::round_to_float(2.85f, -5.5f), 5.5f);
  EXPECT_EQ(Math::round_to_float(4.5f, -5.5f), 5.5f);
  EXPECT_EQ(Math::round_to_float(5.5f, -5.5f), 5.5f);
  EXPECT_EQ(Math::round_to_float(9.5f, -5.5f), 11.0f);
  EXPECT_EQ(Math::round_to_float(10.5f, -5.5f), 11.0f);

  EXPECT_EQ(Math::round_to_float(-0.1f, -5.5f), 0.0f);
  EXPECT_EQ(Math::round_to_float(-2.4f, -5.5f), 0.0f);
  EXPECT_EQ(Math::round_to_float(-2.85f, -5.5f), -5.5f);
  EXPECT_EQ(Math::round_to_float(-4.5f, -5.5f), -5.5f);
  EXPECT_EQ(Math::round_to_float(-5.5f, -5.5f), -5.5f);
  EXPECT_EQ(Math::round_to_float(-9.5f, -5.5f), -11.0f);
  EXPECT_EQ(Math::round_to_float(-10.5f, -5.5f), -11.0f);
}

/* EOF */
