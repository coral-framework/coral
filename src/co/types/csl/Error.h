#ifndef _CO_CSL_ERROR_H_
#define _CO_CSL_ERROR_H_

#include "location.hh"
#include <co/private/RefCounted.h>

namespace co {
namespace csl {

/*!
	Represents an error stack, with information about the errors that occurred
	while attempting to load a CSL file.

	The error line and filename may not be available in some cases, such as "file not found" and
	semantic errors. (In this case, the filename will be empty and the line will be -1.)
 */
class CORAL_EXPORT Error : public RefCounted
{
public:
	Error( const location& loc, const std::string& message, Error* innerError );
	Error( const std::string& filename, const std::string& message, Error* innerError );
	Error(const std::string& message, Error* innerError );

	virtual ~Error();

	inline const std::string& getMessage() const { return _message; }

	inline const std::string& getFileName() const { return _filename; }

	inline int32 getLine() const { return _line; }

	inline int32 getColumn() const { return _column; }

	inline const Error* getInnerError() const { return _innerError.get(); }

private:
	std::string _message;
	std::string _filename;
	int32 _line;
	int32 _column;
	RefPtr<Error> _innerError;
};

} // namespace csl
} // namespace co

//! Prints a CSL error stack.
CORAL_EXPORT std::ostream& operator<<( std::ostream& out, const co::csl::Error& error );

#endif // _CO_CSL_ERROR_H_
