
#include "Parameter_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace co {

//------ co.Parameter has a facet named 'parameter', of type co.IParameter ------//

co::IInterface* Parameter_co_IParameter::getInterface()
{
	return co::typeOf<co::IParameter>::get();
}

co::IPort* Parameter_co_IParameter::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "co.Parameter" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "parameter" ) );
	assert( facet );
	return facet;
}

//------ Parameter_Base ------//

Parameter_Base::Parameter_Base()
{
	// empty
}

Parameter_Base::~Parameter_Base()
{
	// empty
}

co::IObject* Parameter_Base::getProvider()
{
	return this;
}

void Parameter_Base::serviceRetain()
{
	incrementRefCount();
}

void Parameter_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* Parameter_Base::getComponent()
{
	co::IType* type = co::getType( "co.Parameter" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* Parameter_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<co::IParameter*>( this ); break;
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void Parameter_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
}

} // namespace co
