#include "LuaBinding.h"
#include "LuaState.h"
#include "LuaComponent.h"
#include <co/Log.h>
#include <co/ISystem.h>
#include <co/IObject.h>
#include <co/IReflector.h>
#include <co/IMethod.h>
#include <co/IField.h>
#include <co/IPort.h>
#include <co/IParameter.h>
#include <co/ModuleLoadException.h>
#include <co/IllegalArgumentException.h>
#include <lua/Exception.h>
#include <algorithm>
#include <cstring>
#include <sstream>

namespace lua {

/*****************************************************************************/
/*  Helper Macros to Shield Lua from C++ Exceptions                          */
/*****************************************************************************/

#define __BEGIN_EXCEPTIONS_BARRIER__ try {

#define __END_EXCEPTIONS_BARRIER__ \
	} catch( ... ) { handleException( L ); } return lua_error( L );


/*****************************************************************************/
/*  Helper Function to Handle C++ Exceptions and Push a Lua Error String     */
/*****************************************************************************/

class MsgOnStackException {};

static void handleException( lua_State* L )
{
	try
	{
		throw;
	}
	catch( const lua::MsgOnStackException& )
	{
		// message is already on the stack
	}
	catch( const co::Exception& e )
	{
		lua_pushfstring( L, "{%s}%s", e.getTypeName(), e.getMessage().c_str() );
	}
    catch( const std::exception& e )
    {
		lua_pushfstring( L, "std::exception: %s", e.what() );
    }
	catch( ... )
	{
		lua_pushstring( L, "unknown C++ exception" );
	}
}

/*****************************************************************************/
/*  Helper class that manages the 'co' package                               */
/*****************************************************************************/

namespace {
	std::vector<co::RefPtr<LuaComponent> > sg_luaComponents;
}

void coPackage::open( lua_State* L )
{
	static luaL_Reg libFunctions[] = {
		{ "logLM", logLM },
		{ "setLogHandler", setLogHandler },
		{ "addPath", addPath },
		{ "getPaths", getPaths },
		{ "findScript", findScript },
		{ "findFile", findFile },
		{ "getType", getType },
		{ "new", genericNew },
		{ "newComponentType", newComponentType },
		{ "newComponentInstance", newComponentInstance },
		{ NULL, NULL }
	};

	// create our lib table, containing the libFunctions
	luaL_newlib( L, libFunctions );

	// set the 'co.version' constant
	lua_pushliteral( L, CORAL_VERSION_STR );
	lua_setfield( L, -2, "version" );

	// set the 'co.buildKey' constant
	lua_pushliteral( L, CORAL_BUILD_KEY );
	lua_setfield( L, -2, "buildKey" );

	// set the 'co.buildMode' constant
	lua_pushliteral( L, CORAL_BUILD_MODE );
	lua_setfield( L, -2, "buildMode" );

	// set the 'co.compilerOutputRevision' constant
	lua_pushinteger( L, CORAL_COMPILER_OUTPUT_REVISION );
	lua_setfield( L, -2, "compilerOutputRevision" );

	// set the 'co.system' singleton
	LuaState::push( L, co::getSystem() );
	lua_setfield( L, -2, "system" );

	// save the table as a global
	lua_setglobal( L, "co" );
}

void coPackage::close( lua_State* L )
{
	// release the 'co.system' interface
	lua_getglobal( L, "co" );
	if( lua_istable( L, -1 ) )
	{
		lua_pushnil( L );
		lua_setfield( L, -2, "system" );
	}
	lua_pop( L, 1 );

	// release all LuaComponents
	size_t numLuaComponents = sg_luaComponents.size();
	for( size_t i = 0; i < numLuaComponents; ++i )
		sg_luaComponents[i]->releaseComponent();
	sg_luaComponents.clear();

	// release all type bindings
	CompositeTypeBinding::releaseBindings( L );
}

int coPackage::logLM( lua_State* L )
{
	int level = luaL_checkint( L, 1 ) - 1;
	if( level < co::LOG_INFO || level > co::LOG_FATAL )
		return luaL_error( L, "level %d is out of range", level );

	const char* message = luaL_checkstring( L, 2 );
	CORAL_LOG_LEVEL_DEBUG( static_cast<co::LogLevel>( level ), false ) << message;

	return 0;
}

