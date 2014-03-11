#ifndef _CO_INCLUDEANNOTATION_BASE_H_
#define _CO_INCLUDEANNOTATION_BASE_H_

#include <co/IInclude.h>
#include <co/reserved/ComponentBase.h>

namespace co {

//! co.IncludeAnnotation has a facet named 'annotation', of type co.IInclude.
class IncludeAnnotation_co_IInclude : public co::IInclude
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.IncludeAnnotation'.
 */
class IncludeAnnotation_Base : public co::ComponentBase,
	public IncludeAnnotation_co_IInclude
{
public:
	IncludeAnnotation_Base();
	virtual ~IncludeAnnotation_Base();

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

#endif // _CO_INCLUDEANNOTATION_BASE_H_
