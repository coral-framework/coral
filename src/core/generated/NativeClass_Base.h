#ifndef _CO_NATIVECLASS_BASE_H_
#define _CO_NATIVECLASS_BASE_H_

#include <co/INativeClass.h>
#include <co/reserved/ComponentBase.h>

namespace co {

//! co.NativeClass has a facet named 'type', of type co.INativeClass.
class NativeClass_co_INativeClass : public co::INativeClass
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.NativeClass'.
 */
class NativeClass_Base : public co::ComponentBase,
	public NativeClass_co_INativeClass
{
public:
	NativeClass_Base();
	virtual ~NativeClass_Base();

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

#endif // _CO_NATIVECLASS_BASE_H_
