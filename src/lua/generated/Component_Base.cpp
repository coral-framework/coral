
#include "Component_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace lua {

void moduleRetain();
void moduleRelease();

//------ lua.Component has a facet named 'dynamicServiceProvider', of type co.IDynamicServiceProvider ------//

co::IInterface* Component_co_IDynamicServiceProvider::getInterface()
{
	return co::typeOf<co::IDynamicServiceProvider>::get();
}

co::IPort* Component_co_IDynamicServiceProvider::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "lua.Component" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "dynamicServiceProvider" ) );
	assert( facet );
	return facet;
}

//------ lua.Component has a facet named 'reflector', of type co.IReflector ------//

co::IInterface* Component_co_IReflector::getInterface()
{
	return co::typeOf<co::IReflector>::get();
}

co::IPort* Component_co_IReflector::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "lua.Component" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "reflector" ) );
	assert( facet );
	return facet;
}

//------ Component_Base ------//

Component_Base::Component_Base()
{
	moduleRetain();
}

Component_Base::~Component_Base()
{
	moduleRelease();
}

co::IObject* Component_Base::getProvider()
{
	return this;
}

void Component_Base::serviceRetain()
{
	incrementRefCount();
}

void Component_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* Component_Base::getComponent()
{
	co::IType* type = co::getType( "lua.Component" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* Component_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<co::IDynamicServiceProvider*>( this ); break;
	case 1:		res = static_cast<co::IReflector*>( this ); break;
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void Component_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
}

} // namespace lua
