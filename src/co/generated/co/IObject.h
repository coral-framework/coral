
#ifndef _CO_IOBJECT_H_
#define _CO_IOBJECT_H_

#include <co/Common.h>
#include <co/IService.h>

// Forward Declarations:
namespace co {
	class IComponent; typedef co::RefPtr<IComponent> IComponentRef;
	class IPort; typedef co::RefPtr<IPort> IPortRef;
} // namespace co
// End Of Forward Declarations

// co.IObject Mapping:
namespace co {

class IObject : public co::IService
{
public:
	virtual ~IObject() {;}

	// Code From <c++ Block:
	
	/*
		Returns the first service of type `T` provided by this object;
		or NULL if there is no service of type `T`.
	 */
	template<typename T> inline T* findService()
	{
		return static_cast<T*>( findServiceByType( this, typeOf<T>::get() ) );
	}

	/*
		Returns the first service of type T provided by this object.
		Raises co.NoSuchPortException if there is no service of type `T`.
	 */
	template<typename T> inline T* getService()
	{
		return static_cast<T*>( getServiceByType( this, typeOf<T>::get() ) );
	}

	/*
		Returns the service at port `portName` in this object;
		or NULL if there's no port named `portName`.
		__Note__ that NULL is also a valid value for a receptacle.
	 */
	inline IService* findService( const std::string& portName )
	{
		return findServiceByName( this, portName );
	}

	/*
		Returns the service at port `portName` in this object;
		Raises co.NoSuchPortException if there's no port named `portName`.
	 */
	inline IService* getService( const std::string& portName )
	{
		return getServiceByName( this, portName );
	}

	// Binds a `service` to the receptacle named `receptacleName`.
	inline void setService( const std::string& receptacleName, IService* service )
	{
		setServiceByName( this, receptacleName, service );
	}
	
	// End Of c++> Block

	virtual co::IComponent* getComponent() = 0;

	virtual co::IService* getServiceAt( co::IPort* port ) = 0;

	virtual void setServiceAt( co::IPort* receptacle, co::IService* service ) = 0;
};

typedef co::RefPtr<IObject> IObjectRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IObject> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IObject> { static const char* get() { return "co.IObject"; } };
} // namespace co

#endif // _CO_IOBJECT_H_
