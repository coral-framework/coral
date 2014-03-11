#include "Namespace_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace co {

//------ co.Namespace has a facet named 'namespace', of type co.INamespace ------//

co::IInterface* Namespace_co_INamespace::getInterface()
{
	return co::typeOf<co::INamespace>::get();
}

co::IPort* Namespace_co_INamespace::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "co.Namespace" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "namespace" ) );
	assert( facet );
	return facet;
}

//------ Namespace_Base ------//

Namespace_Base::Namespace_Base()
{
	// empty
}

Namespace_Base::~Namespace_Base()
{
	// empty
}

co::IObject* Namespace_Base::getProvider()
{
	return this;
}

void Namespace_Base::serviceRetain()
{
	incrementRefCount();
}

void Namespace_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* Namespace_Base::getComponent()
{
	co::IType* type = co::getType( "co.Namespace" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* Namespace_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<co::INamespace*>( this ); break;
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void Namespace_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
}

} // namespace co
