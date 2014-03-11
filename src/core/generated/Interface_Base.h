#ifndef _CO_INTERFACE_BASE_H_
#define _CO_INTERFACE_BASE_H_

#include <co/IInterface.h>
#include <co/reserved/ComponentBase.h>

namespace co {

//! co.Interface has a facet named 'type', of type co.IInterface.
class Interface_co_IInterface : public co::IInterface
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.Interface'.
 */
class Interface_Base : public co::ComponentBase,
	public Interface_co_IInterface
{
public:
	Interface_Base();
	virtual ~Interface_Base();

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

#endif // _CO_INTERFACE_BASE_H_
