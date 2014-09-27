
#ifndef _LUA_COMPONENT_BASE_H_
#define _LUA_COMPONENT_BASE_H_

#include <co/IDynamicServiceProvider.h>
#include <co/IReflector.h>
#include <co/reserved/ComponentBase.h>

namespace lua {

//! lua.Component has a facet named 'dynamicServiceProvider', of type co.IDynamicServiceProvider.
class Component_co_IDynamicServiceProvider : public co::IDynamicServiceProvider
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

//! lua.Component has a facet named 'reflector', of type co.IReflector.
class Component_co_IReflector : public co::IReflector
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'lua.Component'.
 */
class Component_Base : public co::ComponentBase,
	public Component_co_IDynamicServiceProvider,
	public Component_co_IReflector
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

} // namespace lua

#endif // _LUA_COMPONENT_BASE_H_
