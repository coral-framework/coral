#include "ServiceManager.h"
#include <co/Coral.h>
#include <co/IObject.h>
#include <co/IReflector.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>
#include <co/NoSuchPortException.h>
#include <co/MissingServiceException.h>
#include <co/IllegalArgumentException.h>
#include <sstream>

namespace co {

ServiceManager::ServiceManager()
{
	_isLazy = true;
}

ServiceManager::~ServiceManager()
{
	// empty
}

void ServiceManager::initialize()
{
	// empty
}

void ServiceManager::tearDown()
{
	_services.clear();
}

bool ServiceManager::getIsLazy()
{
	return _isLazy;
}

void ServiceManager::setIsLazy( bool isLazy )
{
	_isLazy = isLazy;
}

#define CHECK_NOT_NULL( paramName ) \
	if( !paramName ) throw IllegalArgumentException( "illegal null " #paramName );

void ServiceManager::addService( IInterface* serviceType, IService* service )
{
	validateService( serviceType, service );
	_services[serviceType].global.setService( service );
}

void ServiceManager::addServiceForType( IInterface* serviceType, IInterface* clientType, IService* service )
{
	CHECK_NOT_NULL( clientType );

	// is clientType the co.IService interface?
	if( !clientType->getBaseType() )
	{
		addService( serviceType, service );
		return;
	}

	validateService( serviceType, service );
	_services[serviceType].custom[clientType].setService( service );
}

void ServiceManager::addServiceProvider( IInterface* serviceType, const std::string& componentName )
{
	IComponent* component = validateComponent( serviceType, componentName );

	LazyService& lazy = _services[serviceType].global;
	lazy.setComponent( component );

	if( !_isLazy )
		createService( serviceType, lazy );
}

void ServiceManager::addServiceProviderForType( IInterface* serviceType, IInterface* clientType, const std::string& componentName )
{
	CHECK_NOT_NULL( clientType );

	// is clientType the co.IService interface?
	if( !clientType->getBaseType() )
	{
		addServiceProvider( serviceType, componentName );
		return;
	}

	IComponent* component = validateComponent( serviceType, componentName );

	LazyService& lazy = _services[serviceType].custom[clientType];
	lazy.setComponent( component );

	if( !_isLazy )
		createService( serviceType, lazy, clientType );
}

ServiceManager::ServiceRecord& ServiceManager::fetchServiceRecord( IInterface* serviceType )
{
	ServiceMap::iterator it = _services.find( serviceType );
	if( it == _services.end() )
		throw MissingServiceException( "unknown service" );
	return it->second;
}

IService* ServiceManager::getService( IInterface* serviceType )
{
	CHECK_NOT_NULL( serviceType );

	ServiceRecord& rec = fetchServiceRecord( serviceType );
	if( !rec.global.isValid() )
		throw MissingServiceException( "service has no global instance" );

	createService( serviceType, rec.global );

	return rec.global.getService();
}

IService* ServiceManager::getServiceForType( IInterface* serviceType, IInterface* clientType )
{
	CHECK_NOT_NULL( serviceType );
	CHECK_NOT_NULL( clientType );
	return getCustomService( fetchServiceRecord( serviceType ), serviceType, clientType );
}

IPort* ServiceManager::findServiceFacet( IComponent* component, IInterface* serviceType )
{
	TSlice<IPort*> facets = component->getFacets();
	for( ; facets; facets.popFirst() )
	{
		if( facets.getFirst()->getType()->isA( serviceType ) )
			return facets.getFirst();
	}
	return NULL;
}

IService* ServiceManager::getServiceForInstance( IInterface* serviceType, IService* clientInstance )
{
	CHECK_NOT_NULL( serviceType );
	CHECK_NOT_NULL( clientInstance );

	// give preference to the clientInstance's built-in service, when available
	IObject* object = clientInstance->getProvider();
	IComponent* component = object->getComponent();
	IPort* facet = findServiceFacet( component, serviceType );
	if( facet )
		return object->getServiceAt( facet );

	// otherwise, fall back to getCustomService()
	return getCustomService( fetchServiceRecord( serviceType ), serviceType, clientInstance->getInterface() );
}

void ServiceManager::removeService( IInterface* serviceType )
{
	CHECK_NOT_NULL( serviceType );

	ServiceMap::iterator it = _services.find( serviceType );
	if( it != _services.end() )
	{
		ServiceRecord& rec = it->second;
		if( rec.global.isValid() )
		{
			rec.global.clear();
			if( rec.custom.empty() )
				_services.erase( it );
			return;
		}
	}

	throw MissingServiceException( "nothing to remove" );
}

void ServiceManager::removeServiceForType( IInterface* serviceType, IInterface* clientType )
{
	CHECK_NOT_NULL( serviceType );
	CHECK_NOT_NULL( clientType );

	ServiceMap::iterator it = _services.find( serviceType );
	if( it != _services.end() )
	{
		ServiceRecord& rec = it->second;
		CustomServicesMap::iterator customIt = rec.custom.find( clientType );
		if( customIt != rec.custom.end() )
		{
			rec.custom.erase( customIt );
			if( !rec.global.isValid() && rec.custom.empty() )
				_services.erase( it );
			return;
		}
	}

	throw MissingServiceException( "nothing to remove" );
}

void ServiceManager::validateService( IInterface* serviceType, IService* service )
{
	CHECK_NOT_NULL( serviceType );
	CHECK_NOT_NULL( service );
	ensureIsA( service, serviceType );
}

IComponent* ServiceManager::validateComponent( IInterface* serviceType, const std::string& componentName )
{
	CHECK_NOT_NULL( serviceType );

	IComponent* ct = NULL;
	try
	{
		ct = co::cast<IComponent>( getType( componentName ) );
	}
	catch( ... )
	{;}

	if( !ct )
		CORAL_THROW( IllegalArgumentException, "invalid component name '" << componentName << "'" );

	if( findServiceFacet( ct, serviceType ) )
		return ct; // ok, the component provides the service

	CORAL_THROW( NoSuchPortException, "component '" << componentName
			<< "' does not provide service '" << serviceType->getFullName() << "'" );
}

void ServiceManager::createService( IInterface* serviceType, LazyService& lazy, IInterface* clientType )
{
	if( lazy.hasService() )
		return;

	IComponent* ct = lazy.getComponent();

	try
	{
		IReflector* reflector = ct->getReflector();
		IObject* component = reflector->newInstance();
		IPort* serviceFacet = findServiceFacet( ct, serviceType );
		assert( serviceFacet );
		lazy.setService( component->getServiceAt( serviceFacet ) );
	}
	catch( std::exception& e )
	{
		if( clientType )
			removeServiceForType( serviceType, clientType );
		else
			removeService( serviceType );

		CORAL_THROW( MissingServiceException, "error instantiating service '" << serviceType->getFullName()
			<< "' from component type '" << ct->getFullName() << "': " << e.what() );
	}
}

IService* ServiceManager::getCustomService( ServiceRecord& rec, IInterface* serviceType, IInterface* clientType )
{
	// pick the first service instance available for (a super-type of) clientType
	LazyService* lazy = findCustomService( rec, clientType );

	// if none is available, fall back to the global provider
	if( !lazy )
	{
		if( rec.global.isValid() )
			lazy = &rec.global;
		else
			throw MissingServiceException( "no suitable service found" );
	}

	assert( lazy );
	createService( serviceType, *lazy, clientType );
	return lazy->getService();
}

ServiceManager::LazyService* ServiceManager::findCustomService( ServiceRecord& rec, IInterface* clientType )
{
	CustomServicesMap::iterator it = rec.custom.find( clientType );
	if( it != rec.custom.end() )
		return &it->second;

	TSlice<IInterface*> superTypes = clientType->getSuperTypes();
	for( ; superTypes; superTypes.popFirst() )
	{
		// check if a custom service instance was provided for this clientType
		CustomServicesMap::iterator it = rec.custom.find( superTypes.getFirst() );
		if( it != rec.custom.end() )
			return &it->second;
	}

	return NULL;
}

CORAL_EXPORT_COMPONENT( ServiceManager, ServiceManager );

} // namespace co
