#include "Annotations.h"
#include "IncludeAnnotation_Base.h"

namespace co {

//---------- @co.CppBlock ------------------------------------------------------

CppBlockAnnotation::CppBlockAnnotation()
{
	// empty constructor
}

CppBlockAnnotation::~CppBlockAnnotation()
{
	// empty destructor
}

std::string CppBlockAnnotation::getValue()
{
	return _value;
}

void CppBlockAnnotation::setValue( const std::string& value )
{
	_value = value;
}

CORAL_EXPORT_COMPONENT( CppBlockAnnotation, CppBlockAnnotation );

//---------- @co.Documentation -------------------------------------------------

DocumentationAnnotation::DocumentationAnnotation()
{
	// empty constructor
}

DocumentationAnnotation::~DocumentationAnnotation()
{
	// empty destructor
}

std::string DocumentationAnnotation::getValue()
{
	return _value;
}

void DocumentationAnnotation::setValue( const std::string& value )
{
	_value = value;
}

void DocumentationAnnotation::addDocFor( const std::string& element, const std::string& text )
{
	DocMap::iterator it = _docMap.find( element );
	if( it != _docMap.end() )
	{
		std::string& str = it->second;
		str.reserve( str.length() + 1 + text.length() );
		str.push_back( '\n' );
		str.append( text );
	}
	else
	{
		_docMap.insert( DocMap::value_type( element, text ) );
	}
}

std::string DocumentationAnnotation::getDocFor( const std::string& element )
{
	DocMap::iterator it = _docMap.find( element );
	if( it == _docMap.end() )
		return std::string();
	return it->second;
}

CORAL_EXPORT_COMPONENT( DocumentationAnnotation, DocumentationAnnotation );

//---------- @co.Include -------------------------------------------------------

class IncludeAnnotation : public IncludeAnnotation_Base
{
public:
	std::string getValue()
	{
		return _value;
	}

	void setValue( const std::string& value )
	{
		_value = value;
	}

private:
	std::string _value;
};
	
CORAL_EXPORT_COMPONENT( IncludeAnnotation, IncludeAnnotation );

} // namespace co
