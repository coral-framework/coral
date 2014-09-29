
#ifndef _CO_MODULE_BASE_H_
#define _CO_MODULE_BASE_H_

#include <co/IModule.h>
#include <co/private/ComponentBase.h>

namespace co {

//! co.Module has a facet named 'module', of type co.IModule.
class Module_co_IModule : public co::IModule
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.Module'.
 */
class Module_Base : public co::ComponentBase,
	public Module_co_IModule
{
public:
	Module_Base();
	virtual ~Module_Base();

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

#endif // _CO_MODULE_BASE_H_
