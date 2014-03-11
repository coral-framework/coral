#include "TypeTransaction_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace co {

//------ co.TypeTransaction has a facet named 'transaction', of type co.ITypeTransaction ------//

co::IInterface* TypeTransaction_co_ITypeTransaction::getInterface()
{
	return co::typeOf<co::ITypeTransaction>::get();
}

co::IPort* TypeTransaction_co_ITypeTransaction::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "co.TypeTransaction" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "transaction" ) );
	assert( facet );
	return facet;
}

//------ TypeTransaction_Base ------//

TypeTransaction_Base::TypeTransaction_Base()
{
	// empty
}

TypeTransaction_Base::~TypeTransaction_Base()
{
	// empty
}

co::IObject* TypeTransaction_Base::getProvider()
{
	return this;
}

void TypeTransaction_Base::serviceRetain()
{
	incrementRefCount();
}

void TypeTransaction_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* TypeTransaction_Base::getComponent()
{
	co::IType* type = co::getType( "co.TypeTransaction" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* TypeTransaction_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<co::ITypeTransaction*>( this ); break;
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void TypeTransaction_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
}

} // namespace co
