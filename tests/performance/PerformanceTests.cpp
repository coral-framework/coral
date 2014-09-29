#include <co/Coral.h>
#include <co/ISystem.h>
#include <co/CSLError.h>
#include <co/ITypeManager.h>
#include <co/IModuleManager.h>
#include <lua/IState.h>
#include <lua/Exception.h>
#include <gtest/gtest.h>

TEST( PerformanceTests, callLuaFunction )
{
	std::string str;
	co::CSLError cslError;

	co::Any args[] = {
		3.14,
		true,
		-8,
		str,
		co::getSystem()->getTypes(),
		cslError,
		co::getPaths()
	};

	static const int NUM_ARGS = CORAL_ARRAY_LENGTH( args );

	co::AnyValue values[NUM_ARGS];
	co::Any results[NUM_ARGS];
	for( int i = 0; i < NUM_ARGS; ++i )
		results[i] = values[i];

	std::string moduleName( "PerformanceTests" );
	std::string functionName( "passthrough" );

	co::getSystem()->getModules()->load( "lua" );

	lua::IState* state = co::getService<lua::IState>();

	for( int i = 0; i < 1000; ++i )
		state->call( moduleName, functionName, args, results );
}

// --- Sanity Tests --- //

TEST( PerformanceTests, test )
{
	// dummy
}

