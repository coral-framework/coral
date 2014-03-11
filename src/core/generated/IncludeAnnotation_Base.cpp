#include "IncludeAnnotation_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace co {

//------ co.IncludeAnnotation has a facet named 'annotation', of type co.IInclude ------//

co::IInterface* IncludeAnnotation_co_IInclude::getInterface()
{
	return co::typeOf<co::IInclude>::get();
}

co::IPort* IncludeAnnotation_co_IInclude::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "co.IncludeAnnotation" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "annotation" ) );
	assert( facet );
	return facet;
}

//------ IncludeAnnotation_Base ------//

IncludeAnnotation_Base::IncludeAnnotation_Base()
{
	// empty
}

IncludeAnnotation_Base::~IncludeAnnotation_Base()
{
	// empty
}

co::IObject* IncludeAnnotation_Base::getProvider()
{
	return this;
}

void IncludeAnnotation_Base::serviceRetain()
{
	incrementRefCount();
}

void IncludeAnnotation_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* IncludeAnnotation_Base::getComponent()
{
	co::IType* type = co::getType( "co.IncludeAnnotation" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* IncludeAnnotation_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<co::IInclude*>( this ); break;
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void IncludeAnnotation_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
}

} // namespace co
