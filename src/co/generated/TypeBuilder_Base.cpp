
#include "TypeBuilder_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace co {

//------ co.TypeBuilder has a facet named 'typeBuilder', of type co.ITypeBuilder ------//

co::IInterface* TypeBuilder_co_ITypeBuilder::getInterface()
{
	return co::typeOf<co::ITypeBuilder>::get();
}

co::IPort* TypeBuilder_co_ITypeBuilder::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "co.TypeBuilder" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "typeBuilder" ) );
	assert( facet );
	return facet;
}

//------ TypeBuilder_Base ------//

TypeBuilder_Base::TypeBuilder_Base()
{
	// empty
}

TypeBuilder_Base::~TypeBuilder_Base()
{
	// empty
}

co::IObject* TypeBuilder_Base::getProvider()
{
	return this;
}

void TypeBuilder_Base::serviceRetain()
{
	incrementRefCount();
}

void TypeBuilder_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* TypeBuilder_Base::getComponent()
{
	co::IType* type = co::getType( "co.TypeBuilder" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* TypeBuilder_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<co::ITypeBuilder*>( this ); break;
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void TypeBuilder_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
}

} // namespace co
