
#ifndef _CO_ARRAYTYPE_BASE_H_
#define _CO_ARRAYTYPE_BASE_H_

#include <co/IArray.h>
#include <co/private/ComponentBase.h>

namespace co {

//! co.ArrayType has a facet named 'type', of type co.IArray.
class ArrayType_co_IArray : public co::IArray
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.ArrayType'.
 */
class ArrayType_Base : public co::ComponentBase,
	public ArrayType_co_IArray
{
public:
	ArrayType_Base();
	virtual ~ArrayType_Base();

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

#endif // _CO_ARRAYTYPE_BASE_H_
