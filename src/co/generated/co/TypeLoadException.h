
#ifndef _CO_TYPELOADEXCEPTION_H_
#define _CO_TYPELOADEXCEPTION_H_

#include <co/Common.h>
#include <co/Exception.h>

// co.TypeLoadException Mapping:
namespace co {

class CORAL_EXPORT_EXCEPTION TypeLoadException : public co::Exception
{
public:
	TypeLoadException()
	{;}

	TypeLoadException( const std::string& message )
		: co::Exception( message )
	{;}

	virtual ~TypeLoadException() throw()
	{;}

	inline const char* getTypeName() const { return "co.TypeLoadException"; }
};

} // namespace co

namespace co {
template<> struct kindOf<co::TypeLoadException> : public kindOfBase<TK_EXCEPTION> {};
template<> struct nameOf<co::TypeLoadException> { static const char* get() { return "co.TypeLoadException"; } };
} // namespace co

#endif // _CO_TYPELOADEXCEPTION_H_
