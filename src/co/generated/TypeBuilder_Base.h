
#ifndef _CO_TYPEBUILDER_BASE_H_
#define _CO_TYPEBUILDER_BASE_H_

#include <co/ITypeBuilder.h>
#include <co/private/ComponentBase.h>

namespace co {

//! co.TypeBuilder has a facet named 'typeBuilder', of type co.ITypeBuilder.
class TypeBuilder_co_ITypeBuilder : public co::ITypeBuilder
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.TypeBuilder'.
 */
class TypeBuilder_Base : public co::ComponentBase,
	public TypeBuilder_co_ITypeBuilder
{
public:
	TypeBuilder_Base();
	virtual ~TypeBuilder_Base();

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

#endif // _CO_TYPEBUILDER_BASE_H_
