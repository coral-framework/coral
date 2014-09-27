
#ifndef _CO_ICLASSTYPE_H_
#define _CO_ICLASSTYPE_H_

#include <co/Common.h>
#include <co/IRecordType.h>

// Forward Declarations:
namespace co {
	class IMethod; typedef co::RefPtr<IMethod> IMethodRef;
} // namespace co
// End Of Forward Declarations

// co.IClassType Mapping:
namespace co {

class IClassType : public co::IRecordType
{
public:
	virtual ~IClassType() {;}

	virtual co::TSlice<co::IMethod*> getMethods() = 0;
};

typedef co::RefPtr<IClassType> IClassTypeRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IClassType> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IClassType> { static const char* get() { return "co.IClassType"; } };
} // namespace co

#endif // _CO_ICLASSTYPE_H_
