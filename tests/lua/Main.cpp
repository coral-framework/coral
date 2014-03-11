#include <co/Coral.h>
#include <gtest/gtest.h>

int main( int argc, char** argv )
{
	testing::InitGoogleTest( &argc, argv );

	// set up the system
	co::addPath( CORAL_PATH );

	int res = RUN_ALL_TESTS();
	co::shutdown();

	return res;
}
