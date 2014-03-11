#ifndef _LUA_MODULEPART_BASE_H_
#define _LUA_MODULEPART_BASE_H_

#include <co/IModulePart.h>
#include <co/reserved/ComponentBase.h>

namespace lua {

//! lua.ModulePart has a facet named 'part', of type co.IModulePart.
class ModulePart_co_IModulePart : public co::IModulePart
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'lua.ModulePart'.
 */
class ModulePart_Base : public co::ComponentBase,
	public ModulePart_co_IModulePart
{
public:
	ModulePart_Base();
	virtual ~ModulePart_Base();

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

#endif // _LUA_MODULEPART_BASE_H_
