#ifndef _CO_ISERVICE_H_
#define _CO_ISERVICE_H_

#include <co/Common.h>

namespace co {

// Forward Decls:
class IPort;

/*!
	The co.IService interface, implicitly inherited by all interfaces.
 */
class IService
{
public:
	virtual ~IService() {;}

	virtual IInterface* getInterface() = 0;

	virtual IObject* getProvider() = 0;

	virtual IPort* getFacet() = 0;

	virtual void serviceRetain() = 0;

	virtual void serviceRelease() = 0;
};

typedef RefPtr<IService> IServiceRef;

template<> struct kindOf<IService> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<IService> { static const char* get() { return "co.IService"; } };

} // namespace co

#endif
