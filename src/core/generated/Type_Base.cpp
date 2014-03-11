#include "Type_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace co {

//------ co.Type has a facet named 'type', of type co.IType ------//

co::IInterface* Type_co_IType::getInterface()
{
	return co::typeOf<co::IType>::get();
}

co::IPort* Type_co_IType::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "co.Type" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "type" ) );
	assert( facet );
	return facet;
}

//------ Type_Base ------//

Type_Base::Type_Base()
{
	// empty
}

Type_Base::~Type_Base()
{
	// empty
}

co::IObject* Type_Base::getProvider()
{
	return this;
}

void Type_Base::serviceRetain()
{
	incrementRefCount();
}

void Type_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* Type_Base::getComponent()
{
	co::IType* type = co::getType( "co.Type" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* Type_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<co::IType*>( this ); break;
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void Type_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
}

} // namespace co
