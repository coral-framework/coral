
#ifndef _LUA_SCRIPTEDANNOTATION_BASE_H_
#define _LUA_SCRIPTEDANNOTATION_BASE_H_

#include <lua/IScripted.h>
#include <co/private/ComponentBase.h>

namespace lua {

//! lua.ScriptedAnnotation has a facet named 'annotation', of type lua.IScripted.
class ScriptedAnnotation_lua_IScripted : public lua::IScripted
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'lua.ScriptedAnnotation'.
 */
class ScriptedAnnotation_Base : public co::ComponentBase,
	public ScriptedAnnotation_lua_IScripted
{
public:
	ScriptedAnnotation_Base();
	virtual ~ScriptedAnnotation_Base();

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

#endif // _LUA_SCRIPTEDANNOTATION_BASE_H_
