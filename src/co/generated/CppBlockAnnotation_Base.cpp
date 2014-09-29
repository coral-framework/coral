
#include "CppBlockAnnotation_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

namespace co {

//------ co.CppBlockAnnotation has a facet named 'annotation', of type co.ICppBlock ------//

co::IInterface* CppBlockAnnotation_co_ICppBlock::getInterface()
{
	return co::typeOf<co::ICppBlock>::get();
}

co::IPort* CppBlockAnnotation_co_ICppBlock::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "co.CppBlockAnnotation" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "annotation" ) );
	assert( facet );
	return facet;
}

//------ CppBlockAnnotation_Base ------//

CppBlockAnnotation_Base::CppBlockAnnotation_Base()
{
	// empty
}

CppBlockAnnotation_Base::~CppBlockAnnotation_Base()
{
	// empty
}

co::IObject* CppBlockAnnotation_Base::getProvider()
{
	return this;
}

void CppBlockAnnotation_Base::serviceRetain()
{
	incrementRefCount();
}

void CppBlockAnnotation_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* CppBlockAnnotation_Base::getComponent()
{
	co::IType* type = co::getType( "co.CppBlockAnnotation" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* CppBlockAnnotation_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0:		res = static_cast<co::ICppBlock*>( this ); break;
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void CppBlockAnnotation_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
}

} // namespace co
