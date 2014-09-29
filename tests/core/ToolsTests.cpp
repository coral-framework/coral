#include <gtest/gtest.h>

//---- co::Properties ----------------------------------------------------------

#include <co/private/Properties.h>

TEST(PropertiesTests, parsing) {
  co::Properties p;
  {
    std::istringstream iss("Truth = Beauty");
    ASSERT_EQ(1, p.load(iss));
    EXPECT_EQ("Beauty", p.getProperty("Truth"));
    EXPECT_EQ("", p.getProperty("truth"));
    EXPECT_EQ("default", p.getProperty("truth", "default"));
  }
  {
    std::istringstream iss("       Truth:Nice");
    ASSERT_EQ(1, p.load(iss));
    EXPECT_EQ("Nice", p.getProperty("Truth"));
  }
  {
    std::istringstream iss("Truth                  :Great");
    ASSERT_EQ(1, p.load(iss));
    EXPECT_EQ("Great", p.getProperty("Truth"));
  }
  {
    std::istringstream iss("cheeses");
    ASSERT_EQ(1, p.load(iss));
    EXPECT_EQ("", p.getProperty("cheeses"));
  }
}

TEST(PropertiesTests, nonExistingFile) {
  co::Properties p;
  EXPECT_EQ(-1, p.load("invalidFile.properties"));
}

TEST(PropertiesTests, sampleFile) {
  co::Properties p;
  ASSERT_EQ(2, p.load(TESTS_DATA_DIR "/sample.properties"));
  EXPECT_EQ("World!", p.getProperty("Hello"));
  EXPECT_EQ("apple, banana, pear, cantaloupe, watermelon, kiwi, mango",
            p.getProperty("fruits"));
}

//---- co::StringTokenizer -----------------------------------------------------

#include <co/private/StringTokenizer.h>

namespace {
const std::string oneToken("one");
const std::string threeTokens("one.two.three");
const std::string anotherThreeTokens("one^two^three");
const std::string threeDifferentDelimiters("one.two,three;");
const std::string emptyTokens("..path1.path2..path3....path4.");
}

TEST(StringTokenizerTests, numberOfTokens) {
  std::string token;
  co::StringTokenizer st1(oneToken, ".");
  EXPECT_TRUE(st1.getNext(token));
  EXPECT_FALSE(st1.getNext(token));

  co::StringTokenizer st2(threeTokens, ".");
  EXPECT_TRUE(st2.getNext(token));
  EXPECT_TRUE(st2.getNext(token));
  EXPECT_TRUE(st2.getNext(token));
  EXPECT_FALSE(st2.getNext(token));

  co::StringTokenizer st3(anotherThreeTokens, ".");
  EXPECT_TRUE(st3.getNext(token));
  EXPECT_FALSE(st3.getNext(token));
}

TEST(StringTokenizerTests, singleDelimiter) {
  std::string token;
  co::StringTokenizer st1(oneToken, ".");
  EXPECT_TRUE(st1.getNext(token));
  EXPECT_EQ(token, "one");
  EXPECT_FALSE(st1.getNext(token));
  EXPECT_FALSE(st1.getNext(token));

  co::StringTokenizer st2(anotherThreeTokens, "^");
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
  co::StringTokenizer st1(threeDifferentDelimiters, ";.,");
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
  co::StringTokenizer st1(emptyTokens, ".");
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
