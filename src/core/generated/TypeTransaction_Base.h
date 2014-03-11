#ifndef _CO_TYPETRANSACTION_BASE_H_
#define _CO_TYPETRANSACTION_BASE_H_

#include <co/ITypeTransaction.h>
#include <co/reserved/ComponentBase.h>

namespace co {

//! co.TypeTransaction has a facet named 'transaction', of type co.ITypeTransaction.
class TypeTransaction_co_ITypeTransaction : public co::ITypeTransaction
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.TypeTransaction'.
 */
class TypeTransaction_Base : public co::ComponentBase,
	public TypeTransaction_co_ITypeTransaction
{
public:
	TypeTransaction_Base();
	virtual ~TypeTransaction_Base();

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

#endif // _CO_TYPETRANSACTION_BASE_H_
