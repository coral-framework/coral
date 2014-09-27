
#ifndef _CO_MISSINGSERVICEEXCEPTION_H_
#define _CO_MISSINGSERVICEEXCEPTION_H_

#include <co/Common.h>
#include <co/Exception.h>

// co.MissingServiceException Mapping:
namespace co {

class CORAL_EXPORT_EXCEPTION MissingServiceException : public co::Exception
{
public:
	MissingServiceException()
	{;}

	MissingServiceException( const std::string& message )
		: co::Exception( message )
	{;}

	virtual ~MissingServiceException() throw()
	{;}

	inline const char* getTypeName() const { return "co.MissingServiceException"; }
};

} // namespace co

namespace co {
template<> struct kindOf<co::MissingServiceException> : public kindOfBase<TK_EXCEPTION> {};
template<> struct nameOf<co::MissingServiceException> { static const char* get() { return "co.MissingServiceException"; } };
} // namespace co

#endif // _CO_MISSINGSERVICEEXCEPTION_H_