void logHandler( const co::Log& log )
{
	lua_State* L = LuaState::getL();
	lua_pushlightuserdata( L, reinterpret_cast<void*>( &logHandler ) );
	lua_rawget( L, LUA_REGISTRYINDEX );
	assert( lua_isfunction( L, -1 ) );
	lua_pushlstring( L, log.message.c_str(), log.message.length() );
	lua_pushinteger( L, log.level + 1 );
	lua_pushboolean( L, log.isDebug );
	LuaState::call( L, 3, 0 );
}

int coPackage::setLogHandler( lua_State* L )
{
	assert( L == LuaState::getL() );
	co::LogHandler handler = NULL;
	if( !lua_isnil( L, 1 ) )
	{
		luaL_checktype( L, 1, LUA_TFUNCTION );
		lua_pushlightuserdata( L, reinterpret_cast<void*>( &logHandler ) );
		lua_pushvalue( L, 1 );
		lua_rawset( L, LUA_REGISTRYINDEX );
		handler = logHandler;
	}
	co::setLogHandler( handler );
	return 0;
}

int coPackage::addPath( lua_State* L )
{
	int numArgs = lua_gettop( L );
	for( int i = 1; i <= numArgs; ++i )
	{
		const char* dirList = luaL_checkstring( L, i );
		co::addPath( dirList );
	}
	return 0;
}

int coPackage::getPaths( lua_State* L )
{
	__BEGIN_EXCEPTIONS_BARRIER__

	LuaState::push( L, co::getPaths() );

	return 1;

	__END_EXCEPTIONS_BARRIER__
}

int coPackage::findScript( lua_State* L )
{
	const char* scriptName = luaL_checkstring( L, 1 );
	std::string filename;
	if( LuaState::findScript( L, scriptName, filename ) )
		lua_pushlstring( L, filename.data(), filename.length() );
	else
		lua_pushnil( L );
	return 1;
}

int coPackage::findFile( lua_State* L )
{
	const char* moduleName = luaL_checkstring( L, 1 );
	const char* fileName = luaL_checkstring( L, 2 );
	std::string path;
	if( co::findFile( moduleName, fileName, path ) )
		lua_pushlstring( L, path.data(), path.length() );
	else
		lua_pushnil( L );
	return 1;
}

int coPackage::getType( lua_State* L )
{
	const char* typeName = luaL_checkstring( L, 1 );

	__BEGIN_EXCEPTIONS_BARRIER__

	co::IType* type = co::getType( typeName );
	LuaState::push( L, type );

	return 1;

	__END_EXCEPTIONS_BARRIER__
}

int coPackage::genericNew( lua_State* L )
{
	const char* typeName = luaL_checkstring( L, 1 );

	__BEGIN_EXCEPTIONS_BARRIER__

	co::IType* type = co::getType( typeName );
	co::TypeKind kind = type->getKind();
	if( kind == co::TK_COMPONENT )
	{
		co::IReflector* reflector = type->getReflector();
		co::RefPtr<co::IObject> c = reflector->newInstance();
		LuaState::push( L, c.get() );
	}
	else if( co::isComplexValue( kind ) )
	{
		ValueBinding::push( L, type, NULL );
	}
	else
	{
		lua_pushvalue( L, 1 );
		lua_pushliteral( L, " is not instantiable" );
		lua_concat( L, 2 );
		throw lua::MsgOnStackException();
	}

	return 1;

	__END_EXCEPTIONS_BARRIER__
}

int coPackage::newComponentType( lua_State* L )
{
	luaL_checktype( L, 2, LUA_TTABLE );

	__BEGIN_EXCEPTIONS_BARRIER__

	co::IComponent* ct = co::cast<co::IComponent>( ServiceBinding::getInstance( L, 1 ) );

	lua_pushvalue( L, 2 );
	int tableRef = luaL_ref( L, LUA_REGISTRYINDEX );

	co::RefPtr<LuaComponent> lc( new LuaComponent );
	lc->setComponent( ct, tableRef );

	sg_luaComponents.push_back( lc );

	LuaState::push( L, static_cast<co::IReflector*>( lc.get() ) );

	return 1;

	__END_EXCEPTIONS_BARRIER__
}

