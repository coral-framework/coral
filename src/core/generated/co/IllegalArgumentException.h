#ifndef _CO_ILLEGALARGUMENTEXCEPTION_H_
#define _CO_ILLEGALARGUMENTEXCEPTION_H_

#include <co/Common.h>
#include <co/Exception.h>

// co.IllegalArgumentException Mapping:
namespace co {

class CORAL_EXPORT_EXCEPTION IllegalArgumentException : public co::Exception
{
public:
	IllegalArgumentException()
	{;}

	IllegalArgumentException( const std::string& message )
		: co::Exception( message )
	{;}

	virtual ~IllegalArgumentException() throw()
	{;}

	inline const char* getTypeName() const { return "co.IllegalArgumentException"; }
};

} // namespace co

namespace co {
template<> struct kindOf<co::IllegalArgumentException> : public kindOfBase<TK_EXCEPTION> {};
template<> struct nameOf<co::IllegalArgumentException> { static const char* get() { return "co.IllegalArgumentException"; } };
} // namespace co

#endif // _CO_ILLEGALARGUMENTEXCEPTION_H_
