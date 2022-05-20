#include <gtest/gtest.h>

#include "path.hpp"

using namespace construo;

TEST(PathTest, path_join)
{
  EXPECT_EQ(path_join("", "bar"), "bar");
  EXPECT_EQ(path_join("foo", ""), "foo/");
  EXPECT_EQ(path_join("foo", "bar"), "foo/bar");
  EXPECT_EQ(path_join("foo/", "bar"), "foo/bar");
  EXPECT_EQ(path_join("foo//", "bar"), "foo//bar");
  EXPECT_EQ(path_join("foo", "/bar"), "/bar");
  EXPECT_EQ(path_join("/foo/", "bar"), "/foo/bar");
  EXPECT_EQ(path_join("/", "bar"), "/bar");
}

TEST(PathTest, path_dirname)
{
  EXPECT_EQ(path_dirname("foo/bar/baz"), "foo/bar");
  EXPECT_EQ(path_dirname("foo/baz"), "foo");
  EXPECT_EQ(path_dirname("foo"), "");
  EXPECT_EQ(path_dirname("/foo/bar/baz"), "/foo/bar");
  EXPECT_EQ(path_dirname("/foo/baz"), "/foo");
  EXPECT_EQ(path_dirname("/foo"), "/");
  EXPECT_EQ(path_dirname("/"), "/");
  EXPECT_EQ(path_dirname(""), "");
}

TEST(PathTest, path_basename)
{
  EXPECT_EQ(path_basename("foo/bar/baz"), "baz");
  EXPECT_EQ(path_basename("foo/baz"), "baz");
  EXPECT_EQ(path_basename("foo"), "foo");
  EXPECT_EQ(path_basename("/foo/bar/baz"), "baz");
  EXPECT_EQ(path_basename("/foo/baz"), "baz");
  EXPECT_EQ(path_basename("/foo"), "foo");
  EXPECT_EQ(path_basename("/"), "");
  EXPECT_EQ(path_basename(""), "");
}

/* EOF */
