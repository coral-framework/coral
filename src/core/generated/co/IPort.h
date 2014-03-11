#ifndef _CO_IPORT_H_
#define _CO_IPORT_H_

#include <co/Common.h>
#include <co/IMember.h>

// Forward Declarations:
namespace co {
	class IInterface; typedef co::RefPtr<IInterface> IInterfaceRef;
} // namespace co
// End Of Forward Declarations

// co.IPort Mapping:
namespace co {

class IPort : public co::IMember
{
public:
	virtual ~IPort() {;}

	virtual bool getIsFacet() = 0;

	virtual co::IInterface* getType() = 0;
};

typedef co::RefPtr<IPort> IPortRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IPort> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IPort> { static const char* get() { return "co.IPort"; } };
} // namespace co

#endif // _CO_IPORT_H_
