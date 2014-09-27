
#ifndef _CO_IDYNAMICTYPEPROVIDER_H_
#define _CO_IDYNAMICTYPEPROVIDER_H_

#include <co/Common.h>
#include <co/IAnnotation.h>

// Forward Declarations:
namespace co {
	class IType; typedef co::RefPtr<IType> ITypeRef;
} // namespace co
// End Of Forward Declarations

// co.IDynamicTypeProvider Mapping:
namespace co {

class IDynamicTypeProvider : public co::IAnnotation
{
public:
	virtual ~IDynamicTypeProvider() {;}

	virtual void provideReflectorFor( co::IType* type ) = 0;
};

typedef co::RefPtr<IDynamicTypeProvider> IDynamicTypeProviderRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IDynamicTypeProvider> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IDynamicTypeProvider> { static const char* get() { return "co.IDynamicTypeProvider"; } };
} // namespace co

#endif // _CO_IDYNAMICTYPEPROVIDER_H_
