#include "Namespace.h"
#include "TypeBuilder.h"
#include "TypeManager.h"
#include "../modules/Module.h"
#include <co/Coral.h>
#include <co/IType.h>
#include <co/ISystem.h>
#include <co/IllegalNameException.h>
#include <co/IllegalArgumentException.h>
#include <co/reserved/LexicalUtils.h>
#include <sstream>

namespace co {

Namespace::Namespace() : _parent( 0 )
{
	// empty
}

Namespace::~Namespace()
{
	// empty
}

void Namespace::setParentAndName( INamespace* parent, const std::string& name )
{
	assert( _fullName.empty() );

	_parent = parent;
	_name = name;

	// compute our 'fullName'
	if( _parent && _parent->getParentNamespace() )
	{
		_fullName = _parent->getFullName();
		_fullName.append( "." );
	}

	_fullName.append( name );
}

void Namespace::setModule( IModule* module )
{
	_module = module;
}

void Namespace::addType( IType* type )
{
	assert( type->getNamespace() == this );

	const std::string& name = type->getName();

	if( findChildNamespace( name ) )
		throwClashingNamespace( name );

	size_t pos;
	if( binarySearch<IType*>( _types, name, typeComparator, pos ) )
		throwClashingType( name );

	_types.insert( _types.begin() + pos, type );
}

void Namespace::removeType( IType* type )
{
	assert( type->getNamespace() == this );

	const std::string& name = type->getName();

	size_t pos;
	if( !binarySearch<IType*>( _types, name, typeComparator, pos ) )
		assert( false );

	_types.erase( _types.begin() + pos );
}

std::string Namespace::getName()
{
	return _name;
}

std::string Namespace::getFullName()
{
	return _fullName;
}

INamespace* Namespace::getParentNamespace()
{
	return _parent;
}

TSlice<IType*> Namespace::getTypes()
{
	return _types;
}

TSlice<INamespace*> Namespace::getChildNamespaces()
{
	return _childNamespaces;
}

IModule* Namespace::getModule()
{
	return _module.get();
}

IType* Namespace::getType( const std::string& name )
{
	return findType( name );
}

INamespace* Namespace::getChildNamespace( const std::string& name )
{
	return findChildNamespace( name );
}

ITypeBuilder* Namespace::defineType( const std::string& name, TypeKind kind )
{
	if( !isUserDefinable( kind ) )
		CORAL_THROW( IllegalArgumentException, "'" << kind
				<<  "' is not a user-definable type kind." );

	if( !LexicalUtils::isValidIdentifier( name ) )
		CORAL_THROW( IllegalNameException, "'" << name
				<< "' is not a valid identifier." );

	IType* type;
	if( ( type = findType( name ) ) && type->getFullName() != "co.IService" )
		throwClashingType( name );

	if( findChildNamespace( name ) )
		throwClashingNamespace( name );

	TypeManager* tm = static_cast<TypeManager*>( getSystem()->getTypes() );
	ITypeBuilder* tb = TypeBuilder::create( kind, this, name );
	tm->addTypeBuilder( tb );

	return tb;
}

INamespace* Namespace::defineChildNamespace( const std::string& name )
{
	Namespace* ns = new Namespace();
	ns->setParentAndName( this, name );

	size_t pos;
	if( binarySearch<INamespace*>( _childNamespaces, name, namespaceComparator, pos ) )
	{
		delete ns;
		throwClashingNamespace( name );
	}

	_childNamespaces.insert( _childNamespaces.begin() + pos, ns );
	return ns;
}

std::string Namespace::getNamespaceName()
{
	if( !_parent )
		return "root namespace";

	const char PREFIX[] = "namespace '";
	std::string str;
	str.reserve( CORAL_ARRAY_LENGTH( PREFIX ) + _fullName.length() + 2 );
	str += PREFIX;
	str += _fullName;
	str += '\'';
	return str;
}

void Namespace::throwClashingType( const std::string& name )
{
	CORAL_THROW( IllegalNameException, getNamespaceName() <<  " already contains a type called '" << name << "'" );
}

void Namespace::throwClashingNamespace( const std::string& name )
{
	CORAL_THROW( IllegalNameException, getNamespaceName() << " already contains a namespace called '" << name << "'" );
}

CORAL_EXPORT_COMPONENT( Namespace, Namespace );

} // namespace co
