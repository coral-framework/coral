
#ifndef _CO_EXCEPTIONTYPE_BASE_H_
#define _CO_EXCEPTIONTYPE_BASE_H_

#include <co/IException.h>
#include <co/private/ComponentBase.h>

namespace co {

//! co.ExceptionType has a facet named 'type', of type co.IException.
class ExceptionType_co_IException : public co::IException
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

/*!
	Inherit from this class to implement the component 'co.ExceptionType'.
 */
class ExceptionType_Base : public co::ComponentBase,
	public ExceptionType_co_IException
{
public:
	ExceptionType_Base();
	virtual ~ExceptionType_Base();

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

#endif // _CO_EXCEPTIONTYPE_BASE_H_
