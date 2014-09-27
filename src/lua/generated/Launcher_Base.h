
#ifndef _LUA_LAUNCHER_BASE_H_
#define _LUA_LAUNCHER_BASE_H_

#include <lua/ILauncher.h>
#include <co/reserved/ComponentBase.h>

namespace lua {

//! lua.Launcher has a facet named 'launcher', of type lua.ILauncher.
class Launcher_lua_ILauncher : public lua::ILauncher
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'lua.Launcher'.
 */
class Launcher_Base : public co::ComponentBase,
	public Launcher_lua_ILauncher
{
public:
	Launcher_Base();
	virtual ~Launcher_Base();

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

#endif // _LUA_LAUNCHER_BASE_H_
