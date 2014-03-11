#include "MethodBuilder.h"
#include "TypeBuilder.h"
#include "descriptors/Method.h"
#include "descriptors/Parameter.h"
#include <co/Coral.h>
#include <co/ISystem.h>
#include <co/ITypeManager.h>
#include <co/IllegalNameException.h>
#include <co/MissingInputException.h>
#include <co/IllegalArgumentException.h>
#include <co/NotSupportedException.h>
#include <co/reserved/LexicalUtils.h>
#include <sstream>

namespace co {

MethodBuilder::MethodBuilder() : _returnType( NULL )
{
	// empty
}

MethodBuilder::~MethodBuilder()
{
	// empty
}

void MethodBuilder::init( ITypeBuilder* typeBuilder, const std::string& name )
{
	assert( typeBuilder );

	_typeBuilder = typeBuilder;
	_name = name;
}

ITypeBuilder* MethodBuilder::getTypeBuilder()
{
	return _typeBuilder.get();
}

std::string MethodBuilder::getMethodName()
{
	return _name;
}

void MethodBuilder::defineReturnType( IType* type )
{
	if( _returnType )
		CORAL_THROW( NotSupportedException, "return type defined twice" );

	if( !type )
		CORAL_THROW( IllegalArgumentException, "illegal null return type" );

	_returnType = type;
}

void MethodBuilder::defineParameter( const std::string& name, IType* type, bool input, bool output )
{
	if( !LexicalUtils::isValidIdentifier( name ) )
		CORAL_THROW( IllegalNameException, "parameter name '" << name << "' is not a valid identifier" );

	for( Slice<IParameter*> r( _parameters ); r; r.popFirst() )
		if( r.getFirst()->getName() == name )
			CORAL_THROW( IllegalNameException, "parameter '" << name << "' defined twice in method '" << _name << "()'" );

	if( !type )
		CORAL_THROW( IllegalArgumentException, "illegal null parameter type" );

	TypeKind kind = type->getKind();
	if( !isData( kind ) )
		CORAL_THROW( IllegalArgumentException, "illegal parameter '"
				<< name << "' of non-data type '" << kind << "'" );

	if( !input && !output )
		CORAL_THROW( IllegalArgumentException, "parameter is neither input nor output" );

	Parameter* paramInfo = new Parameter();
	paramInfo->init( name, type, input, output );

	_parameters.push_back( paramInfo );
}

void MethodBuilder::defineException( IException* exception )
{
	if( !exception )
		CORAL_THROW( IllegalArgumentException, "the passed exception is invalid" );

	_expectedExceptions.push_back( exception );
}

void MethodBuilder::createMethod()
{
	assert( _typeBuilder.isValid() );

	Method* mic = new Method;
	_createdMethodInfo = mic;

	mic->setName( _name );
	mic->setParameters( _parameters );
	mic->setExceptions( _expectedExceptions );
	mic->setReturnType( _returnType );

	// notice: this may throw an exception if the TB's type was already created
	static_cast<TypeBuilder*>( _typeBuilder.get() )->addMethod( mic );
}

CORAL_EXPORT_COMPONENT( MethodBuilder, MethodBuilder );

} // namespace co
