
#ifndef _CO_NOTSUPPORTEDEXCEPTION_H_
#define _CO_NOTSUPPORTEDEXCEPTION_H_

#include <co/Common.h>
#include <co/Exception.h>

// co.NotSupportedException Mapping:
namespace co {

class CORAL_EXPORT_EXCEPTION NotSupportedException : public co::Exception
{
public:
	NotSupportedException()
	{;}

	NotSupportedException( const std::string& message )
		: co::Exception( message )
	{;}

	virtual ~NotSupportedException() throw()
	{;}

	inline const char* getTypeName() const { return "co.NotSupportedException"; }
};

} // namespace co

namespace co {
template<> struct kindOf<co::NotSupportedException> : public kindOfBase<TK_EXCEPTION> {};
template<> struct nameOf<co::NotSupportedException> { static const char* get() { return "co.NotSupportedException"; } };
} // namespace co

#endif // _CO_NOTSUPPORTEDEXCEPTION_H_