int coPackage::newComponentInstance( lua_State* L )
{
	luaL_checktype( L, 2, LUA_TTABLE );

	__BEGIN_EXCEPTIONS_BARRIER__

	co::IReflector* prototype = co::cast<co::IReflector>( ServiceBinding::getInstance( L, 1 ) );
	if( !prototype )
		throw lua::Exception( "bad argument #1 to co.newComponentInstance (reflector expected)" );

	assert( prototype->getFacet()->getOwner()->getFullName() == "lua.Component" );

	lua_pushvalue( L, 2 );
	int tableRef = luaL_ref( L, LUA_REGISTRYINDEX );

	LuaComponent* lc = new LuaComponent;
	lc->setInstance( static_cast<LuaComponent*>( prototype ), tableRef );

	LuaState::push( L, static_cast<co::IObject*>( lc ) );

	return 1;

	__END_EXCEPTIONS_BARRIER__
}

/*****************************************************************************/
/*  Class with re-usable functions for binding co::CompositeTypes to Lua     */
/*****************************************************************************/

CompositeTypeBinding::CompositeTypeList CompositeTypeBinding::sm_boundTypes;
CompositeTypeBinding::InterceptorList CompositeTypeBinding::sm_interceptors;

co::ICompositeType* CompositeTypeBinding::getType( lua_State* L, int udataIdx )
{
	if( !lua_isuserdata( L, udataIdx ) )
		return NULL;

	lua_getmetatable( L, udataIdx );
	lua_rawgeti( L, -1, 1 );

	co::ICompositeType* ct = reinterpret_cast<co::ICompositeType*>( lua_touserdata( L, -1 ) );
	lua_pop( L, 2 );

	return ct;
}

co::Any CompositeTypeBinding::getInstance( lua_State* L, int udataIdx )
{
	co::Any instance;
	if( !tryGetInstance( L, udataIdx, instance ) )
		throw lua::Exception( "unknown userdata type" );
	return instance;
}

bool CompositeTypeBinding::tryGetInstance( lua_State* L, int udataIdx, co::Any& instance )
{
	co::ICompositeType* ct = getType( L, udataIdx );
	if( !ct )
		return false;

	if( co::isValue( ct->getKind() ) )
		instance = ValueBinding::getInstance( L, udataIdx );
	else
		instance = ServiceBinding::getInstance( L, udataIdx );

	return true;
}

void CompositeTypeBinding::releaseBindings( lua_State* L )
{
	for( CompositeTypeList::iterator it = sm_boundTypes.begin(); it != sm_boundTypes.end(); ++it )
	{
		co::ICompositeType* ct = *it;
		lua_pushlightuserdata( L, ct );
		lua_pushnil( L );
		lua_rawset( L, LUA_REGISTRYINDEX );
	}
	sm_boundTypes.clear();
}

void CompositeTypeBinding::addInterceptor( IInterceptor* interceptor )
{
	if( !interceptor )
		throw co::IllegalArgumentException( "illegal null loader" );

	// assert there are no duplicates
	assert( std::find( sm_interceptors.begin(), sm_interceptors.end(), interceptor ) == sm_interceptors.end() );

	sm_interceptors.push_back( interceptor );
}

void CompositeTypeBinding::removeInterceptor( IInterceptor* interceptor )
{
	InterceptorList::iterator newEnd = std::remove( sm_interceptors.begin(), sm_interceptors.end(), interceptor );

	if( newEnd == sm_interceptors.end() )
		throw co::IllegalArgumentException( "the specified loader was not found" );

	sm_interceptors.erase( newEnd, sm_interceptors.end() );
}

struct Metamethods
{
	lua_CFunction index;
	lua_CFunction newIndex;
	lua_CFunction gc;
	lua_CFunction toString;
};

static const Metamethods METAMETHODS_TABLE[] = {
	{ StructBinding::index, StructBinding::newIndex, StructBinding::gc, StructBinding::toString },
	{ ClassBinding::index, ClassBinding::newIndex, ClassBinding::gc, ClassBinding::toString },
	{ ServiceBinding::index, ServiceBinding::newIndex, ServiceBinding::gc, ServiceBinding::toString },
	{ ObjectBinding::index, ObjectBinding::newIndex, ObjectBinding::gc, ObjectBinding::toString }
};

