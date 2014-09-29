
#include "NativeClass_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace co {

//------ co.NativeClass has a facet named 'type', of type co.INativeClass ------//

co::IInterface* NativeClass_co_INativeClass::getInterface()
{
	return co::typeOf<co::INativeClass>::get();
}

co::IPort* NativeClass_co_INativeClass::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "co.NativeClass" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "type" ) );
	assert( facet );
	return facet;
}

//------ NativeClass_Base ------//

NativeClass_Base::NativeClass_Base()
{
	// empty
}

NativeClass_Base::~NativeClass_Base()
{
	// empty
}

co::IObject* NativeClass_Base::getProvider()
{
	return this;
}

void NativeClass_Base::serviceRetain()
{
	incrementRefCount();
}

void NativeClass_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* NativeClass_Base::getComponent()
{
	co::IType* type = co::getType( "co.NativeClass" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* NativeClass_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<co::INativeClass*>( this ); break;
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void NativeClass_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
}

} // namespace co
