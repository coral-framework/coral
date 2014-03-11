#include <gtest/gtest.h>
#include <core/utils/Properties.h>
#include <sstream>

TEST( PropertiesTests, parsing )
{
	co::Properties p;
	{
		std::istringstream iss( "Truth = Beauty" );
		ASSERT_EQ( 1, p.load( iss ) );
		EXPECT_EQ( "Beauty", p.getProperty( "Truth" ) );
		EXPECT_EQ( "", p.getProperty( "truth" ) );
		EXPECT_EQ( "default", p.getProperty( "truth", "default" ) );
	}
	{
		std::istringstream iss( "       Truth:Nice" );
		ASSERT_EQ( 1, p.load( iss ) );
		EXPECT_EQ( "Nice", p.getProperty( "Truth" ) );
	}
	{
		std::istringstream iss( "Truth                  :Great" );
		ASSERT_EQ( 1, p.load( iss ) );
		EXPECT_EQ( "Great", p.getProperty( "Truth" ) );
	}
	{
		std::istringstream iss( "cheeses" );
		ASSERT_EQ( 1, p.load( iss ) );
		EXPECT_EQ( "", p.getProperty( "cheeses" ) );
	}
}

TEST( PropertiesTests, nonExistingFile )
{
	co::Properties p;
	EXPECT_EQ( -1, p.load( "invalidFile.properties" ) );
}

TEST( PropertiesTests, sampleFile )
{
	co::Properties p;
	ASSERT_EQ( 2, p.load( TESTS_DATA_DIR "/sample.properties" ) );
	EXPECT_EQ( "World!", p.getProperty( "Hello" ) );
	EXPECT_EQ( "apple, banana, pear, cantaloupe, watermelon, kiwi, mango", p.getProperty( "fruits" ) );
}
