#include "Exception.h"

namespace co {

Exception::~Exception() throw()
{
	// empty
}

const char* Exception::getTypeName() const
{
	return "co.Exception";
}

const char* Exception::what() const throw()
{
	return _message.c_str();
}

} // namespace co
