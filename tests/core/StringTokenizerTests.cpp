#include <core/utils/StringTokenizer.h>
#include <co/Exception.h>

#include <string>
#include <gtest/gtest.h>

using namespace co;

static const std::string oneToken("one");
static const std::string threeTokens("one.two.three");
static const std::string anotherThreeTokens("one^two^three");
static const std::string threeDifferentDelimiters("one.two,three;");
static const std::string emptyTokens("..path1.path2..path3....path4.");

TEST(StringTokenizerTests, numberOfTokens) {
  std::string token;
  StringTokenizer st1(oneToken, ".");
  EXPECT_TRUE(st1.getNext(token));
  EXPECT_FALSE(st1.getNext(token));

  StringTokenizer st2(threeTokens, ".");
  EXPECT_TRUE(st2.getNext(token));
  EXPECT_TRUE(st2.getNext(token));
  EXPECT_TRUE(st2.getNext(token));
  EXPECT_FALSE(st2.getNext(token));

  StringTokenizer st3(anotherThreeTokens, ".");
  EXPECT_TRUE(st3.getNext(token));
  EXPECT_FALSE(st3.getNext(token));
}

TEST(StringTokenizerTests, singleDelimiter) {
  std::string token;
  StringTokenizer st1(oneToken, ".");
  EXPECT_TRUE(st1.getNext(token));
  EXPECT_EQ(token, "one");
  EXPECT_FALSE(st1.getNext(token));
  EXPECT_FALSE(st1.getNext(token));

  StringTokenizer st2(anotherThreeTokens, "^");
  EXPECT_TRUE(st2.getNext(token));
  EXPECT_EQ(token, "one");
  EXPECT_TRUE(st2.getNext(token));
  EXPECT_NE(token, "one");
  EXPECT_TRUE(st2.getNext(token));
  EXPECT_EQ(token, "three");
  EXPECT_FALSE(st2.getNext(token));
}

TEST(StringTokenizerTests, multipleDelimiters) {
  std::string token;
  StringTokenizer st1(threeDifferentDelimiters, ";.,");
  EXPECT_TRUE(st1.getNext(token));
  EXPECT_EQ(token, "one");
  EXPECT_TRUE(st1.getNext(token));
  EXPECT_EQ(token, "two");
  EXPECT_TRUE(st1.getNext(token));
  EXPECT_EQ(token, "three");
  EXPECT_FALSE(st1.getNext(token));
}

TEST(StringTokenizerTests, noEmptyTokens) {
  std::string token;
  StringTokenizer st1(emptyTokens, ".");
  EXPECT_TRUE(st1.getNext(token));
  EXPECT_EQ(token, "path1");
  EXPECT_TRUE(st1.getNext(token));
  EXPECT_EQ(token, "path2");
  EXPECT_TRUE(st1.getNext(token));
  EXPECT_EQ(token, "path3");
  EXPECT_TRUE(st1.getNext(token));
  EXPECT_EQ(token, "path4");
  EXPECT_FALSE(st1.getNext(token));
}
