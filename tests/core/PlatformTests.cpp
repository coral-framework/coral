#include <co/Platform.h>
#include <gtest/gtest.h>

TEST( PlatformTests, sizeOfPrimitives )
{
	EXPECT_EQ( 1, sizeof(co::int8) );
	EXPECT_EQ( 1, sizeof(co::uint8) );

	EXPECT_EQ( 2, sizeof(co::int16) );
	EXPECT_EQ( 2, sizeof(co::uint16) );

	EXPECT_EQ( 4, sizeof(co::int32) );
	EXPECT_EQ( 4, sizeof(co::uint32) );
	EXPECT_EQ( 4, sizeof(float) );

	EXPECT_EQ( 8, sizeof(double) );
}
