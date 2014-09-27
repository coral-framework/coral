
#ifndef _CO_NAMESPACE_BASE_H_
#define _CO_NAMESPACE_BASE_H_

#include <co/INamespace.h>
#include <co/reserved/ComponentBase.h>

namespace co {

//! co.Namespace has a facet named 'namespace', of type co.INamespace.
class Namespace_co_INamespace : public co::INamespace
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.Namespace'.
 */
class Namespace_Base : public co::ComponentBase,
	public Namespace_co_INamespace
{
public:
	Namespace_Base();
	virtual ~Namespace_Base();

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

#endif // _CO_NAMESPACE_BASE_H_
