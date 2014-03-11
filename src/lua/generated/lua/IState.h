#ifndef _LUA_ISTATE_H_
#define _LUA_ISTATE_H_

#include <co/Common.h>
#include <co/Any.h>
#include <co/IService.h>

// Forward Declarations:
namespace lua {
	class IInterceptor; typedef co::RefPtr<IInterceptor> IInterceptorRef;
} // namespace lua
// End Of Forward Declarations

// lua.IState Mapping:
namespace lua {

class IState : public co::IService
{
public:
	virtual ~IState() {;}

	virtual co::TSlice<lua::IInterceptor*> getInterceptors() = 0;

	virtual void addInterceptor( lua::IInterceptor* interceptor ) = 0;

	virtual co::int32 call( const std::string& moduleName, const std::string& functionName, co::Slice<co::Any> args, co::Slice<co::Any> results ) = 0;

	virtual void collectGarbage() = 0;

	virtual bool findScript( const std::string& name, std::string& filename ) = 0;

	virtual void removeInterceptor( lua::IInterceptor* interceptor ) = 0;
};

typedef co::RefPtr<IState> IStateRef;

} // namespace lua

namespace co {
template<> struct kindOf<lua::IState> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<lua::IState> { static const char* get() { return "lua.IState"; } };
} // namespace co

#endif // _LUA_ISTATE_H_
