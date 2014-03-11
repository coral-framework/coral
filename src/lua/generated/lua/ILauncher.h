#ifndef _LUA_ILAUNCHER_H_
#define _LUA_ILAUNCHER_H_

#include <co/Common.h>
#include <co/IService.h>

// lua.ILauncher Mapping:
namespace lua {

class ILauncher : public co::IService
{
public:
	virtual ~ILauncher() {;}

	virtual co::int32 main( co::Slice<std::string> args ) = 0;
};

typedef co::RefPtr<ILauncher> ILauncherRef;

} // namespace lua

namespace co {
template<> struct kindOf<lua::ILauncher> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<lua::ILauncher> { static const char* get() { return "lua.ILauncher"; } };
} // namespace co

#endif // _LUA_ILAUNCHER_H_
