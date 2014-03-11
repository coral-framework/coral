#ifndef _CO_SERVICEMANAGER_BASE_H_
#define _CO_SERVICEMANAGER_BASE_H_

#include <co/IServiceManager.h>
#include <co/reserved/ComponentBase.h>

namespace co {

//! co.ServiceManager has a facet named 'serviceManager', of type co.IServiceManager.
class ServiceManager_co_IServiceManager : public co::IServiceManager
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.ServiceManager'.
 */
class ServiceManager_Base : public co::ComponentBase,
	public ServiceManager_co_IServiceManager
{
public:
	ServiceManager_Base();
	virtual ~ServiceManager_Base();

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

#endif // _CO_SERVICEMANAGER_BASE_H_
