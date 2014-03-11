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
	
		/*!
			Attempts to retrieve a service of a certain type from this object.
			\tparam T interface type of the requested service.
			\return NULL if this object does not provide the requested service.
		 */
		template<typename T> inline T* findService()
		{
			return static_cast<T*>( findServiceByType( this, typeOf<T>::get() ) );
		}

		/*!
			Retrieves a service of a certain type from this object.
			\tparam T interface type of the requested service.
			\throw NoSuchPortException if this object does not provide the requested service.
		 */
		template<typename T> inline T* getService()
		{
			return static_cast<T*>( getServiceByType( this, typeOf<T>::get() ) );
		}

		/*!
			Attempts to retrieve the service at port \a portName in this object.
			\return NULL if this object has no port named \a portName
					(note that NULL can also be returned for receptacles).
		 */
		inline IService* findService( const std::string& portName )
		{
			return findServiceByName( this, portName );
		}

		/*!
			Retrieve the service at port \a portName in this object.
			\throw NoSuchPortException if this object has no port named \a portName.
		 */
		inline IService* getService( const std::string& portName )
		{
			return getServiceByName( this, portName );
		}

		// Binds a \a service to the receptacle named \a receptacleName.
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
