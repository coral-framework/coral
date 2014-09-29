
#ifndef _CO_TYPE_BASE_H_
#define _CO_TYPE_BASE_H_

#include <co/IType.h>
#include <co/private/ComponentBase.h>

namespace co {

//! co.Type has a facet named 'type', of type co.IType.
class Type_co_IType : public co::IType
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.Type'.
 */
class Type_Base : public co::ComponentBase,
	public Type_co_IType
{
public:
	Type_Base();
	virtual ~Type_Base();

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

#endif // _CO_TYPE_BASE_H_
