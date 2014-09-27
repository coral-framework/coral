
#ifndef _CO_IMEMBER_H_
#define _CO_IMEMBER_H_

#include <co/Common.h>
#include <co/MemberKind.h>
#include <co/IAnnotated.h>

// Forward Declarations:
namespace co {
	class ICompositeType; typedef co::RefPtr<ICompositeType> ICompositeTypeRef;
} // namespace co
// End Of Forward Declarations

// co.IMember Mapping:
namespace co {

class IMember : public co::IAnnotated
{
public:
	virtual ~IMember() {;}

	virtual co::int16 getIndex() = 0;

	virtual co::MemberKind getKind() = 0;

	virtual std::string getName() = 0;

	virtual co::ICompositeType* getOwner() = 0;
};

typedef co::RefPtr<IMember> IMemberRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IMember> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IMember> { static const char* get() { return "co.IMember"; } };
} // namespace co

#endif // _CO_IMEMBER_H_
