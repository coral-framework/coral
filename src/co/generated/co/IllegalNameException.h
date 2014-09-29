
#ifndef _CO_ILLEGALNAMEEXCEPTION_H_
#define _CO_ILLEGALNAMEEXCEPTION_H_

#include <co/Common.h>
#include <co/Exception.h>

// co.IllegalNameException Mapping:
namespace co {

class CORAL_EXPORT_EXCEPTION IllegalNameException : public co::Exception
{
public:
	IllegalNameException()
	{;}

	IllegalNameException( const std::string& message )
		: co::Exception( message )
	{;}

	virtual ~IllegalNameException() throw()
	{;}

	inline const char* getTypeName() const { return "co.IllegalNameException"; }
};

} // namespace co

namespace co {
template<> struct kindOf<co::IllegalNameException> : public kindOfBase<TK_EXCEPTION> {};
template<> struct nameOf<co::IllegalNameException> { static const char* get() { return "co.IllegalNameException"; } };
} // namespace co

#endif // _CO_ILLEGALNAMEEXCEPTION_H_
