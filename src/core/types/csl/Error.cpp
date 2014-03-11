#include "Error.h"
#include <ostream>

namespace co {
namespace csl {

Error::Error( const location& loc, const std::string& message, Error* innerError )
	: _message( message ), _filename( *loc.begin.filename ), _innerError( innerError )
{
	_line = loc.begin.line;
	_column = loc.begin.column;
}

Error::Error( const std::string& filename, const std::string& message, Error* innerError )
	: _message( message ), _filename( filename ), _innerError( innerError )
{
	_line = -1;
	_column = -1;
}

Error::Error( const std::string& message, Error* innerError )
	: _message( message ), _innerError( innerError )
{
	_line = -1;
	_column = -1;
}

Error::~Error()
{
	// empty
}

} // namespace csl
} // namespace co

std::ostream& operator<<( std::ostream& out, const co::csl::Error& error )
{
	int level = 0;

	const co::csl::Error* currentError = &error;
	while( currentError )
	{
		for( int i = 0; i < level; ++i )
			out << '\t';

		if( currentError->getLine() == -1 )
		{
			out << "Error: " << currentError->getMessage() << ".\n";
			assert( !currentError->getInnerError() );
			return out;
		}

		const co::csl::Error* nextError = currentError->getInnerError();
		bool nextIsNotLast = ( nextError != NULL && nextError->getLine() != -1 );

		out << ( nextIsNotLast ? "From " : "In file " )
			<< currentError->getFileName()
			<< ":" << currentError->getLine()
			<< ":" << currentError->getColumn()
			<< ": " << currentError->getMessage()
			<< ( nextIsNotLast ? ":\n" : "" );

		++level;

		currentError = nextError;
	}

	return out;
}
