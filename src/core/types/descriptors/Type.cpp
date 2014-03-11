#include "Type.h"
#include "../BasicReflector.h"
#include "../SignatureCalculator.h"
#include "../../modules/Module.h"
#include <co/Coral.h>
#include <co/ISystem.h>
#include <co/INamespace.h>
#include <co/IModuleManager.h>
#include <co/ModuleLoadException.h>
#include <co/IDynamicTypeProvider.h>
#include <co/NotSupportedException.h>
#include <cassert>
#include <sstream>

namespace co {

IAnnotation* findAnnotation( Slice<IAnnotation*> annotations, IInterface* annotationType )
{
	if( !annotationType )
		throw IllegalArgumentException( "illegal null annotationType" );

	for( ; annotations; annotations.popFirst() )
		if( annotations.getFirst()->getInterface()->isA( annotationType ) )
			return annotations.getFirst();

	return NULL;
}

//------ TypeImpl -------------------------------------------------------------

void TypeImpl::setNamespace( INamespace* ns )
{
	_namespace = ns;
	if( _fullName.empty() )
	{
		if( ns->getParentNamespace() ) // are we in the root ns?
		{
			_fullName = ns->getFullName();
			_fullName.push_back( '.' );
		}
		_fullName.append( _name );
	}
}

void TypeImpl::setType( TypeKind kind, const std::string& name, INamespace* ns )
{
	assert( _kind == TK_NULL );
	_kind = kind;
	_name = name;
	setNamespace( ns );
}

IReflector* TypeImpl::getReflector( IType* myType )
{
	if( _reflector.isValid() )
		return _reflector.get();

	if( hasBuiltInReflector( _kind ) )
	{
		_reflector = BasicReflector::create( myType );
		return _reflector.get();
	}

	// is this a dynamic type?
	IDynamicTypeProvider* dtp = myType->findAnnotation<IDynamicTypeProvider>();
	if( dtp )
	{
		try
		{
			dtp->provideReflectorFor( myType );
			if( _reflector.isValid() )
				return _reflector.get();

			throw Exception( "no reflector was provided, though no exception was raised" );
		}
		catch( Exception& e )
		{
			const std::string& componentName = dtp->getProvider()->getComponent()->getFullName();
			std::string annotationName( componentName, 0, componentName.size() - 10 );
			CORAL_THROW( NotSupportedException, "error obtaining a reflector for type '"
				<< myType->getFullName() << "' via provider '@" << annotationName
				<< "': " << e.getMessage() );
		}
	}

	// is the type's module already loaded?
	INamespace* ns = myType->getNamespace();
	IModule* module = ns->getModule();
	if( !module )
	{
		/*
			Loading the type's module should cause its reflector to be installed.
			Notice that reflectors cannot be obtained before the system is set up
			(ISystem::setupBase()), since we cannot load modules before that.
		 */
		try
		{
			module = getSystem()->getModules()->load( ns->getFullName() );
		}
		catch( std::exception& e )
		{
			// if an exception was raised, it is fair to expect the reflector was not installed
			assert( !_reflector.isValid() );

			CORAL_THROW( ModuleLoadException, "could not load a reflector for '"
							<< myType->getFullName() << "': " << e.what() );
		}
		catch( ... )
		{
			CORAL_THROW( ModuleLoadException, "could not load a reflector for '"
							<< myType->getFullName() << "': unknown exception" );
		}
	}

	assert( module );

	if( _reflector.isValid() )
		return _reflector.get();

	CORAL_THROW( NotSupportedException, "could not obtain a reflector for type '"
		<< myType->getFullName() << "' (module " << ns->getFullName()
		<< " is " << MODULE_STATE_STRINGS[module->getState()] << ")" );
}

void TypeImpl::calculateSignatures( IType* myType )
{
	assert( !_isCalculatingSignatures );
	_isCalculatingSignatures = true;

	SignatureCalculator sc( myType );
	sc.calculate();

	_fullSignature = sc.getFullSignature();
	_binarySignature = sc.getBinarySignature();

	_hasSignatures = true;
	_isCalculatingSignatures = false;
}

//------ co.Type --------------------------------------------------------------

TypeComponent::TypeComponent( TypeKind kind )
{
	_kind = kind;
	_name = TK_STRINGS[kind];
	_fullName = _name;
}

TypeComponent::~TypeComponent()
{
	// empty
}

CORAL_EXPORT_COMPONENT( TypeComponent, Type );

} // namespace co
