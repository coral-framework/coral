#include "StringTokenizer.h"

namespace co {

StringTokenizer::StringTokenizer( const std::string& str, const std::string& delimiters )
	: _str( str ), _delimiters( delimiters )
{
	_offset = 0;
}

bool StringTokenizer::nextToken()
{
	// skip delimiters at beginning
	size_t startPos = _str.find_first_not_of( _delimiters, _offset );
	if( startPos == std::string::npos )
	{
		_offset = _str.length();
		return false;
	}

	// find first delimiter after startPos
	size_t endPos = _str.find_first_of( _delimiters, startPos );
	if( endPos == std::string::npos )
	{
		_offset = _str.length();
		_token.assign( _str, startPos, _offset );
		return true;
	}

	_token.assign( _str, startPos, endPos - startPos );
	_offset = endPos;

	return true;
}

} // namespace co
