#ifndef _LUA_UNIVERSE_BASE_H_
#define _LUA_UNIVERSE_BASE_H_

#include <lua/IState.h>
#include <co/reserved/ComponentBase.h>

namespace lua {

//! lua.Universe has a facet named 'state', of type lua.IState.
class Universe_lua_IState : public lua::IState
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'lua.Universe'.
 */
class Universe_Base : public co::ComponentBase,
	public Universe_lua_IState
{
public:
	Universe_Base();
	virtual ~Universe_Base();

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

#endif // _LUA_UNIVERSE_BASE_H_
