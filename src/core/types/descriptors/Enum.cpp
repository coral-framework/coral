#include "Enum.h"

namespace co {

Enum::~Enum()
{
	// empty
}

void Enum::addIdentifier( const std::string& identifier )
{
	_identifiers.push_back( identifier );
}

TSlice<std::string> Enum::getIdentifiers()
{
	return _identifiers;
}

int32 Enum::getValueOf( const std::string& identifier )
{
	int32 count = static_cast<int32>( _identifiers.size() );
	for( int32 i = 0; i < count; ++i )
	{
		if( identifier == _identifiers[i] )
			return i;
	}
	return -1;
}

CORAL_EXPORT_COMPONENT( Enum, Enum );

} // namespace co
