#include "LuaState.h"
#include "LuaBinding.h"
#include <co/Log.h>
#include <co/IReflector.h>
#include <co/reserved/OS.h>
#include <co/IllegalArgumentException.h>
#include <lua/Exception.h>
#include <sstream>

extern "C" {
	#include "lfs/lfs.h"
	#include "socket/mime.h"
	#include "socket/luasocket.h"
}

namespace lua {

lua_State* LuaState::sm_L( NULL );
int LuaState::sm_instancesTableRegIdx( 0 );

LuaState::LuaState()
{
	// empty
}

LuaState::~LuaState()
{
	// empty
}

void LuaState::setup()
{
	assert( sm_L == NULL );
	assert( sm_instancesTableRegIdx == 0 );

	sm_L = luaL_newstate();
	assert( sm_L );

	// open all standard Lua libraries
	luaL_openlibs( sm_L );

	// pre-load all statically-linked Lua modules
	lua_getglobal( sm_L, "package" );
	lua_getfield( sm_L, -1, "preload" );

	// pre-load the LuaFileSystem library
	lua_pushcfunction( sm_L, luaopen_lfs );
	lua_setfield( sm_L, -2, "lfs" );

	// pre-load the LuaSocket libraries
	lua_pushcfunction( sm_L, luaopen_socket_core );
	lua_setfield( sm_L, -2, "socket.core" );
	lua_pushcfunction( sm_L, luaopen_mime_core );
	lua_setfield( sm_L, -2, "mime.core" );

	// create the 'co' package
	coPackage::open( sm_L );

	// clear the stack
	lua_settop( sm_L, 0 );
}

void LuaState::tearDown()
{
	// release the 'co.system' service
	coPackage::close( sm_L );

	// force a full GC cycle to ensure all __gc finalizers are called.
	lua_settop( sm_L, 0 );
	lua_gc( sm_L, LUA_GCCOLLECT, 0 );

	// close our Lua universe
	lua_close( sm_L );

	sm_L = NULL;
	sm_instancesTableRegIdx = 0;
}

void LuaState::dumpStack( lua_State* L )
{
	int top = lua_gettop( L );
	printf( "dumpStack -- START\n" );
	for( int i = 1; i <= top; ++i )
	{
		printf( "%d\t%s\t", i, luaL_typename( L, i ) );
		switch( lua_type( L, i ) )
		{
		case LUA_TNUMBER:
			printf( "%g\n", lua_tonumber( L, i ) );
			break;
		case LUA_TSTRING:
			printf( "%s\n", lua_tostring( L, i ) );
			break;
		case LUA_TBOOLEAN:
			printf( "%s\n", lua_toboolean( L, i ) ? "true" : "false" );
			break;
		case LUA_TNIL:
			printf( "%s\n", "nil" );
			break;
		default:
			printf( "%p\n", lua_topointer( L, i ) );
			break;
		}
	}
	printf( "dumpStack -- END\n" );
}

co::TSlice<lua::IInterceptor*> LuaState::getInterceptors()
{
	return CompositeTypeBinding::sm_interceptors;
}

void LuaState::addInterceptor( lua::IInterceptor* interceptor )
{
	return CompositeTypeBinding::addInterceptor( interceptor );
}

void LuaState::removeInterceptor( lua::IInterceptor* interceptor )
{
	return CompositeTypeBinding::removeInterceptor( interceptor );
}

bool LuaState::findScript( lua_State*, const std::string& name, std::string& filename )
{
	static const std::string s_extension( "lua" );

	// look for "?.lua" first, then "?/__init.lua"
	std::string filePaths[2];
	filePaths[0] = name;
	co::OS::convertDotsToDirSeps( filePaths[0] );
	filePaths[1] = filePaths[0];
	filePaths[0].append( ".lua" );
	filePaths[1].append( CORAL_OS_DIR_SEP_STR "__init.lua" );

	return co::OS::searchFile2( co::getPaths(), co::Slice<std::string>( filePaths, 2 ), filename );
}

void LuaState::loadFile( lua_State* L, const std::string& filename )
{
	int res = luaL_loadfilex( L, filename.c_str(), "t" );
	if( res != LUA_OK )
		raiseException( L, res );
}

void LuaState::require( lua_State* L, const std::string& moduleName )
{
	// first we try to retrieve the value cached in 'package.loaded' directly.
	lua_getfield( L, LUA_REGISTRYINDEX, "_LOADED" );
	push( L, moduleName );
	lua_gettable( L, -2 );
	if( !lua_isnil( L, -1 ) )
	{
		lua_replace( L, -2 );
		return;
	}

	// no luck, so we resort to calling the real "require" function
	lua_pop( L, 2 );
	lua_getglobal( L, "require" );
	push( L, moduleName );
	call( L, 1, 1 );
}

static int traceback( lua_State* L )
{
	luaL_traceback( L, L, lua_tostring( L, 1 ), 1 );
	return 1;
}

void LuaState::call( lua_State* L, int numArgs, int numResults )
{
	int errFunc = lua_gettop( L ) - numArgs;
	lua_pushcfunction( L, traceback );
	lua_insert( L, errFunc );

	int res = lua_pcall( L, numArgs, numResults, errFunc );
	lua_remove( L, errFunc );

	if( res != LUA_OK )
		raiseException( L, res );
}

void LuaState::push( lua_State* L, co::Any var )
{
	var = var.asIn();
	const co::__any::State& s = var.state;

	co::TypeKind k = var.getKind();
	assert( k != co::TK_ANY );

	switch( k )
	{
	case co::TK_NULL:	lua_pushnil( L ); break;
	case co::TK_BOOL:	lua_pushboolean( L, s.data.b ); break;
	case co::TK_INT8:	lua_pushinteger( L, s.data.i8 ); break;
	case co::TK_INT16:	lua_pushinteger( L, s.data.i16 ); break;
	case co::TK_INT32:	lua_pushinteger( L, s.data.i32 ); break;
	case co::TK_UINT8:	lua_pushinteger( L, s.data.u8 ); break;
	case co::TK_UINT16:	lua_pushinteger( L, s.data.u16 ); break;
	case co::TK_UINT32:	lua_pushinteger( L, s.data.u32 ); break;
	case co::TK_FLOAT:	lua_pushnumber( L, s.data.f ); break;
	case co::TK_DOUBLE:	lua_pushnumber( L, s.data.d ); break;
	case co::TK_ENUM:
		{
			co::TSlice<std::string> ids = static_cast<co::IEnum*>( s.type )->getIdentifiers();
			co::uint32 id = s.data.u32;
			if( id < ids.getSize() )
				push( L, ids[id] );
			else
			{
				// enum value is out of bounds
				assert( false );
				lua_pushliteral( L, "<invalid>" );
			}
		}
		break;
	case co::TK_STRING:	push( L, *s.data.str ); break;
	case co::TK_ANY: assert( false );
	case co::TK_ARRAY:	pushArray( L, var ); break;
	case co::TK_STRUCT:
		StructBinding::push( L, static_cast<co::IStruct*>( s.type ), s.data.ptr );
		break;
	case co::TK_NATIVECLASS:
		ClassBinding::push( L, static_cast<co::INativeClass*>( s.type ), s.data.ptr );
		break;
	case co::TK_INTERFACE:
		push( L, s.data.service );
		break;
	default:
		assert( false );
		lua_pushnil( L );
	}
}

void LuaState::push( lua_State* L, const std::string& str )
{
	lua_pushlstring( L, str.c_str(), str.length() );
}

template<typename BindingClass, typename InstanceType>
inline void LuaState::pushInstance( lua_State* L, InstanceType* ptr )
{
	if( !ptr )
	{
		lua_pushnil( L );
		return;
	}

	// try to get an existing object (userdata) from the instances table
	pushInstancesTable( L );
	lua_pushlightuserdata( L, ptr );
	lua_rawget( L, -2 );

	// 'itf' not in the instances table yet?
	if( lua_isnil( L, -1 ) )
	{
		lua_pop( L, 1 );

		// create the userdata ...
		BindingClass::create( L, ptr );

		// ... and add it to the instances table
		lua_pushlightuserdata( L, ptr );
		lua_pushvalue( L, -2 );
		lua_rawset( L, -4 );
	}

	// remove the instances table from the stack
	lua_remove( L, -2 );
}

void LuaState::push( lua_State* L, co::IService* service )
{
	if( service && service->getInterface()->getFullName() == "co.IObject" )
		push( L, static_cast<co::IObject*>( service ) );
	else
		pushInstance<ServiceBinding, co::IService>( L, service );
}

void LuaState::push( lua_State* L, co::IObject* object )
{
	pushInstance<ObjectBinding, co::IObject>( L, object );
}

void LuaState::get( lua_State* L, int index, const co::Any& var )
{
	assert( var.isOut() );

	switch( lua_type( L, index ) )
	{
	case LUA_TNIL: var.put( co::Any() ); break;
	case LUA_TBOOLEAN: var.put( lua_toboolean( L, index ) != 0 ); break;

	case LUA_TLIGHTUSERDATA:
		throw lua::Exception( "no conversion from a Lua light userdata" );
		break;

	case LUA_TNUMBER: var.put( lua_tonumber( L, index ) ); break;
	case LUA_TSTRING:
		{
			size_t len;
			const char* cstr = lua_tolstring( L, index, &len );
			var.put( std::string( cstr, len ) );
		}
		break;

	case LUA_TTABLE:
		{
			lua_pushliteral( L, "__coral" );
			lua_rawget( L, index < 0 ? index - 1 : index );
			int tp = lua_type( L, -1 );
			if( tp == LUA_TNIL )
			{
				lua_pop( L, 1 );
				co::TypeKind k = var.getKind();
				if( k == co::TK_ARRAY )
				{
					getArray( L, index, var );
				}
				else
				{
					if( k != co::TK_ANY )
						CORAL_THROW( lua::Exception, "no conversion from Lua array to '" << var.state << "'" );

					co::AnyValue& av = var.get<co::AnyValue&>();
					av.create<std::vector<co::AnyValue> >();
					getArray( L, index, av.getAny() );
				}
			}
			else
			{
				const char* unexpectedType = NULL;
				if( tp != LUA_TUSERDATA )
					unexpectedType = lua_typename( L, -1 );
				else
				{
					co::Any instance;
					if( CompositeTypeBinding::tryGetInstance( L, -1, instance ) )
						var.put( instance );
					else
						unexpectedType = "unknown userdata";
				}
				lua_pop( L, 1 );
				if( unexpectedType )
					CORAL_THROW( lua::Exception, "Coral object expected, got " << unexpectedType );
			}
		}
		break;

	case LUA_TFUNCTION:
		throw lua::Exception( "no conversion from a Lua function" );
		break;

	case LUA_TUSERDATA:
		var.put( CompositeTypeBinding::getInstance( L, index ) );
		break;

	case LUA_TTHREAD:
		throw lua::Exception( "no conversion from a Lua thread" );
		break;

	default:
		assert( false );
	}
}

bool LuaState::findScript( const std::string& name, std::string& filename )
{
	return findScript( getL(), name, filename );
}

co::int32 LuaState::call( const std::string& moduleName, const std::string& functionName,
							co::Slice<co::Any> args, co::Slice<co::Any> results )
{
	lua_State* L = getL();
	int top = lua_gettop( L );

	try
	{
		require( L, moduleName );

		// should we retrieve a function from (presumably) the returned module table?
		if( !functionName.empty() )
		{
			if( !lua_istable( L, -1 ) )
				throw lua::Exception( "module did not return a table" );

			push( L, functionName );
			lua_gettable( L, -2 );
			lua_replace( L, -2 );

			if( lua_isnil( L, -1 ) )
				throw lua::Exception( "no such function" );
		}

		// push the args and call the function
		int numArgs = static_cast<int>( args.getSize() );
		for( int i = 0; i < numArgs; ++i )
			push( L, args[i] );

		call( L, numArgs, LUA_MULTRET );

		int numResults = lua_gettop( L ) - top;
		assert( numResults >= 0 );

		int maxResults = std::min( numResults, static_cast<int>( results.getSize() ) );
		for( int i = 0; i < maxResults; ++i )
		{
			co::AnyValue value;
			get( L, top + i + 1, value );

			// all vars in results[] must be valid 'out' vars
			const co::Any& dest = results[i];
			assert( dest.isValid() && dest.isOut() );

			if( dest.getType()->getKind() == co::TK_ANY )
				value.swap( dest.get<co::AnyValue&>() );
			else
				dest.put( value.getAny() );
		}

		lua_settop( L, top );

		return numResults;
	}
	catch( lua::Exception& e )
	{
		lua_settop( L, top );

		std::stringstream ss;
		ss << "error calling function ";
		if( !functionName.empty() )
			ss << "'" << functionName << "' ";
		ss << "from Lua module '" << moduleName << "': " << e.getMessage();

		throw lua::Exception( ss.str() );
	}
}

void LuaState::collectGarbage()
{
	lua_gc( getL(), LUA_GCCOLLECT, 0 );
}

void LuaState::pushInstancesTable( lua_State* L )
{
	if( sm_instancesTableRegIdx == 0 )
	{
		// the instances table
		lua_newtable( L );

		// the instances table's metatable, for making it a weak table
		lua_createtable( L, 0, 1 );
		lua_pushliteral( L, "__mode" );
		lua_pushliteral( L, "v" );
		lua_rawset( L, -3 );

		lua_setmetatable( L, -2 );

		sm_instancesTableRegIdx = luaL_ref( L, LUA_REGISTRYINDEX );
	}

	lua_rawgeti( L, LUA_REGISTRYINDEX, sm_instancesTableRegIdx );
}

void LuaState::pushArray( lua_State* L, const co::Any& array )
{
	assert( array.isIn() && array.getType()->getKind() == co::TK_ARRAY );
	size_t count = array.getCount();
	lua_createtable( L, count, 0 );
	for( size_t i = 0; i < count; ++i )
	{
		push( L, array[i] );
		lua_rawseti( L, -2, i + 1 );
	}
}

void LuaState::getArray( lua_State* L, int index, const co::Any& array )
{
	assert( lua_type( L, index ) == LUA_TTABLE );

	size_t len = lua_rawlen( L, index );
	array.resize( len );

	if( len == 0 )
		return;

	size_t i = 1;
	int stackTop = lua_gettop( L );

	try
	{
		for( ; i <= len; ++i )
		{
			lua_rawgeti( L, index, i );
			get( L, -1, array[i - 1] );
			lua_settop( L, stackTop );
		}
	}
	catch( std::exception& e )
	{
		lua_settop( L, stackTop );
		CORAL_THROW( lua::Exception, "invalid array element #" << i << " (" << e.what() << ")" );
	}
}

void LuaState::checkType( lua_State* L, int index, int expectedType )
{
	int actualType = lua_type( L, index );
	if( actualType != expectedType )
		CORAL_THROW( lua::Exception, lua_typename( L, expectedType )
						<< " expected, got " << lua_typename( L, actualType ) );
}

void LuaState::raiseException( lua_State* L, int errorCode )
{
	switch( errorCode )
	{
	case LUA_ERRRUN:
	case LUA_ERRSYNTAX:
	case LUA_ERRMEM:
	case LUA_ERRGCMM:
	case LUA_ERRERR:
	case LUA_ERRFILE:
		break;
	default:
		assert( false );
	}

	size_t size;
	const char* msg = lua_tolstring( L, -1, &size );
	lua_pop( L, 1 );

	/*
		Exception messages in the form "{some.IException}some message" are raised
		as instances of some.IException, using Coral's reflection API.
	 */

	// the message must be at least 5 chars long and start with a parenthesis
	if( size >= 5 && msg[0] == '{' )
	{
		const char* typeStart = msg + 1;
		const char* typeEnd = typeStart + 1;
		const char* msgStart = NULL;
		const char* msgEnd = msg + size;

		while( typeEnd < msgEnd )
		{
			if( *typeEnd == '}' )
			{
				msgStart = typeEnd + 1;
				break;
			}
			++typeEnd;
		}

		// the parenthesis must have been closed
		if( msgStart )
		{
			std::string typeName( typeStart, typeEnd - typeStart );
			std::string parsedMsg( msgStart, msgEnd - msgStart );

			// try to obtain an exception type/reflector
			co::IReflector* reflector;
			try
			{
				co::IType* type = co::getType( typeName );
				if( type->getKind() != co::TK_EXCEPTION )
					throw co::IllegalArgumentException( "type is not an exception" );
				reflector = type->getReflector();
			}
			catch( std::exception& e )
			{
				CORAL_THROW( lua::Exception, "[error throwing an exception of type '" << typeName << "' from Lua: "
								<< e.what() << "] " << parsedMsg );
			}

			reflector->raise( parsedMsg );
		}
	}

	throw lua::Exception( std::string( msg, size ) );
}

CORAL_EXPORT_COMPONENT( LuaState, Universe );

} // namespace lua
