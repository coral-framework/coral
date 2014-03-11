#ifndef _CO_ILLEGALSTATEEXCEPTION_H_
#define _CO_ILLEGALSTATEEXCEPTION_H_

#include <co/Common.h>
#include <co/Exception.h>

// co.IllegalStateException Mapping:
namespace co {

class CORAL_EXPORT_EXCEPTION IllegalStateException : public co::Exception
{
public:
	IllegalStateException()
	{;}

	IllegalStateException( const std::string& message )
		: co::Exception( message )
	{;}

	virtual ~IllegalStateException() throw()
	{;}

	inline const char* getTypeName() const { return "co.IllegalStateException"; }
};

} // namespace co

namespace co {
template<> struct kindOf<co::IllegalStateException> : public kindOfBase<TK_EXCEPTION> {};
template<> struct nameOf<co::IllegalStateException> { static const char* get() { return "co.IllegalStateException"; } };
} // namespace co

#endif // _CO_ILLEGALSTATEEXCEPTION_H_
