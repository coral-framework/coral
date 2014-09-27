
#ifndef _CO_IMODULEMANAGER_H_
#define _CO_IMODULEMANAGER_H_

#include <co/Common.h>
#include <co/IService.h>

// Forward Declarations:
namespace co {
	class IModule; typedef co::RefPtr<IModule> IModuleRef;
	class IModulePartLoader; typedef co::RefPtr<IModulePartLoader> IModulePartLoaderRef;
} // namespace co
// End Of Forward Declarations

// co.IModuleManager Mapping:
namespace co {

class IModuleManager : public co::IService
{
public:
	virtual ~IModuleManager() {;}

	virtual bool getCompatibilityChecking() = 0;

	virtual void setCompatibilityChecking( bool compatibilityChecking ) = 0;

	virtual co::TSlice<co::IModulePartLoader*> getLoaders() = 0;

	virtual co::TSlice<co::IModule*> getModules() = 0;

	virtual co::IModule* findModule( const std::string& moduleName ) = 0;

	virtual void installLoader( co::IModulePartLoader* loader ) = 0;

	virtual bool isLoadable( const std::string& moduleName ) = 0;

	virtual co::IModule* load( const std::string& moduleName ) = 0;

	virtual void uninstallLoader( co::IModulePartLoader* loader ) = 0;
};

typedef co::RefPtr<IModuleManager> IModuleManagerRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IModuleManager> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IModuleManager> { static const char* get() { return "co.IModuleManager"; } };
} // namespace co

#endif // _CO_IMODULEMANAGER_H_
