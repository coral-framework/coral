#include "LuaComponent.h"
#include "LuaBinding.h"
#include <co/IMethod.h>
#include <co/IParameter.h>
#include <co/IField.h>
#include <co/IPort.h>
#include <co/NotSupportedException.h>
#include <lua/Exception.h>
#include <cctype>
#include <cassert>
#include <sstream>

namespace lua {

/*
	Helper macros to get a lua_State* L and protect
	the Lua stack from eventual exceptions.
 */
#define __BEGIN_LUA_API_CODE__ \
	lua_State* L = LuaState::getL(); \
	int originalTop = lua_gettop( L ); \
	try {

#define __END_LUA_API_CODE__ \
	} catch( ... ) { lua_settop( L, originalTop ); throw; } \
	lua_settop( L, originalTop );


//------ LuaComponent ---------------------------------------------------------

LuaComponent::LuaComponent()
{
	_componentType = NULL;
	_facets = NULL;
	_numFacets = 0;
	_tableRef = LUA_NOREF;
}

LuaComponent::~LuaComponent()
{
	for( int i = 0; i < _numFacets; ++i )
		delete _facets[i];
	delete[] _facets;
}

void LuaComponent::setComponent( co::IComponent* ct, int prototypeTableRef )
{
	assert( ct != NULL && _componentType == NULL && _facets == NULL );

	_componentType = ct;
	_tableRef = prototypeTableRef;

	// register ourself as the type's reflector
	_componentType->setReflector( this );
}

void LuaComponent::releaseComponent()
{
	lua_State* L = LuaState::getL();
	luaL_unref( L, LUA_REGISTRYINDEX, _tableRef );
	_tableRef = 0;

	_componentType->setReflector( NULL );
}

void LuaComponent::setInstance( LuaComponent* prototype, int instanceTableRef )
{
	assert( prototype && prototype->_facets == NULL );

	_componentType = prototype->_componentType;
	_tableRef = instanceTableRef;

	// create proxy interfaces for our facets
	co::TSlice<co::IPort*> facets = _componentType->getFacets();
	int numFacets = static_cast<int>( facets.getSize() );
	_facets = new co::IService*[numFacets];
	for( int i = 0; i < numFacets; ++i )
	{
		/*
			To avoid having exceptions raised here by getReflector(), we should
			check all interface reflectors before instantiating the component.
		 */
		assert( _numFacets == i );
		facets[i]->getType()->getReflector()->newDynamicProxy( this );
		assert( _numFacets == i + 1 );
	}
}

co::IComponent* LuaComponent::getComponent()
{
	return _componentType ? _componentType : lua::Component_Base::getComponent();
}

co::IService* LuaComponent::getServiceAt( co::IPort* port )
{
	co::ICompositeType* owner = port->getOwner();
	if( owner == _componentType )
		return dynamicGetService( port );
	return lua::Component_Base::getServiceAt( port );
}

void LuaComponent::setServiceAt( co::IPort* receptacle, co::IService* instance )
{
	co::ICompositeType* owner = receptacle->getOwner();
	if( owner == _componentType )
		dynamicSetService( receptacle, instance );
	else
		lua::Component_Base::setServiceAt( receptacle, instance );
}

co::int32 LuaComponent::dynamicRegisterService( co::IService* proxy )
{
	_facets[_numFacets] = proxy;
	return _numFacets++;
}

co::IPort* LuaComponent::getFacetByCookie( co::int32 cookie )
{
	return _componentType->getFacets()[cookie];
}

co::IPort* LuaComponent::dynamicGetFacet( co::int32 cookie )
{
	return getFacetByCookie( cookie );
}

void LuaComponent::pushFacetTable( lua_State* L, co::int32 cookie )
{
	lua_rawgeti( L, LUA_REGISTRYINDEX, _tableRef );
	lua_pushstring( L, getFacetByCookie( cookie )->getName().c_str() );
	lua_gettable( L, -2 );
	assert( lua_istable( L, -1 ) );
}

void LuaComponent::pushAccessorName( lua_State* L, const char* prefix, const std::string& fieldName, const char* suffix )
{
	char buffer[256];
	char* p = buffer;
	while( *prefix )
		*p++ = *prefix++;
	assert( !fieldName.empty() && fieldName.length() < sizeof(buffer) - ( p - buffer ) );
	const char* str = fieldName.c_str();
	*p++ = toupper( *str++ );
	while( *str )
		*p++ = *str++;
	if( suffix )
		while( *suffix )
			*p++ = *suffix++;
	lua_pushlstring( L, buffer, p - buffer );
}

void LuaComponent::getMethod( lua_State* L, int t, co::int32 cookie )
{
	const char* methodName = lua_tostring( L, -1 );
	lua_gettable( L, t );
	if( lua_isnil( L, -1 ) )
	{
		std::stringstream ss;
		ss << "missing method '" << methodName << "' in LuaComponent '" << _componentType->getFullName() << "'";
		if( cookie != -1 )
			ss << ", facet '" << getFacetByCookie( cookie )->getName() << "'";
		throw lua::Exception( ss.str() );
	}
	assert( lua_isfunction( L, -1 ) );
}

void LuaComponent::dynamicGetField( co::int32 cookie, co::IField* ai, const co::Any& var )
{
	__BEGIN_LUA_API_CODE__

	pushFacetTable( L, cookie );
	pushAccessorName( L, "get", ai->getName() );
	getMethod( L, -2, cookie );
	lua_pushvalue( L, -3 ); // push the 'self' argument
	LuaState::call( L, 1, 1 );
	LuaState::get( L, -1, var );

	__END_LUA_API_CODE__
}

void LuaComponent::dynamicSetField( co::int32 cookie, co::IField* ai, const co::Any& var )
{
	__BEGIN_LUA_API_CODE__

	pushFacetTable( L, cookie );
	pushAccessorName( L, "set", ai->getName() );
	getMethod( L, -2, cookie );
	lua_pushvalue( L, -3 ); // push the 'self' argument
	LuaState::push( L, var );
	LuaState::call( L, 2, 0 );

	__END_LUA_API_CODE__
}

void LuaComponent::dynamicInvoke( co::int32 cookie, co::IMethod* mi, co::Slice<co::Any> args, const co::Any& res )
{
	__BEGIN_LUA_API_CODE__

	pushFacetTable( L, cookie );
	lua_pushstring( L, mi->getName().c_str() );
	getMethod( L, -2, cookie );
	lua_pushvalue( L, -3 ); // push the 'self' argument

	co::IType* returnType = mi->getReturnType();
	int numOut = returnType ? 1 : 0;
	int numIn = 1;

	co::TSlice<co::IParameter*> paramList = mi->getParameters();
	assert( paramList.getSize() == args.getSize() );

	int numArgs = static_cast<int>( args.getSize() );
	for( int i = 0; i < numArgs; ++i )
	{
		co::IParameter* param = paramList[i];
		if( param->getIsIn() )
		{
			LuaState::push( L, args[i] );
			++numIn;
		}
		if( param->getIsOut() )
			++numOut;
	}

	LuaState::call( L, numIn, numOut );

	// get the function result, if needed
	int outIdx = lua_gettop( L ) + 1 - numOut;
	if( returnType )
	{
		try
		{
			LuaState::get( L, outIdx++, res );
		}
		catch( std::exception& e )
		{
			CORAL_THROW( lua::Exception, "bad result for method '" << mi->getName() << "' (" << e.what() << ")" );
		}
	}

	// get other output values
	int i = 0;
	try
	{
		for( ; i < numArgs; ++i )
		{
			if( paramList[i]->getIsOut() )
				LuaState::get( L, outIdx++, args[i] );
		}
	}
	catch( std::exception& e )
	{
		CORAL_THROW( lua::Exception, "bad result for parameter #" << i + 1
			<< " in method '" << mi->getName() << "' (" << e.what() << ")" );
	}

	__END_LUA_API_CODE__
}

co::uint32 LuaComponent::getSize()
{
	return sizeof(LuaComponent);
}

co::IType* LuaComponent::getType()
{
	return _componentType;
}

void LuaComponent::createValues( void*, size_t )
{
	raiseNotSupportedException();
}

void LuaComponent::copyValues( const void*, void*, size_t )
{
	raiseNotSupportedException();
}

void LuaComponent::destroyValues( void*, size_t )
{
	raiseNotSupportedException();
}

bool LuaComponent::compareValues( const void*, const void*, size_t )
{
	raiseNotSupportedException();
	return false;
}

co::IObject* LuaComponent::newInstance()
{
	assert( _facets == NULL && _tableRef != LUA_NOREF );

	co::IObject* instance;
	__BEGIN_LUA_API_CODE__

	// call the component prototype table to produce a new instance
	lua_rawgeti( L, LUA_REGISTRYINDEX, _tableRef );
	lua_getmetatable( L, -1 );
	lua_pushliteral( L, "__call" );
	lua_rawget( L, -2 ); // get the ComponentMT.__call function
	lua_pushvalue( L, -3 ); // pass the component's proto table as argument

	LuaState::call( L, 1, 1 );

	// extract the returned co::IObject
	instance = ObjectBinding::getInstance( L, -1 );

	__END_LUA_API_CODE__

	return instance;
}

co::IService* LuaComponent::newDynamicProxy( co::IDynamicServiceProvider* )
{
	raiseNotSupportedException();
	return NULL;
}

void LuaComponent::getField( const co::Any&, co::IField*, const co::Any& )
{
	raiseNotSupportedException();
}

void LuaComponent::setField( const co::Any&, co::IField*, const co::Any& )
{
	raiseNotSupportedException();
}

void LuaComponent::invoke( const co::Any&, co::IMethod*, co::Slice<co::Any>, const co::Any& )
{
	raiseNotSupportedException();
}

void LuaComponent::raise( const std::string& )
{
	raiseNotSupportedException();
}

co::IService* LuaComponent::dynamicGetService( co::IPort* port )
{
	if( port->getIsFacet() )
		return _facets[port->getIndex()];

	// call the Lua component to get a client interface
	co::RefPtr<co::IService> service;
	__BEGIN_LUA_API_CODE__

	lua_rawgeti( L, LUA_REGISTRYINDEX, _tableRef );
	pushAccessorName( L, "get", port->getName(), "Service" );
	getMethod( L, -2 );
	lua_pushvalue( L, -2 ); // push the 'self' argument
	LuaState::call( L, 1, 1 );

	LuaState::get( L, -1, service );

	__END_LUA_API_CODE__

	// check interface compatibility
	co::ensureIsA( service.get(), port->getType() );
	return service.get();
}

void LuaComponent::dynamicSetService( co::IPort* receptacle, co::IService* instance )
{
	// check interface compatibility
	co::ensureIsA( instance, receptacle->getType() );

	__BEGIN_LUA_API_CODE__

	lua_rawgeti( L, LUA_REGISTRYINDEX, _tableRef );
	pushAccessorName( L, "set", receptacle->getName(), "Service" );
	getMethod( L, -2 );
	lua_pushvalue( L, -2 ); // push the 'self' argument
	LuaState::push( L, instance );
	LuaState::call( L, 2, 0 );

	__END_LUA_API_CODE__
}

void LuaComponent::raiseNotSupportedException()
{
	throw co::NotSupportedException( "operation not supported by this reflector" );
}

CORAL_EXPORT_COMPONENT( LuaComponent, Component );

} // namespace lua
