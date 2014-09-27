#include <gtest/gtest.h>

#include <co/Coral.h>
#include <co/IModule.h>
#include <co/ISystem.h>
#include <co/IObject.h>
#include <co/IReflector.h>
#include <co/ModuleState.h>
#include <co/ICompositeType.h>
#include <co/IField.h>
#include <co/IModuleManager.h>
#include <co/IServiceManager.h>
#include <co/IllegalStateException.h>
#include <co/ITypeTransaction.h>
#include <co/IllegalArgumentException.h>
#include <moduleA/IDummy.h>
#include <moduleA/TestInterface.h>

TEST( ModuleTests, setupSystemThenLoadModuleA )
{
	// shutdown and re-setup the system
	co::shutdown();
	co::ISystem* system = co::getSystem();
	system->setup();

	// moduleA should not have been loaded yet
	EXPECT_TRUE( system->getModules()->findModule( "moduleA" ) == NULL );

	// load moduleA: first call should load, second call should just retrieve the module
	co::IModule* moduleA = system->getModules()->load( "moduleA" );
	ASSERT_TRUE( moduleA != NULL );
	EXPECT_EQ( moduleA, system->getModules()->load( "moduleA" ) );

	// now moduleA should have been loaded
	EXPECT_EQ( moduleA, system->getModules()->findModule( "moduleA" ) );

	// and its types should have reflectors
	co::IType* type = co::getType( "moduleA.TestInterface" );
	EXPECT_TRUE( type->getReflector() != NULL );
}

TEST( ModuleTests, setupSystemRequiringModuleB )
{
	// shutdown and re-setup the system requiring moduleB
	co::shutdown();
	co::ISystem* system = co::getSystem();

	// moduleA should not have been loaded yet
	EXPECT_TRUE( system->getModules()->findModule( "moduleA" ) == NULL );

	// setup the system requiring moduleB
	std::string requiredModule( "moduleB" );
	system->setup( co::Slice<std::string>( &requiredModule, 1 ) );

	// now moduleA should have been loaded, as a dependency of moduleB
	ASSERT_TRUE( system->getModules()->findModule( "moduleA" ) != NULL );

	// and its types should have reflectors
	co::IType* type = co::getType( "moduleA.TestInterface" );
	EXPECT_TRUE( type->getReflector() != NULL );
}

TEST( ModuleTests, systemAndModuleLifeCycles )
{
	// fully restart the system
	co::shutdown();
	co::ISystem* system = co::getSystem();

	// cannot tearDown() before setting the system up
	EXPECT_THROW( system->tearDown(), co::IllegalStateException );

	// cannot setupPresentation() before setupBase()
	EXPECT_THROW( system->setupPresentation(), co::IllegalStateException );

	// setupBase() requiring moduleA
	std::string requiredModule( "moduleA" );
	system->setupBase( co::Slice<std::string>( &requiredModule, 1 ) );

	// moduleA should have been loaded, but not moduleB
	co::IModule* moduleA = system->getModules()->findModule( "moduleA" );
	co::IModule* moduleB = system->getModules()->findModule( "moduleB" );
	ASSERT_TRUE( moduleA != NULL );
	ASSERT_TRUE( moduleB == NULL );

	// moduleA's state should be ModuleState_Integrated
	EXPECT_EQ( co::ModuleState_Integrated, moduleA->getState() );

	// cannot tearDown() because the system was not fully set up yet (setupPresentation())
	EXPECT_THROW( system->tearDown(), co::IllegalStateException );

	// setupPresentation()
	system->setupPresentation();

	// moduleA's state should now be ModuleState_PresentationIntegrated
	EXPECT_EQ( co::ModuleState_PresentationIntegrated, moduleA->getState() );

	// moduleB should not have been loaded yet
	ASSERT_TRUE( system->getModules()->findModule( "moduleB" ) == NULL );

	// loading moduleB now should bring it directly to the ModuleState_PresentationIntegrated state
	moduleB = system->getModules()->load( "moduleB" );
	ASSERT_TRUE( moduleB != NULL );
	EXPECT_EQ( co::ModuleState_PresentationIntegrated, moduleB->getState() );

	// ok to tearDown() now
	system->tearDown();

	// all modules should now be in the ModuleState_Disposed state
	EXPECT_EQ( co::ModuleState_Disposed, moduleA->getState() );
	EXPECT_EQ( co::ModuleState_Disposed, moduleB->getState() );

	// cannot load additional modules after the system is torn down
	EXPECT_THROW( system->getModules()->load( "whatever" ), co::IllegalStateException );

	// we need another full restart
	co::shutdown();
	system = co::getSystem();

	// re-setup
	system->setup();

	// cannot setup() the system again before tearing it down
	EXPECT_THROW( system->setup(), co::IllegalStateException );
}

TEST( ModuleTests, crossModuleInheritance )
{
	// instantiate a component from 'moduleA' that implements an interface from 'co'
	co::RefPtr<co::IObject> instance = co::newInstance( "moduleA.TestComponent" );

	// exercise dynamic_casts
	moduleA::TestInterface* ti = instance->getService<moduleA::TestInterface>();
	EXPECT_EQ( instance.get(), ti->getProvider() );

	co::ITypeTransaction* tct = instance->getService<co::ITypeTransaction>();
	EXPECT_EQ( instance.get(), tct->getProvider() );
}

