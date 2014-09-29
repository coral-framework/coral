
#include "Method_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace co {

//------ co.Method has a facet named 'methodInfo', of type co.IMethod ------//

co::IInterface* Method_co_IMethod::getInterface()
{
	return co::typeOf<co::IMethod>::get();
}

co::IPort* Method_co_IMethod::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "co.Method" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "methodInfo" ) );
	assert( facet );
	return facet;
}

//------ Method_Base ------//

Method_Base::Method_Base()
{
	// empty
}

Method_Base::~Method_Base()
{
	// empty
}

co::IObject* Method_Base::getProvider()
{
	return this;
}

void Method_Base::serviceRetain()
{
	incrementRefCount();
}

void Method_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* Method_Base::getComponent()
{
	co::IType* type = co::getType( "co.Method" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* Method_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<co::IMethod*>( this ); break;
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void Method_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
}

} // namespace co
