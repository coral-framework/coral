#ifndef _LUA_ISCRIPTED_H_
#define _LUA_ISCRIPTED_H_

#include <co/Common.h>
#include <co/IDynamicTypeProvider.h>

// lua.IScripted Mapping:
namespace lua {

class IScripted : public co::IDynamicTypeProvider
{
public:
	virtual ~IScripted() {;}

	virtual std::string getValue() = 0;

	virtual void setValue( const std::string& value ) = 0;
};

typedef co::RefPtr<IScripted> IScriptedRef;

} // namespace lua

namespace co {
template<> struct kindOf<lua::IScripted> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<lua::IScripted> { static const char* get() { return "lua.IScripted"; } };
} // namespace co

#endif // _LUA_ISCRIPTED_H_
