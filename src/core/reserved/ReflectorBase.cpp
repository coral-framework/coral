#include "ReflectorBase.h"
#include <co/IMethod.h>
#include <co/IField.h>
#include <co/IPort.h>
#include <co/IllegalCastException.h>
#include <co/MissingInputException.h>
#include <co/IllegalArgumentException.h>
#include <co/NotSupportedException.h>
#include <sstream>

namespace co {

/*****************************************************************************/
/* ReflectorBase's 'reflector' facet, of type co::IReflector                 */
/*****************************************************************************/

IInterface* ReflectorBase_co_Reflector::getInterface()
{
	return co::typeOf<IReflector>::get();
}

IPort* ReflectorBase_co_Reflector::getFacet()
{
	IPort* facet = static_cast<IPort*>( typeOf<IReflector>::get()->getMember( "reflector" ) );
	assert( facet );
	return facet;
}

/*****************************************************************************/
/* ReflectorBase                                                             */
/*****************************************************************************/

ReflectorBase::ReflectorBase()
{
	// empty
}

ReflectorBase::~ReflectorBase()
{
	// empty
}

IObject* ReflectorBase::getProvider()
{
	return this;
}

void ReflectorBase::serviceRetain()
{
	incrementRefCount();
}

void ReflectorBase::serviceRelease()
{
	decrementRefCount();
}

IComponent* ReflectorBase::getComponent()
{
	return getOrCreateInternalComponent( "co.ReflectorBase", "co.IReflector", "reflector" );
}

IService* ReflectorBase::getServiceAt( IPort* port )
{
	checkValidPort( port );

	IService* res = NULL;
	switch( port->getIndex() )
	{
	case 0: res = static_cast<IReflector*>( this ); break;
	default:
		raiseUnexpectedPortIndex();
	}

	return res;
}

void ReflectorBase::setServiceAt( IPort* receptacle, IService* )
{
	checkValidReceptacle( receptacle );
	raiseUnexpectedPortIndex();
}

void ReflectorBase::createValues( void* ptr, size_t numValues )
{
	TypeKind kind = getType()->getKind();
	if( kind == TK_INTERFACE )
	{
		for( size_t i = 0; i < numValues; ++i )
			new( reinterpret_cast<RefPtr<IService>*>( ptr ) + i ) RefPtr<IService>();
	}
	else
	{
		assert( false );
		raiseNotSupportedException();
	}
}

void ReflectorBase::copyValues( const void* fromPtr, void* toPtr, size_t numValues )
{
	TypeKind kind = getType()->getKind();
	if( kind == TK_INTERFACE )
	{
		for( size_t i = 0; i < numValues; ++i )
			reinterpret_cast<RefPtr<IService>*>( toPtr )[i] =
				reinterpret_cast<const RefPtr<IService>*>( fromPtr )[i];
	}
	else
	{
		assert( false );
		raiseNotSupportedException();
	}
}

void ReflectorBase::destroyValues( void* ptr, size_t numValues )
{
	TypeKind kind = getType()->getKind();
	if( kind == TK_INTERFACE )
	{
		for( size_t i = 0; i < numValues; ++i )
			reinterpret_cast<RefPtr<IService>*>( ptr )[i].~RefPtr();
	}
	else
	{
		assert( false );
		raiseNotSupportedException();
	}
}

bool ReflectorBase::compareValues( const void* a, const void* b, size_t numValues )
{
	TypeKind kind = getType()->getKind();
	if( kind == TK_INTERFACE )
	{
		for( size_t i = 0; i < numValues; ++i )
			if( reinterpret_cast<const RefPtr<IService>*>( a )[i] !=
				reinterpret_cast<const RefPtr<IService>*>( b )[i] )
				return false;
		return true;
	}

	assert( false );
	raiseNotSupportedException();
	return false;
}

IObject* ReflectorBase::newInstance()
{
	raiseNotSupportedException();
	return NULL;
}

IService* ReflectorBase::newDynamicProxy( IDynamicServiceProvider* )
{
	raiseNotSupportedException();
	return NULL;
}

void ReflectorBase::getField( const Any&, IField*, const Any& )
{
	raiseNotSupportedException();
}

void ReflectorBase::setField( const Any&, IField*, const Any& )
{
	raiseNotSupportedException();
}

void ReflectorBase::invoke( const Any&, IMethod*, Slice<Any>, const Any& )
{
	raiseNotSupportedException();
}

void ReflectorBase::raise( const std::string& )
{
	raiseNotSupportedException();
}

void ReflectorBase::checkValidDynamicProvider( co::IDynamicServiceProvider* provider )
{
	if( provider == NULL )
		throw co::IllegalArgumentException( "illegal null co::IDynamicServiceProvider" );
}

void ReflectorBase::checkNumArguments( co::IMethod* mi, size_t numArgs )
{
	assert( mi );
	size_t expectedNumArgs = mi->getParameters().getSize();
	if( numArgs < expectedNumArgs )
		CORAL_THROW( co::MissingInputException, "method '" << mi->getName() << "' takes "
					<< expectedNumArgs << " argument" << ( expectedNumArgs > 1 ? "s" : "" )
					<< ", but only " << numArgs << ( numArgs > 1 ? " were" : " was" ) << " passed" );
}

void ReflectorBase::raiseFieldIsReadOnly( co::IField* ai )
{
	assert( ai && ai->getIsReadOnly() );
	CORAL_THROW( co::IllegalArgumentException, "field '" << ai->getName() << "' is read-only and cannot be changed" );
}

void ReflectorBase::raiseArgumentTypeException( co::IMethod* mi, int argIndex, const co::IllegalCastException& e )
{
	CORAL_THROW( co::IllegalCastException, "invalid argument #" << argIndex + 1 << " to method "
					<< mi->getName() << "(): " << e.getMessage() );
}

void ReflectorBase::raiseUnexpectedMemberIndex()
{
	assert( false );
	throw co::IllegalArgumentException( "unexpected invalid member index" );
}

void ReflectorBase::raiseNotSupportedException()
{
	throw co::NotSupportedException( "operation not supported by this reflector" );
}

/*****************************************************************************/
/* Auxiliary Functions                                                       */
/*****************************************************************************/

void* checkInstance( const Any& instance, ICompositeType* ct, IMember* member )
{
	assert( ct );

	Any in = instance.asIn();
	if( in.isNull() )
		throw IllegalArgumentException( "illegal null instance" );

	if( !in.getType()->isA( ct ) )
		CORAL_THROW( IllegalArgumentException, "illegal instance (" << ct->getFullName()
				<< " expected, got " << in.getType()->getFullName() << ")" );

	if( !member )
		throw co::IllegalArgumentException( "illegal null member" );

	ICompositeType* owner = member->getOwner();
	if( owner != ct )
		CORAL_THROW( IllegalArgumentException, "member '" << member->getName()
					<< "' belongs to '" << owner->getFullName()
					<< "', not to '" << ct->getFullName() << "'" );

	return in.state.data.ptr;
}

} // namespace co