void CompositeTypeBinding::pushMetatable( lua_State* L, co::ICompositeType* ct, co::IReflector* reflector )
{
	assert( ct );
	lua_pushlightuserdata( L, ct );
	lua_rawget( L, LUA_REGISTRYINDEX );

	// type doesn't have a metatable yet?
	if( lua_isnil( L, -1 ) )
	{
		co::TypeKind tag = ct->getKind();
		if( tag == co::TK_INTERFACE && ct->getFullName() == "co.IObject" )
			tag = co::TK_COMPONENT;

		assert( co::isComposite( tag ) );
		const Metamethods& mms = METAMETHODS_TABLE[tag - co::TK_STRUCT];

		lua_pop( L, 1 );
		lua_createtable( L, 1, 5 );

		// save the ICompositeType at MT[1]
		lua_pushlightuserdata( L, ct );
		lua_rawseti( L, -2, 1 );

		// if a reflector was provided, save it at MT[2]
		if( reflector )
		{
			lua_pushlightuserdata( L, reflector );
			lua_rawseti( L, -2, 2 );
		}

		assert( mms.index );
		lua_pushliteral( L, "__index" );
		lua_pushcfunction( L, mms.index );
		lua_rawset( L, -3 );

		assert( mms.newIndex );
		lua_pushliteral( L, "__newindex" );
		lua_pushcfunction( L, mms.newIndex );
		lua_rawset( L, -3 );

		assert( mms.gc );
		lua_pushliteral( L, "__gc" );
		lua_pushcfunction( L, mms.gc );
		lua_rawset( L, -3 );

		assert( mms.toString );
		lua_pushliteral( L, "__tostring" );
		lua_pushcfunction( L, mms.toString );
		lua_rawset( L, -3 );

		lua_pushliteral( L, "__coraltype" );
		lua_pushstring( L, ct->getFullName().c_str() );
		lua_rawset( L, -3 );

		// save the metatable in the registry
		lua_pushlightuserdata( L, ct );
		lua_pushvalue( L, -2 );
		lua_rawset( L, LUA_REGISTRYINDEX );

		sm_boundTypes.push_back( ct );
	}
}

bool CompositeTypeBinding::pushMember( lua_State* L, co::ICompositeType* ct, bool mustExist )
{
	// check if member is cached (in the type's metatable)
	lua_getmetatable( L, 1 );
	lua_pushvalue( L, 2 );
	lua_rawget( L, -2 );

	switch( lua_type( L, -1 ) )
	{
	case LUA_TNIL:
		// not in the cache yet
		{
			// pop the nil
			lua_pop( L, 1 );

			// try to find the member
			const char* memberName = lua_tostring( L, 2 );
			co::IMember* member = ct->getMember( memberName );
			if( !member )
			{
				if( mustExist )
				{
					lua_pushliteral( L, "non-existing member '" );
					lua_pushvalue( L, 2 );
					lua_pushliteral( L, "'" );
					lua_concat( L, 3 );
					throw lua::MsgOnStackException();
				}
				else
				{
					lua_pushnil( L );
					return false;
				}
			}

			// push the member
			lua_pushlightuserdata( L, member );
            if( member->getKind() == co::MemberKind::Method )
				lua_pushcclosure( L, callMethod, 1 );

			// cache the member for future use
			lua_pushvalue( L, 2 );
			lua_pushvalue( L, -2 );
			lua_rawset( L, -4 );
		}
		break;

	case LUA_TFUNCTION:
		// got a cached method
		break;

	case LUA_TLIGHTUSERDATA:
		// got a cached non-method (field or interface)
		break;

	default:
		assert( false );
	}

	// remove the metatable
	lua_remove( L, -2 );

	return true;
}

inline co::IField* checkField( lua_State* L, int index )
{
	assert( lua_islightuserdata( L, index ) );
	co::IMember* member = reinterpret_cast<co::IMember*>( lua_touserdata( L, index ) );
	return static_cast<co::IField*>( member );
}

