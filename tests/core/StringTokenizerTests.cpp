#include <core/utils/StringTokenizer.h>
#include <co/Exception.h>

#include <string>
#include <gtest/gtest.h>

using namespace co;

static const std::string oneToken( "one" );
static const std::string threeTokens( "one.two.three" );
static const std::string anotherThreeTokens( "one^two^three" );
static const std::string threeDifferentDelimiters( "one.two,three;" );
static const std::string emptyTokens( "..path1.path2..path3....path4." );

TEST( StringTokenizerTests, numberOfTokens )
{
	StringTokenizer st1( oneToken, "." );
	EXPECT_TRUE( st1.nextToken() );
	EXPECT_FALSE( st1.nextToken() );

	StringTokenizer st2( threeTokens, "." );
	EXPECT_TRUE( st2.nextToken() );
	EXPECT_TRUE( st2.nextToken() );
	EXPECT_TRUE( st2.nextToken() );
	EXPECT_FALSE( st2.nextToken() );

	StringTokenizer st3( anotherThreeTokens, "." );
	EXPECT_TRUE( st3.nextToken() );
	EXPECT_FALSE( st3.nextToken() );
}

TEST( StringTokenizerTests, singleDelimiter )
{
	StringTokenizer st1( oneToken, "." );
	EXPECT_TRUE( st1.nextToken() );
	EXPECT_EQ( st1.getToken(), "one" );
	EXPECT_FALSE( st1.nextToken() );
	EXPECT_FALSE( st1.nextToken() );

	StringTokenizer st2( anotherThreeTokens, "^" );
	EXPECT_TRUE( st2.nextToken() );
	EXPECT_EQ( st2.getToken(), "one" );
	EXPECT_TRUE( st2.nextToken() );
	EXPECT_NE( st2.getToken(), "one" );
	EXPECT_TRUE( st2.nextToken() );
	EXPECT_EQ( st2.getToken(), "three" );
	EXPECT_FALSE( st2.nextToken() );
}

TEST( StringTokenizerTests, multipleDelimiters )
{
	StringTokenizer st1( threeDifferentDelimiters, ";.," );
	EXPECT_TRUE( st1.nextToken() );
	EXPECT_EQ( st1.getToken(), "one" );
	EXPECT_TRUE( st1.nextToken() );
	EXPECT_EQ( st1.getToken(), "two" );
	EXPECT_TRUE( st1.nextToken() );
	EXPECT_EQ( st1.getToken(), "three" );
	EXPECT_FALSE( st1.nextToken() );
}

TEST( StringTokenizerTests, noEmptyTokens )
{
	StringTokenizer st1( emptyTokens, "." );
	EXPECT_TRUE( st1.nextToken() );
	EXPECT_EQ( st1.getToken(), "path1" );
	EXPECT_TRUE( st1.nextToken() );
	EXPECT_EQ( st1.getToken(), "path2" );
	EXPECT_TRUE( st1.nextToken() );
	EXPECT_EQ( st1.getToken(), "path3" );
	EXPECT_TRUE( st1.nextToken() );
	EXPECT_EQ( st1.getToken(), "path4" );
	EXPECT_FALSE( st1.nextToken() );
}
