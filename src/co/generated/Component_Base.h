
#ifndef _CO_COMPONENT_BASE_H_
#define _CO_COMPONENT_BASE_H_

#include <co/IComponent.h>
#include <co/private/ComponentBase.h>

namespace co {

//! co.Component has a facet named 'type', of type co.IComponent.
class Component_co_IComponent : public co::IComponent
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.Component'.
 */
class Component_Base : public co::ComponentBase,
	public Component_co_IComponent
{
public:
	Component_Base();
	virtual ~Component_Base();

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

#endif // _CO_COMPONENT_BASE_H_
