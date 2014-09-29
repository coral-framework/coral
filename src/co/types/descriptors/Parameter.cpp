#include "Parameter.h"
#include <co/IllegalNameException.h>
#include <co/IllegalArgumentException.h>
#include <co/private/LexicalUtils.h>
#include <sstream>

namespace co {

Parameter::Parameter() : _type( 0 ), _isIn( true ), _isOut( false )
{
	// empty
}

Parameter::~Parameter()
{
	// empty
}

void Parameter::init( const std::string& name, IType* type, bool isIn, bool isOut )
{
	if( !LexicalUtils::isValidIdentifier( name ) )
		CORAL_THROW( IllegalNameException, "the passed parameter name is invalid" );

	if( type == NULL )
		CORAL_THROW( IllegalArgumentException, "the passed parameter type is invalid" );

	if( isIn == false && isOut == false )
		CORAL_THROW( IllegalArgumentException, "both isIn and isOut are false" );

	setName( name );
	setType( type );
	setIsIn( isIn );
	setIsOut( isOut );
}

void Parameter::setName( const std::string& name )
{
	_name = name;
}

void Parameter::setType( IType* type )
{
	_type = type;
}

void Parameter::setIsIn( bool isIn )
{
	_isIn = isIn;
}

void Parameter::setIsOut( bool isOut )
{
	_isOut = isOut;
}

std::string Parameter::getName()
{
	return _name;
}

IType* Parameter::getType()
{
	return _type;
}

bool Parameter::getIsIn()
{
	return _isIn;
}

bool Parameter::getIsOut()
{
	return _isOut;
}

CORAL_EXPORT_COMPONENT( Parameter, Parameter );

} // namespace co
