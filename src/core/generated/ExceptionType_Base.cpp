#include "ExceptionType_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace co {

//------ co.ExceptionType has a facet named 'type', of type co.IException ------//

co::IInterface* ExceptionType_co_IException::getInterface()
{
	return co::typeOf<co::IException>::get();
}

co::IPort* ExceptionType_co_IException::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "co.ExceptionType" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "type" ) );
	assert( facet );
	return facet;
}

//------ ExceptionType_Base ------//

ExceptionType_Base::ExceptionType_Base()
{
	// empty
}

ExceptionType_Base::~ExceptionType_Base()
{
	// empty
}

co::IObject* ExceptionType_Base::getProvider()
{
	return this;
}

void ExceptionType_Base::serviceRetain()
{
	incrementRefCount();
}

void ExceptionType_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* ExceptionType_Base::getComponent()
{
	co::IType* type = co::getType( "co.ExceptionType" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* ExceptionType_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<co::IException*>( this ); break;
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void ExceptionType_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
}

} // namespace co
