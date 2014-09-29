#ifndef _CO_SERVICEMANAGER_H_
#define _CO_SERVICEMANAGER_H_

#include "ServiceManager_Base.h"
#include <co/IComponent.h>
#include <map>

namespace co {

/*!
	Implements co.IServiceManager.
 */
class ServiceManager : public ServiceManager_Base
{
public:
	ServiceManager();
	virtual ~ServiceManager();

	// internal methods:
	void initialize();
	void tearDown();

	// IServiceManager methods:
	bool getIsLazy();
	void setIsLazy( bool isLazy );
	void addService( IInterface* serviceType, IService* service );
	void addServiceForType( IInterface* serviceType, IInterface* clientType, IService* service );
	void addServiceProvider( IInterface* serviceType, const std::string& componentName );
	void addServiceProviderForType( IInterface* serviceType, IInterface* clientType, const std::string& componentName );
	IService* getService( IInterface* serviceType );
	IService* getServiceForType( IInterface* serviceType, IInterface* clientType );
	IService* getServiceForInstance( IInterface* serviceType, IService* clientInstance );
	void removeService( IInterface* serviceType );
	void removeServiceForType( IInterface* serviceType, IInterface* clientType );

private:
	class LazyService
	{
	public:
		inline LazyService() : _service( NULL ), _hasService( false ) {;}
		inline ~LazyService() { clear(); }

		inline bool isValid() const { return _service != NULL; }

		inline void clear()
		{
			if( _hasService )
				_service->serviceRelease();
			_service = NULL;
			_hasService = false;
		}

		inline bool hasService() const { return _hasService; }

		inline IService* getService() { return _service; }

		inline void setService( IService* instance )
		{
			clear();
			_service = instance;
			_service->serviceRetain();
			_hasService = true;
		}

		inline IComponent* getComponent() { return _component; }

		inline void setComponent( IComponent* type )
		{
			clear();
			_component = type;
		}

	private:
		union
		{
			IService* _service;
			IComponent* _component;
		};
		bool _hasService; // true if we got a '_service'; false if we got a '_component'.
	};

	typedef std::map<IInterface*, LazyService> CustomServicesMap;

	struct ServiceRecord
	{
		LazyService global;
		CustomServicesMap custom; // specialized by clientType
	};

	typedef std::map<IInterface*, ServiceRecord> ServiceMap;

	inline ServiceRecord& fetchServiceRecord( IInterface* serviceType );
	inline IPort* findServiceFacet( IComponent* component, IInterface* serviceType );

	void validateService( IInterface* serviceType, IService* service );
	IComponent* validateComponent( IInterface* serviceType, const std::string& componentName );

	void createService( IInterface* serviceType, LazyService& lazy, IInterface* clientType = 0 );

	IService* getCustomService( ServiceRecord& rec, IInterface* serviceType, IInterface* clientType );
	LazyService* findCustomService( ServiceRecord& rec, IInterface* clientType );

private:
	bool _isLazy;
	ServiceMap _services;
};

} // namespace co

#endif
