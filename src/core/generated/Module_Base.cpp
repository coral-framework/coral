#include "Module_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace co {

//------ co.Module has a facet named 'module', of type co.IModule ------//

co::IInterface* Module_co_IModule::getInterface()
{
	return co::typeOf<co::IModule>::get();
}

co::IPort* Module_co_IModule::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "co.Module" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "module" ) );
	assert( facet );
	return facet;
}

//------ Module_Base ------//

Module_Base::Module_Base()
{
	// empty
}

Module_Base::~Module_Base()
{
	// empty
}

co::IObject* Module_Base::getProvider()
{
	return this;
}

void Module_Base::serviceRetain()
{
	incrementRefCount();
}

void Module_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* Module_Base::getComponent()
{
	co::IType* type = co::getType( "co.Module" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* Module_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<co::IModule*>( this ); break;
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void Module_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
}

} // namespace co
