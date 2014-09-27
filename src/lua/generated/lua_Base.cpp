
#include "lua_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace lua {

void moduleRetain();
void moduleRelease();

//------ lua.lua has a facet named 'part', of type co.IModulePart ------//

co::IInterface* lua_co_IModulePart::getInterface()
{
	return co::typeOf<co::IModulePart>::get();
}

co::IPort* lua_co_IModulePart::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "lua.lua" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "part" ) );
	assert( facet );
	return facet;
}

//------ lua_Base ------//

lua_Base::lua_Base()
{
	moduleRetain();
}

lua_Base::~lua_Base()
{
	moduleRelease();
}

co::IObject* lua_Base::getProvider()
{
	return this;
}

void lua_Base::serviceRetain()
{
	incrementRefCount();
}

void lua_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* lua_Base::getComponent()
{
	co::IType* type = co::getType( "lua.lua" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* lua_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<co::IModulePart*>( this ); break;
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void lua_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
}

} // namespace lua
