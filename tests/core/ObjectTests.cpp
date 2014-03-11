#include <gtest/gtest.h>
#include <co/Coral.h>
#include <co/RefPtr.h>
#include <co/IArray.h>
#include <co/IObject.h>
#include <co/IInterface.h>
#include <co/NoSuchPortException.h>

TEST( ObjectTests, getServiceByType )
{
	co::RefPtr<co::IObject> obj = co::newInstance( "co.ArrayType" );

	co::IType* s = obj->getService<co::IType>();
	EXPECT_TRUE( s != NULL && s == obj->getService<co::IArray>() );

	EXPECT_TRUE( obj->findService<co::IInterface>() == NULL );
	EXPECT_THROW( obj->getService<co::IInterface>(), co::NoSuchPortException );
}

TEST( ObjectTests, getServiceByName )
{
	co::RefPtr<co::IObject> obj = co::newInstance( "co.ArrayType" );

	co::IService* s = obj->getService( "type" );
	EXPECT_TRUE( s != NULL && s == obj->getService<co::IArray>() );

	EXPECT_TRUE( obj->findService( "unknown" ) == NULL );
	EXPECT_THROW( obj->getService( "unknown" ), co::NoSuchPortException );
}
