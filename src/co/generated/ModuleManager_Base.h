
#ifndef _CO_MODULEMANAGER_BASE_H_
#define _CO_MODULEMANAGER_BASE_H_

#include <co/IModuleManager.h>
#include <co/private/ComponentBase.h>

namespace co {

//! co.ModuleManager has a facet named 'moduleManager', of type co.IModuleManager.
class ModuleManager_co_IModuleManager : public co::IModuleManager
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.ModuleManager'.
 */
class ModuleManager_Base : public co::ComponentBase,
	public ModuleManager_co_IModuleManager
{
public:
	ModuleManager_Base();
	virtual ~ModuleManager_Base();

	// co::IService Methods:
	co::IObject* getProvider();
	void serviceRetain();
	void serviceRelease();

	// co::IObject Methods:
	co::IComponent* getComponent();
	co::IService* getServiceAt( co::IPort* );
	void setServiceAt( co::IPort*, co::IService* );
};

} // namespace co

#endif // _CO_MODULEMANAGER_BASE_H_