void CompositeTypeBinding::getField( lua_State* L,  const co::Any& instance )
{
	co::IField* field = checkField( L, -1 );
	co::IReflector* reflector = field->getOwner()->getReflector();
	co::AnyValue value;
	reflector->getField( instance, field, value );
	lua_pop( L, 1 );
	LuaState::push( L, value );

	// notify interceptors
	if( instance.getType()->getKind() == co::TK_INTERFACE )
	{
		co::IService* service = instance.get<co::IService*>();
		for( co::Slice<IInterceptor*> r( sm_interceptors ); r; r.popFirst() )
			r.getFirst()->postGetField( service, field, value );
	}
}

void CompositeTypeBinding::setField( lua_State* L, const co::Any& instance )
{
	co::AnyValue value;
	LuaState::get( L, -1, value );

	co::IField* field = checkField( L, -2 );
	value.convert( field->getType() );

	co::IReflector* reflector = field->getOwner()->getReflector();
	reflector->setField( instance, field, value.getAny() );

	lua_pop( L, 2 );

	// notify interceptors
	if( instance.getType()->getKind() == co::TK_INTERFACE )
	{
		co::IService* service = instance.get<co::IService*>();
		for( co::Slice<IInterceptor*> r( sm_interceptors ); r; r.popFirst() )
			r.getFirst()->postSetField( service, field, value );
	}
}

int CompositeTypeBinding::callMethod( lua_State* L )
{
	// get the co::IMethod* from upvalue 1
	co::IMethod* method = static_cast<co::IMethod*>( reinterpret_cast<co::IMember*>(
								lua_touserdata( L, lua_upvalueindex( 1 ) ) ) );

	// get the method's list of parameters
	co::TSlice<co::IParameter*> paramList = method->getParameters();

	/*
		We currently set a hardcoded limit on the number of method parameters.
		Supporting an unbounded number of parameters would degrade performance.
		Generally any method with more than 8 parameters should be refactored.
	 */
	const int MAX_NUM_PARAMETERS = 8;
	int numParams = static_cast<int>( paramList.getSize() );
	if( numParams > MAX_NUM_PARAMETERS )
		luaL_error( L, "method '%s' exceeds the hardcoded limit of %d parameters",
						method->getName().c_str(), MAX_NUM_PARAMETERS );

	__BEGIN_EXCEPTIONS_BARRIER__

	// prepare the argument list
	co::Any args[MAX_NUM_PARAMETERS];
	co::AnyValue values[MAX_NUM_PARAMETERS];

	int numRequiredArgs = numParams;
	int numPassedArgs = lua_gettop( L ) - 1;
	int i = 0;

	try
	{
		for( ; i < numParams; ++i )
		{
			args[i].set( values[i] );

			co::IParameter* paramInfo = paramList[i];
			if( paramInfo->getIsIn() )
			{
				if( numPassedArgs <= i )
				{
					// a required argument was not passed, but we don't break the loop
					// because we want to compute numRequiredArgs
					continue;
				}

				LuaState::get( L, i + 2, args[i] );
			}
			else // only 'out'
			{
				--numRequiredArgs;
			}

			values[i].convert( paramInfo->getType() );
		}
	}
	catch( const co::Exception& e )
	{
		lua_pushfstring( L, "bad argument #%d to method '%s' (%s)",
					i + 1, method->getName().c_str(), e.getMessage().c_str() );
		throw lua::MsgOnStackException();
	}

	// check the number of required/passed parameters
	if( numRequiredArgs > numPassedArgs )
	{
		lua_pushfstring( L, "insufficient number of arguments to method '%s' (%d expected, got %d)",
			method->getName().c_str(), numRequiredArgs, numPassedArgs );
		throw lua::MsgOnStackException();
	}

	// invoke the method
	co::Any instance;
	tryGetInstance( L, 1, instance );

	co::AnyValue returnValue;
	co::Slice<co::Any> argsRange( args, numParams );

	co::IReflector* reflector = method->getOwner()->getReflector();
	reflector->invoke( instance, method, argsRange, returnValue );

	if( instance.getType()->getKind() == co::TK_INTERFACE )
	{
		co::IService* service = instance.get<co::IService*>();
		for( co::Slice<IInterceptor*> r( sm_interceptors ); r; r.popFirst() )
			r.getFirst()->postInvoke( service, method, argsRange, returnValue );
	}

	// return result and output parameters

	int numOut = 0;
	if( method->getReturnType() != NULL )
	{
		++numOut;
		LuaState::push( L, returnValue );
	}

	for( i = 0; i < numParams; ++i )
	{
		if( paramList[i]->getIsOut() )
		{
			++numOut;
			LuaState::push( L, args[i] );
		}
	}

	return numOut;

	__END_EXCEPTIONS_BARRIER__
}

