
#ifndef _LUA_LUA_BASE_H_
#define _LUA_LUA_BASE_H_

#include <co/IModulePart.h>
#include <co/reserved/ComponentBase.h>

namespace lua {

//! lua.lua has a facet named 'part', of type co.IModulePart.
class lua_co_IModulePart : public co::IModulePart
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'lua.lua'.
 */
class lua_Base : public co::ComponentBase,
	public lua_co_IModulePart
{
public:
	lua_Base();
	virtual ~lua_Base();

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

#endif // _LUA_LUA_BASE_H_
