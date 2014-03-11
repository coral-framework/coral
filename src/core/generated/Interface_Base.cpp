#include "Interface_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace co {

//------ co.Interface has a facet named 'type', of type co.IInterface ------//

co::IInterface* Interface_co_IInterface::getInterface()
{
	return co::typeOf<co::IInterface>::get();
}

co::IPort* Interface_co_IInterface::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "co.Interface" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "type" ) );
	assert( facet );
	return facet;
}

//------ Interface_Base ------//

Interface_Base::Interface_Base()
{
	// empty
}

Interface_Base::~Interface_Base()
{
	// empty
}

co::IObject* Interface_Base::getProvider()
{
	return this;
}

void Interface_Base::serviceRetain()
{
	incrementRefCount();
}

void Interface_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* Interface_Base::getComponent()
{
	co::IType* type = co::getType( "co.Interface" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* Interface_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<co::IInterface*>( this ); break;
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void Interface_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
}

} // namespace co
