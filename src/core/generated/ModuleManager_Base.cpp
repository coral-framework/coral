#include "ModuleManager_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace co {

//------ co.ModuleManager has a facet named 'moduleManager', of type co.IModuleManager ------//

co::IInterface* ModuleManager_co_IModuleManager::getInterface()
{
	return co::typeOf<co::IModuleManager>::get();
}

co::IPort* ModuleManager_co_IModuleManager::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "co.ModuleManager" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "moduleManager" ) );
	assert( facet );
	return facet;
}

//------ ModuleManager_Base ------//

ModuleManager_Base::ModuleManager_Base()
{
	// empty
}

ModuleManager_Base::~ModuleManager_Base()
{
	// empty
}

co::IObject* ModuleManager_Base::getProvider()
{
	return this;
}

void ModuleManager_Base::serviceRetain()
{
	incrementRefCount();
}

void ModuleManager_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* ModuleManager_Base::getComponent()
{
	co::IType* type = co::getType( "co.ModuleManager" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* ModuleManager_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<co::IModuleManager*>( this ); break;
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void ModuleManager_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
}

} // namespace co
