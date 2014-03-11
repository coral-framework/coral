#include "Component_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace co {

//------ co.Component has a facet named 'type', of type co.IComponent ------//

co::IInterface* Component_co_IComponent::getInterface()
{
	return co::typeOf<co::IComponent>::get();
}

co::IPort* Component_co_IComponent::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "co.Component" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "type" ) );
	assert( facet );
	return facet;
}

//------ Component_Base ------//

Component_Base::Component_Base()
{
	// empty
}

Component_Base::~Component_Base()
{
	// empty
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
	co::IType* type = co::getType( "co.Component" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* Component_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<co::IComponent*>( this ); break;
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

} // namespace co
