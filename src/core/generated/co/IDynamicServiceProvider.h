#ifndef _CO_IDYNAMICSERVICEPROVIDER_H_
#define _CO_IDYNAMICSERVICEPROVIDER_H_

#include <co/Common.h>
#include <co/Any.h>
#include <co/IService.h>

// Forward Declarations:
namespace co {
	class IField; typedef co::RefPtr<IField> IFieldRef;
	class IMethod; typedef co::RefPtr<IMethod> IMethodRef;
	class IPort; typedef co::RefPtr<IPort> IPortRef;
} // namespace co
// End Of Forward Declarations

// co.IDynamicServiceProvider Mapping:
namespace co {

class IDynamicServiceProvider : public co::IService
{
public:
	virtual ~IDynamicServiceProvider() {;}

	virtual co::IPort* dynamicGetFacet( co::int32 dynFacetId ) = 0;

	virtual void dynamicGetField( co::int32 dynFacetId, co::IField* field, const co::Any& value ) = 0;

	virtual void dynamicInvoke( co::int32 dynFacetId, co::IMethod* method, co::Slice<co::Any> args, const co::Any& result ) = 0;

	virtual co::int32 dynamicRegisterService( co::IService* dynamicServiceProxy ) = 0;

	virtual void dynamicSetField( co::int32 dynFacetId, co::IField* field, const co::Any& value ) = 0;
};

typedef co::RefPtr<IDynamicServiceProvider> IDynamicServiceProviderRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IDynamicServiceProvider> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IDynamicServiceProvider> { static const char* get() { return "co.IDynamicServiceProvider"; } };
} // namespace co

#endif // _CO_IDYNAMICSERVICEPROVIDER_H_
