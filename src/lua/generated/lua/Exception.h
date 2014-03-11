#ifndef _LUA_EXCEPTION_H_
#define _LUA_EXCEPTION_H_

#include <co/Common.h>
#include <co/Exception.h>

// lua.Exception Mapping:
namespace lua {

class CORAL_EXPORT_EXCEPTION Exception : public co::Exception
{
public:
	Exception()
	{;}

	Exception( const std::string& message )
		: co::Exception( message )
	{;}

	virtual ~Exception() throw()
	{;}

	inline const char* getTypeName() const { return "lua.Exception"; }
};

} // namespace lua

namespace co {
template<> struct kindOf<lua::Exception> : public kindOfBase<TK_EXCEPTION> {};
template<> struct nameOf<lua::Exception> { static const char* get() { return "lua.Exception"; } };
} // namespace co

#endif // _LUA_EXCEPTION_H_
