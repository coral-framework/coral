
#ifndef _CO_IMETHOD_H_
#define _CO_IMETHOD_H_

#include <co/Common.h>
#include <co/IMember.h>

// Forward Declarations:
namespace co {
	class IException; typedef co::RefPtr<IException> IExceptionRef;
	class IParameter; typedef co::RefPtr<IParameter> IParameterRef;
	class IType; typedef co::RefPtr<IType> ITypeRef;
} // namespace co
// End Of Forward Declarations

// co.IMethod Mapping:
namespace co {

class IMethod : public co::IMember
{
public:
	virtual ~IMethod() {;}

	virtual co::TSlice<co::IException*> getExceptions() = 0;

	virtual co::TSlice<co::IParameter*> getParameters() = 0;

	virtual co::IType* getReturnType() = 0;
};

typedef co::RefPtr<IMethod> IMethodRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IMethod> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IMethod> { static const char* get() { return "co.IMethod"; } };
} // namespace co

#endif // _CO_IMETHOD_H_
