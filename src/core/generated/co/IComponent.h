#ifndef _CO_ICOMPONENT_H_
#define _CO_ICOMPONENT_H_

#include <co/Common.h>
#include <co/ICompositeType.h>

// Forward Declarations:
namespace co {
	class IPort; typedef co::RefPtr<IPort> IPortRef;
} // namespace co
// End Of Forward Declarations

// co.IComponent Mapping:
namespace co {

class IComponent : public co::ICompositeType
{
public:
	virtual ~IComponent() {;}

	virtual co::TSlice<co::IPort*> getFacets() = 0;

	virtual co::TSlice<co::IPort*> getPorts() = 0;

	virtual co::TSlice<co::IPort*> getReceptacles() = 0;
};

typedef co::RefPtr<IComponent> IComponentRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IComponent> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IComponent> { static const char* get() { return "co.IComponent"; } };
} // namespace co

#endif // _CO_ICOMPONENT_H_
