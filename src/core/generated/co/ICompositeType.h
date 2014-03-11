#ifndef _CO_ICOMPOSITETYPE_H_
#define _CO_ICOMPOSITETYPE_H_

#include <co/Common.h>
#include <co/IType.h>

// Forward Declarations:
namespace co {
	class IMember; typedef co::RefPtr<IMember> IMemberRef;
} // namespace co
// End Of Forward Declarations

// co.ICompositeType Mapping:
namespace co {

class ICompositeType : public co::IType
{
public:
	virtual ~ICompositeType() {;}

	virtual co::TSlice<co::IMember*> getMembers() = 0;

	virtual co::IMember* getMember( const std::string& name ) = 0;
};

typedef co::RefPtr<ICompositeType> ICompositeTypeRef;

} // namespace co

namespace co {
template<> struct kindOf<co::ICompositeType> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::ICompositeType> { static const char* get() { return "co.ICompositeType"; } };
} // namespace co

#endif // _CO_ICOMPOSITETYPE_H_
