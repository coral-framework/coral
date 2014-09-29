
#ifndef _CO_ILLEGALCASTEXCEPTION_H_
#define _CO_ILLEGALCASTEXCEPTION_H_

#include <co/Common.h>
#include <co/Exception.h>

// co.IllegalCastException Mapping:
namespace co {

class CORAL_EXPORT_EXCEPTION IllegalCastException : public co::Exception
{
public:
	IllegalCastException()
	{;}

	IllegalCastException( const std::string& message )
		: co::Exception( message )
	{;}

	virtual ~IllegalCastException() throw()
	{;}

	inline const char* getTypeName() const { return "co.IllegalCastException"; }
};

} // namespace co

namespace co {
template<> struct kindOf<co::IllegalCastException> : public kindOfBase<TK_EXCEPTION> {};
template<> struct nameOf<co::IllegalCastException> { static const char* get() { return "co.IllegalCastException"; } };
} // namespace co

#endif // _CO_ILLEGALCASTEXCEPTION_H_
