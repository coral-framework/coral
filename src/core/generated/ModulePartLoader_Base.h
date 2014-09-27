
#ifndef _CO_MODULEPARTLOADER_BASE_H_
#define _CO_MODULEPARTLOADER_BASE_H_

#include <co/IModulePartLoader.h>
#include <co/reserved/ComponentBase.h>

namespace co {

//! co.ModulePartLoader has a facet named 'loader', of type co.IModulePartLoader.
class ModulePartLoader_co_IModulePartLoader : public co::IModulePartLoader
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.ModulePartLoader'.
 */
class ModulePartLoader_Base : public co::ComponentBase,
	public ModulePartLoader_co_IModulePartLoader
{
public:
	ModulePartLoader_Base();
	virtual ~ModulePartLoader_Base();

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

#endif // _CO_MODULEPARTLOADER_BASE_H_
