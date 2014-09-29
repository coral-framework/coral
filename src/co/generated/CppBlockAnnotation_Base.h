
#ifndef _CO_CPPBLOCKANNOTATION_BASE_H_
#define _CO_CPPBLOCKANNOTATION_BASE_H_

#include <co/ICppBlock.h>
#include <co/private/ComponentBase.h>

namespace co {

//! co.CppBlockAnnotation has a facet named 'annotation', of type co.ICppBlock.
class CppBlockAnnotation_co_ICppBlock : public co::ICppBlock
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.CppBlockAnnotation'.
 */
class CppBlockAnnotation_Base : public co::ComponentBase,
	public CppBlockAnnotation_co_ICppBlock
{
public:
	CppBlockAnnotation_Base();
	virtual ~CppBlockAnnotation_Base();

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

#endif // _CO_CPPBLOCKANNOTATION_BASE_H_
