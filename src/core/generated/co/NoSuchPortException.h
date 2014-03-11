#ifndef _CO_NOSUCHPORTEXCEPTION_H_
#define _CO_NOSUCHPORTEXCEPTION_H_

#include <co/Common.h>
#include <co/Exception.h>

// co.NoSuchPortException Mapping:
namespace co {

class CORAL_EXPORT_EXCEPTION NoSuchPortException : public co::Exception
{
public:
	NoSuchPortException()
	{;}

	NoSuchPortException( const std::string& message )
		: co::Exception( message )
	{;}

	virtual ~NoSuchPortException() throw()
	{;}

	inline const char* getTypeName() const { return "co.NoSuchPortException"; }
};

} // namespace co

namespace co {
template<> struct kindOf<co::NoSuchPortException> : public kindOfBase<TK_EXCEPTION> {};
template<> struct nameOf<co::NoSuchPortException> { static const char* get() { return "co.NoSuchPortException"; } };
} // namespace co

#endif // _CO_NOSUCHPORTEXCEPTION_H_