/*****************************************************************************/
/*  Helper class for binding co::IObjects to Lua                             */
/*****************************************************************************/

void ObjectBinding::create( lua_State* L, co::IObject* object )
{
	// create the userdata
	co::IObject** ud = reinterpret_cast<co::IObject**>( lua_newuserdata( L, sizeof(co::IObject*) ) );

	*ud = object;
	object->serviceRetain();

	// set the userdata's metatable
	pushMetatable( L, object->getComponent() );
	lua_setmetatable( L, -2 );

	// notify interceptors
	for( co::Slice<IInterceptor*> r( sm_interceptors ); r; r.popFirst() )
		r.getFirst()->serviceRetained( object );
}

co::IObject* ObjectBinding::getInstance( lua_State* L, int index )
{
	co::IService* s = ServiceBinding::getInstance( L, index );
	assert( s->getInterface() == co::typeOf<co::IObject>::get() );
	return static_cast<co::IObject*>( s );
}

inline co::IPort* checkPort( lua_State* L, int index )
{
	assert( lua_islightuserdata( L, index ) );
	co::IMember* member = reinterpret_cast<co::IMember*>( lua_touserdata( L, index ) );
	return static_cast<co::IPort*>( member );
}

inline bool isStringComponent( lua_State* L, int index )
{
	size_t length;
	const char* str = lua_tolstring( L, index, &length );
	return ( length == 9 && strncmp( str, "component", length ) == 0 );
}

int ObjectBinding::index( lua_State* L )
{
	co::IObject** ud = reinterpret_cast<co::IObject**>( lua_touserdata( L, 1 ) );
	assert( ud );
	assert( lua_isstring( L, 2 ) );

	__BEGIN_EXCEPTIONS_BARRIER__

	co::IObject* object = *ud;

	if( isStringComponent( L, 2 ) )
	{
		LuaState::push( L, object->getComponent() );
		return 1;
	}

	if( pushMember( L, object->getComponent() ) )
	{
		assert( lua_islightuserdata( L, -1 ) );
		co::IPort* port = checkPort( L, -1 );
		co::IService* service = object->getServiceAt( port );
		LuaState::push( L, service );

		// notify interceptors
		for( co::Slice<IInterceptor*> r( sm_interceptors ); r; r.popFirst() )
			r.getFirst()->postGetService( object, port, service );
	}

	return 1;

	__END_EXCEPTIONS_BARRIER__
}

int ObjectBinding::newIndex( lua_State* L )
{
	co::IObject** ud = reinterpret_cast<co::IObject**>( lua_touserdata( L, 1 ) );
	assert( ud );
	assert( lua_isstring( L, 2 ) );

	__BEGIN_EXCEPTIONS_BARRIER__

	if( isStringComponent( L, 2 ) )
		throw co::IllegalArgumentException( "'component' is a facet and cannot be set" );

	co::IObject* object = *ud;
	pushMember( L, object->getComponent(), true );
	if( lua_islightuserdata( L, -1 ) )
	{
		co::IPort* port = checkPort( L, -1 );
		if( port->getIsFacet() )
		{
			lua_pushliteral( L, "'" );
			lua_pushvalue( L, 2 );
			lua_pushliteral( L, "' is a facet and cannot be set" );
			lua_concat( L, 3 );
			throw lua::MsgOnStackException();
		}

		co::AnyValue value;
		LuaState::get( L, 3, value );
	
		co::RefPtr<co::IService> service;
		value.get( service );

		co::ensureIsA( service.get(), port->getType() );
		object->setServiceAt( port, service.get() );

		// notify interceptors
		for( co::Slice<IInterceptor*> r( sm_interceptors ); r; r.popFirst() )
			r.getFirst()->postSetService( object, port, service.get() );
	}

	return 0;

	__END_EXCEPTIONS_BARRIER__
}

