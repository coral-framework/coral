
#ifndef _CO_IMODULE_H_
#define _CO_IMODULE_H_

#include <co/Common.h>
#include <co/ModuleState.h>
#include <co/IService.h>

// Forward Declarations:
namespace co {
	class IModulePart; typedef co::RefPtr<IModulePart> IModulePartRef;
	class INamespace; typedef co::RefPtr<INamespace> INamespaceRef;
} // namespace co
// End Of Forward Declarations

// co.IModule Mapping:
namespace co {

class IModule : public co::IService
{
public:
	virtual ~IModule() {;}

	virtual co::INamespace* getNamespace() = 0;

	virtual co::TSlice<co::IModulePart*> getParts() = 0;

	virtual co::int32 getRank() = 0;

	virtual void setRank( co::int32 rank ) = 0;

	virtual co::ModuleState getState() = 0;

	virtual void abort() = 0;

	virtual void disintegrate() = 0;

	virtual void dispose() = 0;

	virtual void initialize() = 0;

	virtual void integrate() = 0;

	virtual void integratePresentation() = 0;
};

typedef co::RefPtr<IModule> IModuleRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IModule> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IModule> { static const char* get() { return "co.IModule"; } };
} // namespace co

#endif // _CO_IMODULE_H_
