
#ifndef _LUA_IINTERCEPTOR_H_
#define _LUA_IINTERCEPTOR_H_

#include <co/Common.h>
#include <co/IService.h>
#include <co/Any.h>

// Forward Declarations:
namespace co {
	class IField; typedef co::RefPtr<IField> IFieldRef;
	class IMethod; typedef co::RefPtr<IMethod> IMethodRef;
	class IObject; typedef co::RefPtr<IObject> IObjectRef;
	class IPort; typedef co::RefPtr<IPort> IPortRef;
} // namespace co
// End Of Forward Declarations

// lua.IInterceptor Mapping:
namespace lua {

class IInterceptor : public co::IService
{
public:
	virtual ~IInterceptor() {;}

	virtual void postGetField( co::IService* service, co::IField* field, const co::Any& value ) = 0;

	virtual void postGetService( co::IObject* object, co::IPort* port, co::IService* service ) = 0;

	virtual void postInvoke( co::IService* service, co::IMethod* method, co::Slice<co::Any> args, const co::Any& returnValue ) = 0;

	virtual void postSetField( co::IService* service, co::IField* field, const co::Any& value ) = 0;

	virtual void postSetService( co::IObject* object, co::IPort* receptable, co::IService* service ) = 0;

	virtual void serviceReleased( co::IService* service ) = 0;

	virtual void serviceRetained( co::IService* service ) = 0;
};

typedef co::RefPtr<IInterceptor> IInterceptorRef;

} // namespace lua

namespace co {
template<> struct kindOf<lua::IInterceptor> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<lua::IInterceptor> { static const char* get() { return "lua.IInterceptor"; } };
} // namespace co

#endif // _LUA_IINTERCEPTOR_H_