int ObjectBinding::gc( lua_State* L )
{
	co::IObject** ud = reinterpret_cast<co::IObject**>( lua_touserdata( L, 1 ) );
	assert( ud );

	co::IObject* object = *ud;

	__BEGIN_EXCEPTIONS_BARRIER__

	object->serviceRelease();

	// notify interceptors
	for( co::Slice<IInterceptor*> r( sm_interceptors ); r; r.popFirst() )
		r.getFirst()->serviceReleased( object );

	return 0;

	__END_EXCEPTIONS_BARRIER__
}

int ObjectBinding::toString( lua_State* L )
{
	co::IObject** ud = reinterpret_cast<co::IObject**>( lua_touserdata( L, 1 ) );
	assert( ud );

	co::IObject* comp = *ud;
	lua_pushfstring( L, "%s: %p", comp->getComponent()->getFullName().c_str(), comp );

	return 1;
}

/*****************************************************************************/
/*  Helper class for binding co::Interfaces to Lua                           */
/*****************************************************************************/

void ServiceBinding::create( lua_State* L, co::IService* service )
{
	// create the userdata
	co::IService** ud = reinterpret_cast<co::IService**>( lua_newuserdata( L, sizeof(co::IService*) ) );

	*ud = service;
	service->serviceRetain();

	// set the userdata's metatable
	pushMetatable( L, service->getInterface() );
	lua_setmetatable( L, -2 );

	// notify interceptors
	for( co::Slice<IInterceptor*> r( sm_interceptors ); r; r.popFirst() )
		r.getFirst()->serviceRetained( service );
}

co::IService* ServiceBinding::getInstance( lua_State* L, int index )
{
	return *reinterpret_cast<co::IService**>( lua_touserdata( L, index ) );
}

int ServiceBinding::index( lua_State* L )
{
	co::IService** ud = reinterpret_cast<co::IService**>( lua_touserdata( L, 1 ) );
	assert( ud );
	assert( lua_isstring( L, 2 ) );

	__BEGIN_EXCEPTIONS_BARRIER__

	co::IService* service = *ud;
	if( pushMember( L, service->getInterface() ) )
		if( lua_islightuserdata( L, -1 ) )
			getField( L, service );

	return 1;

	__END_EXCEPTIONS_BARRIER__
}

int ServiceBinding::newIndex( lua_State* L )
{
	co::IService** ud = reinterpret_cast<co::IService**>( lua_touserdata( L, 1 ) );
	assert( ud );
	assert( lua_isstring( L, 2 ) );

	__BEGIN_EXCEPTIONS_BARRIER__

	co::IService* service = *ud;
	pushMember( L, service->getInterface(), true );
	if( lua_isfunction( L, -1 ) )
	{
		lua_pushliteral( L, "'" );
		lua_pushvalue( L, 2 );
		lua_pushliteral( L, "' is a method and cannot be overwritten" );
		lua_concat( L, 3 );
		throw lua::MsgOnStackException();
	}

	lua_pushvalue( L, 3 );
	setField( L, service );

	return 0;

	__END_EXCEPTIONS_BARRIER__
}

int ServiceBinding::gc( lua_State* L )
{
	co::IService** ud = reinterpret_cast<co::IService**>( lua_touserdata( L, 1 ) );
	assert( ud );

	co::IService* service = *ud;

	__BEGIN_EXCEPTIONS_BARRIER__

	// notify interceptors
	for( co::Slice<IInterceptor*> r( sm_interceptors ); r; r.popFirst() )
		r.getFirst()->serviceReleased( service );

	service->serviceRelease();

	return 0;

	__END_EXCEPTIONS_BARRIER__
}

int ServiceBinding::toString( lua_State* L )
{
	co::IService** ud = reinterpret_cast<co::IService**>( lua_touserdata( L, 1 ) );
	assert( ud );

	co::IService* service = *ud;
	lua_pushfstring( L, "%s: %p", service->getInterface()->getFullName().c_str(), service );

	return 1;
}

/*****************************************************************************/
/*  Re-usable class for binding complex values to Lua                        */
/*****************************************************************************/

