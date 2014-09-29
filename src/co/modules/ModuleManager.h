#ifndef _MODULEMANAGER_H_
#define _MODULEMANAGER_H_

#include "ModuleManager_Base.h"
#include <co/IModule.h>
#include <co/IModulePartLoader.h>

namespace co {

// forward declarations:
class Module;

/*!
	Implements co.IModuleManager.
 */
class ModuleManager : public ModuleManager_Base
{
public:
	ModuleManager();
	virtual ~ModuleManager();

	// internal methods:
	void initialize();
	void tearDown();
	void updateModules( ModuleState toState );

	// IModuleManager methods:
	TSlice<IModule*> getModules();
	TSlice<IModulePartLoader*> getLoaders();
	bool getCompatibilityChecking();
	void setCompatibilityChecking( bool enabled );
	IModule* findModule( const std::string& moduleName );
	void installLoader( IModulePartLoader* loader );
	void uninstallLoader( IModulePartLoader* loader );
	bool isLoadable( const std::string& moduleName );
	IModule* load( const std::string& moduleName );

private:
	Module* createModule( const std::string& moduleName );
	void updateModule( IModule* module, ModuleState toState );
	void verifyModuleIntegrity( IModule* module );
	void syncModuleWithSystemState( IModule* module );

private:
	typedef std::vector<IModuleRef> ModuleList;
	ModuleList _modules;

	typedef std::vector<IModulePartLoaderRef> ModulePartLoaderList;
	ModulePartLoaderList _loaders;
	
	bool _compatibilityChecking;
};

} // namespace co

#endif
