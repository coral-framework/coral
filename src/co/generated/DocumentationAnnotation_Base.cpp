
#include "DocumentationAnnotation_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace co {

//------ co.DocumentationAnnotation has a facet named 'annotation', of type co.IDocumentation ------//

co::IInterface* DocumentationAnnotation_co_IDocumentation::getInterface()
{
	return co::typeOf<co::IDocumentation>::get();
}

co::IPort* DocumentationAnnotation_co_IDocumentation::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "co.DocumentationAnnotation" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "annotation" ) );
	assert( facet );
	return facet;
}

//------ DocumentationAnnotation_Base ------//

DocumentationAnnotation_Base::DocumentationAnnotation_Base()
{
	// empty
}

DocumentationAnnotation_Base::~DocumentationAnnotation_Base()
{
	// empty
}

co::IObject* DocumentationAnnotation_Base::getProvider()
{
	return this;
}

void DocumentationAnnotation_Base::serviceRetain()
{
	incrementRefCount();
}

void DocumentationAnnotation_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* DocumentationAnnotation_Base::getComponent()
{
	co::IType* type = co::getType( "co.DocumentationAnnotation" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* DocumentationAnnotation_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<co::IDocumentation*>( this ); break;
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void DocumentationAnnotation_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
}

} // namespace co