void ValueBinding::push( lua_State* L, co::IType* type, void* instancePtr )
{
	assert( co::isComplexValue( type->getKind() ) );

	co::IReflector* reflector = type->getReflector();
	reflector->serviceRetain();

	// create the userdata
	void* ud = lua_newuserdata( L, reflector->getSize() );
	reflector->createValues( ud, 1 );

	// if an instance was provided, copy it
	if( instancePtr )
		reflector->copyValues( instancePtr, ud, 1 );

	// set the userdata's metatable
	pushMetatable( L, static_cast<co::ICompositeType*>( type ), reflector );
	lua_setmetatable( L, -2 );
}

co::Any ValueBinding::getInstance( lua_State* L, int index )
{
	co::ICompositeType* ct = getType( L, index );
	assert( ct && co::isValue( ct->getKind() ) );
	return co::Any( false, ct, lua_touserdata( L, index ) );
}

int ValueBinding::gc( lua_State* L )
{
	__BEGIN_EXCEPTIONS_BARRIER__

	lua_getmetatable( L, 1 );
	lua_rawgeti( L, -1, 2 );
	assert( lua_islightuserdata( L, -1 ) );

	co::IReflector* reflector = reinterpret_cast<co::IReflector*>( lua_touserdata( L, -1 ) );
	reflector->destroyValues( lua_touserdata( L, 1 ), 1 );
	reflector->serviceRelease();

	return 0;

	__END_EXCEPTIONS_BARRIER__
}

int ValueBinding::toString( lua_State* L )
{
	co::ICompositeType* ct = getType( L, 1 );
	assert( ct );

	lua_pushfstring( L, "%s: %p", ct->getFullName().c_str(), lua_touserdata( L, 1 ) );

	return 1;
}

/*****************************************************************************/
/*  Helper class for binding native classes to Lua                           */
/*****************************************************************************/

int ClassBinding::index( lua_State* L )
{
	assert( lua_isstring( L, 2 ) );

	__BEGIN_EXCEPTIONS_BARRIER__

	co::ICompositeType* ct = getType( L, 1 );
	assert( ct );

	if( pushMember( L, ct ) && lua_islightuserdata( L, -1 ) )
		getField( L, co::Any( false, ct, lua_touserdata( L, 1 ) ) );

	return 1;

	__END_EXCEPTIONS_BARRIER__
}

int ClassBinding::newIndex( lua_State* L )
{
	assert( lua_isstring( L, 2 ) );

	__BEGIN_EXCEPTIONS_BARRIER__

	co::ICompositeType* ct = getType( L, 1 );
	assert( ct );

	pushMember( L, ct, true );
	if( lua_isfunction( L, -1 ) )
	{
		lua_pushliteral( L, "'" );
		lua_pushvalue( L, 2 );
		lua_pushliteral( L, "' is a method and cannot be overwritten" );
		lua_concat( L, 3 );
		throw lua::MsgOnStackException();
	}

	lua_pushvalue( L, 3 );
	setField( L, co::Any( false, ct, lua_touserdata( L, 1 ) ) );

	return 0;

	__END_EXCEPTIONS_BARRIER__
}

/*****************************************************************************/
/*  Helper class for binding structs to Lua                                  */
/*****************************************************************************/

int StructBinding::index( lua_State* L )
{
	assert( lua_isstring( L, 2 ) );

	__BEGIN_EXCEPTIONS_BARRIER__

	co::ICompositeType* ct = getType( L, 1 );
	assert( ct );

	if( pushMember( L, ct ) )
	{
		assert( lua_islightuserdata( L, -1 ) );
		getField( L, co::Any( false, ct, lua_touserdata( L, 1 ) ) );
	}

	return 1;

	__END_EXCEPTIONS_BARRIER__
}

int StructBinding::newIndex( lua_State* L )
{
	assert( lua_isstring( L, 2 ) );

	__BEGIN_EXCEPTIONS_BARRIER__

	co::ICompositeType* ct = getType( L, 1 );
	assert( ct );

	pushMember( L, ct, true );
	assert( lua_islightuserdata( L, -1 ) );

	lua_pushvalue( L, 3 );
	setField( L, co::Any( false, ct, lua_touserdata( L, 1 ) ) );

	return 0;

	__END_EXCEPTIONS_BARRIER__
}

} // namespace lua
