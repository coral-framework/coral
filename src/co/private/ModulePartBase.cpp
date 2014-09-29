#include "ModulePartBase.h"
#include <co/Coral.h>
#include <co/IModule.h>
#include <co/RefPtr.h>
#include <co/Exception.h>
#include <co/INamespace.h>
#include <co/ITypeBuilder.h>
#include <co/IPort.h>
#include <co/IInterface.h>
#include <co/ITypeTransaction.h>

namespace co {

//------ ModulePartBase provides a facet named 'part', of type co::IModulePart ------//

IInterface* ModulePartBase_co_ModulePart::getInterface()
{
	return co::typeOf<IModulePart>::get();
}

IPort* ModulePartBase_co_ModulePart::getFacet()
{
	IPort* facet = static_cast<IPort*>( typeOf<IModulePart>::get()->getMember( "part" ) );
	assert( facet );
	return facet;
}

//------ ModulePartBase ------//

ModulePartBase::ModulePartBase()
{
	// empty
}

ModulePartBase::~ModulePartBase()
{
	// empty
}

IObject* ModulePartBase::getProvider()
{
	return this;
}

void ModulePartBase::serviceRetain()
{
	incrementRefCount();
}

void ModulePartBase::serviceRelease()
{
	decrementRefCount();
}

IComponent* ModulePartBase::getComponent()
{
	return getOrCreateInternalComponent( "co.ModulePartBase", "co.IModulePart", "part" );
}

IService* ModulePartBase::getServiceAt( IPort* port )
{
	checkValidPort( port );

	IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0: res = static_cast<IModulePart*>( this ); break;
	default:
		raiseUnexpectedPortIndex();
	}

	return res;
}

void ModulePartBase::setServiceAt( IPort* receptacle, IService* )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
}

void ModulePartBase::initialize( co::IModule* )
{
	// empty
}

void ModulePartBase::integrate( co::IModule* )
{
	// empty
}

void ModulePartBase::integratePresentation( co::IModule* )
{
	// empty
}

void ModulePartBase::disintegrate( co::IModule* )
{
	// empty
}

void ModulePartBase::dispose( co::IModule* )
{
	// empty
}

} // namespace co
