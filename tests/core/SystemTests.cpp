#include <co/Coral.h>
#include <co/RefPtr.h>
#include <co/ISystem.h>
#include <co/ITypeManager.h>
#include <gtest/gtest.h>

TEST( SystemTests, coralPathDirsAreUnique )
{
	co::Slice<std::string> pathsA = co::getPaths();
	ASSERT_GT( pathsA.getSize(), static_cast<size_t>( 0 ) );

	// adding the same dirs again shouldn't change the CORAL_PATH
	std::string morePaths;
	for( co::Slice<std::string> it( pathsA ); it; it.popFirst() )
	{
		morePaths.append( it.getFirst() );
		morePaths.append( ";" );
	}
	co::addPath( morePaths );

	co::Slice<std::string> pathsB = co::getPaths();
	EXPECT_EQ( pathsA.getSize(), pathsB.getSize() );
}

TEST( SystemTests, startupShutdown )
{
	co::RefPtr<co::ISystem> system1 = co::getSystem();
	ASSERT_TRUE( system1.isValid() );

	co::shutdown();

	co::RefPtr<co::ISystem> system2 = co::getSystem();
	ASSERT_TRUE( system2.isValid() );

	ASSERT_NE( system1.get(), system2.get() );

	// system1 will be destroyed at the end of this test, system2 will remain
}

TEST( SystemTests, typeManager )
{
	co::RefPtr<co::ISystem> system = co::getSystem();
	co::RefPtr<co::ITypeManager> types = system->getTypes();
	ASSERT_TRUE( types.isValid() );
	EXPECT_TRUE( types->findType( "co.ISystem" ) != NULL );
}
