#ifndef _CO_FIELD_BASE_H_
#define _CO_FIELD_BASE_H_

#include <co/IField.h>
#include <co/reserved/ComponentBase.h>

namespace co {

//! co.Field has a facet named 'field', of type co.IField.
class Field_co_IField : public co::IField
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.Field'.
 */
class Field_Base : public co::ComponentBase,
	public Field_co_IField
{
public:
	Field_Base();
	virtual ~Field_Base();

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

#endif // _CO_FIELD_BASE_H_
