
#ifndef _CO_PARAMETER_BASE_H_
#define _CO_PARAMETER_BASE_H_

#include <co/IParameter.h>
#include <co/reserved/ComponentBase.h>

namespace co {

//! co.Parameter has a facet named 'parameter', of type co.IParameter.
class Parameter_co_IParameter : public co::IParameter
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.Parameter'.
 */
class Parameter_Base : public co::ComponentBase,
	public Parameter_co_IParameter
{
public:
	Parameter_Base();
	virtual ~Parameter_Base();

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

#endif // _CO_PARAMETER_BASE_H_