TEST( ModuleTests, crossModuleReflection )
{
	// exercise the reflection API on one of moduleA's types
	co::IType* type = co::getType( "moduleA.TestStruct" );
	co::IReflector* reflector = type->getReflector();
	ASSERT_TRUE( reflector != NULL );

	std::vector<co::int8> instanceMemory( reflector->getSize() );
	void* instancePtr = &instanceMemory.front();
	co::Any instanceAny( true, type, instancePtr );

	EXPECT_NO_THROW( reflector->createValues( instancePtr, 1 ) );

	// get an IField
	co::ICompositeType* ct = co::cast<co::ICompositeType>( type );
	assert( ct );

	co::IField* anInt8Field = co::cast<co::IField>( ct->getMember( "anInt8" ) );
	assert( anInt8Field );

	// exercise the reflection API
	co::AnyValue a1, a2;

	reflector->getField( instanceAny, anInt8Field, a1 );
	EXPECT_EQ( 0, a1.get<int>() );

	a2.set( 7 );
	reflector->setField( instanceAny, anInt8Field, a2 );

	reflector->getField( instanceAny, anInt8Field, a1 );
	EXPECT_EQ( 7, a1.get<int>() );

	// test exception catching
	instanceAny = -0.1;
	try
	{
		reflector->getField( instanceAny, anInt8Field, a1 );
		FAIL();
	}
	catch( co::IllegalArgumentException& e )
	{
		EXPECT_EQ( "illegal instance (moduleA.TestStruct expected, got double)", e.getMessage() );
	}

	reflector->destroyValues( instancePtr, 1 );
}

TEST( ModuleTests, serviceDependencies )
{
	// shutdown and re-setup the system
	co::shutdown();
	co::ISystem* system = co::getSystem();
	system->setup();

	co::IServiceManager* sm = system->getServices();
	co::IInterface* serviceType = co::typeOf<moduleA::TestInterface>::get();

	// register a service provided by moduleA
	ASSERT_TRUE( sm->getIsLazy() );
	sm->addServiceProvider( serviceType, "moduleA.TestComponent" );

	// moduleA should not have been loaded yet
	EXPECT_TRUE( system->getModules()->findModule( "moduleA" ) == NULL );

	// now getting the service should cause moduleA to be loaded
	EXPECT_TRUE( co::getService<moduleA::TestInterface>() != NULL );

	// now moduleA should have been loaded
	EXPECT_TRUE( system->getModules()->findModule( "moduleA" ) != NULL );

	// we need another full restart
	co::shutdown();
	system = co::getSystem();
	system->setup();

	sm = system->getServices();
	serviceType = co::typeOf<moduleA::TestInterface>::get();

	// moduleA should not have been loaded yet
	EXPECT_TRUE( system->getModules()->findModule( "moduleA" ) == NULL );

	// re-register the service provided by moduleA, forcing its immediate instantiation
	sm->setIsLazy( false );
	ASSERT_FALSE( sm->getIsLazy() );
	sm->addServiceProvider( serviceType, "moduleA.TestComponent" );

	// moduleA should already have been loaded now
	EXPECT_TRUE( system->getModules()->findModule( "moduleA" ) != NULL );

	// revert the isLazy option to its default value
	sm->setIsLazy( true );
	ASSERT_TRUE( sm->getIsLazy() );
}

TEST( ModuleTests, luaScriptedNoArg )
{
	try
	{
		co::RefPtr<co::IObject> obj( co::newInstance( "moduleB.LuaScriptedNoArg" ) );
		FAIL() << "exception was not raised";
	}
	catch( co::Exception& e )
	{
		EXPECT_EQ( "error obtaining a reflector for type 'moduleB.LuaScriptedNoArg'"
			" via provider '@lua.Scripted':"
			" this annotation requires a script name as argument", e.getMessage() );
	}
}

TEST( ModuleTests, luaScriptedMissing )
{
	try
	{
		co::RefPtr<co::IObject> obj( co::newInstance( "moduleB.LuaScriptedMissing" ) );
		FAIL() << "exception was not raised";
	}
	catch( co::Exception& e )
	{
		EXPECT_EQ( 0, e.getMessage().find( "error obtaining a reflector for"
			" type 'moduleB.LuaScriptedMissing' via provider"
			" '@lua.Scripted': module 'moduleB.missingScript' not found:" ) );
	}
}

TEST( ModuleTests, luaScriptedError )
{
	try
	{
		co::RefPtr<co::IObject> obj( co::newInstance( "moduleB.LuaScriptedError" ) );
		FAIL() << "exception was not raised";
	}
	catch( co::Exception& e )
	{
		EXPECT_EQ( 0, e.getMessage().find( "error obtaining a reflector for type"
			" 'moduleB.LuaScriptedError' via provider '@lua.Scripted':" ) );
		EXPECT_NE( std::string::npos, e.getMessage().find( "faulty.lua:1: syntax error near 'script'" ) );
	}
}

TEST( ModuleTests, luaScriptedEmpty )
{
	try
	{
		co::RefPtr<co::IObject> obj( co::newInstance( "moduleB.LuaScriptedEmpty" ) );
		FAIL() << "exception was not raised";
	}
	catch( co::Exception& e )
	{
		EXPECT_EQ( "error obtaining a reflector for type 'moduleB.LuaScriptedEmpty'"
			" via provider '@lua.Scripted': script 'moduleB.empty' does"
			" not implement 'moduleB.LuaScriptedEmpty'", e.getMessage() );
	}
}

TEST( ModuleTests, luaScripted )
{
	co::RefPtr<co::IObject> objA( co::newInstance( "moduleB.LuaScriptedA" ) );
	co::RefPtr<co::IObject> objB( co::newInstance( "moduleB.LuaScriptedB" ) );
	EXPECT_EQ( "A's foo", objA->getService<moduleA::IDummy>()->getFoo() );
	EXPECT_EQ( "B's foo", objB->getService<moduleA::IDummy>()->getFoo() );
}
