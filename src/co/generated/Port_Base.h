
#ifndef _CO_PORT_BASE_H_
#define _CO_PORT_BASE_H_

#include <co/IPort.h>
#include <co/private/ComponentBase.h>

namespace co {

//! co.Port has a facet named 'port', of type co.IPort.
class Port_co_IPort : public co::IPort
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.Port'.
 */
class Port_Base : public co::ComponentBase,
	public Port_co_IPort
{
public:
	Port_Base();
	virtual ~Port_Base();

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

#endif // _CO_PORT_BASE_H_
