
#include "ScriptedAnnotation_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace lua {

void moduleRetain();
void moduleRelease();

//------ lua.ScriptedAnnotation has a facet named 'annotation', of type lua.IScripted ------//

co::IInterface* ScriptedAnnotation_lua_IScripted::getInterface()
{
	return co::typeOf<lua::IScripted>::get();
}

co::IPort* ScriptedAnnotation_lua_IScripted::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "lua.ScriptedAnnotation" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "annotation" ) );
	assert( facet );
	return facet;
}

//------ ScriptedAnnotation_Base ------//

ScriptedAnnotation_Base::ScriptedAnnotation_Base()
{
	moduleRetain();
}

ScriptedAnnotation_Base::~ScriptedAnnotation_Base()
{
	moduleRelease();
}

co::IObject* ScriptedAnnotation_Base::getProvider()
{
	return this;
}

void ScriptedAnnotation_Base::serviceRetain()
{
	incrementRefCount();
}

void ScriptedAnnotation_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* ScriptedAnnotation_Base::getComponent()
{
	co::IType* type = co::getType( "lua.ScriptedAnnotation" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* ScriptedAnnotation_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<lua::IScripted*>( this ); break;
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void ScriptedAnnotation_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
}

} // namespace lua
