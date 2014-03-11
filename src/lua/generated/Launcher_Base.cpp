#include "Launcher_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace lua {

void moduleRetain();
void moduleRelease();

//------ lua.Launcher has a facet named 'launcher', of type lua.ILauncher ------//

co::IInterface* Launcher_lua_ILauncher::getInterface()
{
	return co::typeOf<lua::ILauncher>::get();
}

co::IPort* Launcher_lua_ILauncher::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "lua.Launcher" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "launcher" ) );
	assert( facet );
	return facet;
}

//------ Launcher_Base ------//

Launcher_Base::Launcher_Base()
{
	moduleRetain();
}

Launcher_Base::~Launcher_Base()
{
	moduleRelease();
}

co::IObject* Launcher_Base::getProvider()
{
	return this;
}

void Launcher_Base::serviceRetain()
{
	incrementRefCount();
}

void Launcher_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* Launcher_Base::getComponent()
{
	co::IType* type = co::getType( "lua.Launcher" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* Launcher_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<lua::ILauncher*>( this ); break;
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void Launcher_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
}

} // namespace lua
