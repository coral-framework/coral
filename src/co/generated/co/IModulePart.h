
#ifndef _CO_IMODULEPART_H_
#define _CO_IMODULEPART_H_

#include <co/Common.h>
#include <co/IService.h>

// Forward Declarations:
namespace co {
	class IModule; typedef co::RefPtr<IModule> IModuleRef;
} // namespace co
// End Of Forward Declarations

// co.IModulePart Mapping:
namespace co {

class IModulePart : public co::IService
{
public:
	virtual ~IModulePart() {;}

	virtual void disintegrate( co::IModule* module ) = 0;

	virtual void dispose( co::IModule* module ) = 0;

	virtual void initialize( co::IModule* module ) = 0;

	virtual void integrate( co::IModule* module ) = 0;
};

typedef co::RefPtr<IModulePart> IModulePartRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IModulePart> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IModulePart> { static const char* get() { return "co.IModulePart"; } };
} // namespace co

#endif // _CO_IMODULEPART_H_
