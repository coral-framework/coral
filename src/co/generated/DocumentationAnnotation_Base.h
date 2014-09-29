
#ifndef _CO_DOCUMENTATIONANNOTATION_BASE_H_
#define _CO_DOCUMENTATIONANNOTATION_BASE_H_

#include <co/IDocumentation.h>
#include <co/private/ComponentBase.h>

namespace co {

//! co.DocumentationAnnotation has a facet named 'annotation', of type co.IDocumentation.
class DocumentationAnnotation_co_IDocumentation : public co::IDocumentation
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.DocumentationAnnotation'.
 */
class DocumentationAnnotation_Base : public co::ComponentBase,
	public DocumentationAnnotation_co_IDocumentation
{
public:
	DocumentationAnnotation_Base();
	virtual ~DocumentationAnnotation_Base();

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

#endif // _CO_DOCUMENTATIONANNOTATION_BASE_H_
