
#ifndef _CO_ENUM_BASE_H_
#define _CO_ENUM_BASE_H_

#include <co/IEnum.h>
#include <co/reserved/ComponentBase.h>

namespace co {

//! co.Enum has a facet named 'type', of type co.IEnum.
class Enum_co_IEnum : public co::IEnum
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.Enum'.
 */
class Enum_Base : public co::ComponentBase,
	public Enum_co_IEnum
{
public:
	Enum_Base();
	virtual ~Enum_Base();

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

#endif // _CO_ENUM_BASE_H_
