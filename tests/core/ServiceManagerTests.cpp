#include <co/Coral.h>
#include <co/ISystem.h>
#include <co/IArray.h>
#include <co/IObject.h>
#include <co/IMember.h>
#include <co/IStruct.h>
#include <co/ITypeManager.h>
#include <co/IInterface.h>
#include <co/IServiceManager.h>
#include <co/IRecordType.h>
#include <co/ITypeTransaction.h>
#include <co/IllegalCastException.h>
#include <co/MissingServiceException.h>
#include <co/IllegalArgumentException.h>
#include <co/NoSuchPortException.h>
#include <gtest/gtest.h>

TEST( ServiceManagerTests, globalService )
{
	co::IServiceManager* sm = co::getSystem()->getServices();

	// try to get the ITypeManager service before it's been added
	co::IInterface* typeManagerType = co::typeOf<co::ITypeManager>::get();
	EXPECT_THROW( sm->getService( NULL ), co::IllegalArgumentException );
	EXPECT_THROW( sm->getService( typeManagerType ), co::MissingServiceException );

	// add the ITypeManager service
	EXPECT_THROW( sm->addService( typeManagerType, NULL ), co::IllegalArgumentException );
	EXPECT_THROW( sm->addService( typeManagerType, co::getSystem() ), co::IllegalCastException );
	sm->addService( typeManagerType, co::getSystem()->getTypes() );

	// now we should be able to get it
	EXPECT_TRUE( sm->getService( typeManagerType ) != NULL );

	// test the utility functions
	EXPECT_EQ( sm->getService( typeManagerType ), co::getServiceForType( typeManagerType, NULL ) );
	EXPECT_EQ( sm->getService( typeManagerType ), co::getService<co::ITypeManager>() );

	// remove the service & check if it's been removed
	sm->removeService( typeManagerType );
	EXPECT_THROW( sm->removeService( typeManagerType ), co::MissingServiceException );
	EXPECT_THROW( sm->getService( typeManagerType ), co::MissingServiceException );
}

TEST( ServiceManagerTests, lazyGlobalService )
{
	co::IServiceManager* sm = co::getSystem()->getServices();

	// make sure the service has not been added yet
	co::IInterface* tctType = co::typeOf<co::ITypeTransaction>::get();
	EXPECT_THROW( sm->getService( tctType ), co::MissingServiceException );

	// add the service with lazy instantiation
	EXPECT_TRUE( sm->getIsLazy() );
	EXPECT_THROW( sm->addServiceProvider( NULL, "co.TypeTransaction" ),
					co::IllegalArgumentException );
	EXPECT_THROW( sm->addServiceProvider( tctType, "invalid" ), co::IllegalArgumentException );
	EXPECT_THROW( sm->addServiceProvider( co::typeOf<co::IType>::get(),
					"co.TypeTransaction" ), co::NoSuchPortException );
	sm->addServiceProvider( tctType, "co.TypeTransaction" );

	// now we should be able to get the service instance
	co::IService* service = sm->getService( tctType );
	EXPECT_TRUE( service != NULL );
	EXPECT_EQ( tctType, service->getInterface() );

	// remove the service
	co::ITypeTransaction* tct = co::cast<co::ITypeTransaction>( service );
	ASSERT_TRUE( tct != NULL );

	tct->rollback();
	sm->removeService( tctType );

	EXPECT_THROW( sm->getService( tctType ), co::MissingServiceException );
}

TEST( ServiceManagerTests, customServicesPerType )
{
	co::IServiceManager* sm = co::getSystem()->getServices();

	// make sure the service has not been added yet
	co::IInterface* memberInfoType = co::typeOf<co::IMember>::get();
	EXPECT_THROW( sm->getService( memberInfoType ), co::MissingServiceException );

	// add a custom instance for IRecordTypes
	co::IInterface* recordType = co::typeOf<co::IRecordType>::get();
	co::IObject* instance = co::newInstance( "co.Field" );
	sm->addServiceForType( memberInfoType, recordType, instance->getService<co::IMember>() );

	// add a different custom instance for IInterfaces
	co::IInterface* interfaceType = co::typeOf<co::IInterface>::get();
	sm->addServiceProviderForType( memberInfoType, interfaceType, "co.Port" );

	// getting the service for an IArray should hit an unavailable global instance
	EXPECT_THROW( sm->getServiceForType( memberInfoType, co::typeOf<co::IArray>::get() ),
					co::MissingServiceException );

	// getting the service for a IStruct should return an IField
	co::IService* itf = sm->getServiceForType( memberInfoType, co::typeOf<co::IStruct>::get() );
	EXPECT_EQ( "co.IField", itf->getInterface()->getFullName() );

	// getting the service for an IInterface should return an IPort
	itf = sm->getServiceForType( memberInfoType, interfaceType );
	EXPECT_EQ( "co.IPort", itf->getInterface()->getFullName() );

	// test the utility functions
	EXPECT_EQ( itf, co::getServiceForType( memberInfoType, interfaceType ) );
	EXPECT_EQ( itf, co::getService<co::IMember>( interfaceType ) );

	// add a global instance for the service
	sm->addServiceProvider( memberInfoType, "co.Method" );

	// now, getting the service for an IArray should return a IMethod
	co::IInterface* arrayTypeType = co::typeOf<co::IArray>::get();
	itf = sm->getServiceForType( memberInfoType, arrayTypeType );
	EXPECT_EQ( "co.IMethod", itf->getInterface()->getFullName() );

	// test the utility functions
	EXPECT_EQ( itf, co::getServiceForType( memberInfoType, arrayTypeType ) );
	EXPECT_EQ( itf, co::getService<co::IMember>( arrayTypeType ) );

	// getting the service for a IStruct should still return an IField
	itf = sm->getServiceForType( memberInfoType, co::typeOf<co::IStruct>::get() );
	EXPECT_EQ( "co.IField", itf->getInterface()->getFullName() );

	// getting the service for an IInterface should still return an IPort
	itf = sm->getServiceForType( memberInfoType, interfaceType );
	EXPECT_EQ( "co.IPort", itf->getInterface()->getFullName() );
}

TEST( ServiceManagerTests, customServicesPerInstance )
{
	// --- assumes the services added in 'customServicesPerType' have not been removed ---
	co::IServiceManager* sm = co::getSystem()->getServices();
	co::IInterface* memberInfoType = co::typeOf<co::IMember>::get();

	// getting a IMember service for a IMethod instance should return the intance's service
	co::IMember* aMethod = co::typeOf<co::IService>::get()->getMember( "serviceRetain" );
	ASSERT_TRUE( aMethod != NULL );

	co::IService* itf = sm->getServiceForInstance( memberInfoType, aMethod );
	co::IMember* mi = co::cast<co::IMember>( itf );
	ASSERT_TRUE( mi != NULL );
	EXPECT_EQ( "serviceRetain", mi->getName() );

	EXPECT_EQ( itf, co::getServiceForInstance( memberInfoType, aMethod ) );
	EXPECT_EQ( itf, co::getService<co::IMember>( aMethod ) );

	// getting a IMember service for the system should return the global service instance
	itf = sm->getServiceForInstance( memberInfoType, co::getSystem() );
	EXPECT_EQ( "co.IMethod", itf->getInterface()->getFullName() );

	EXPECT_EQ( itf, co::getServiceForInstance( memberInfoType, co::getSystem() ) );
	EXPECT_EQ( itf, co::getService<co::IMember>( co::getSystem() ) );

	/*
		Getting an IMember service for an IInterface instance should
		return the IInterface-specialized service.
	 */
	co::IService* clientInstance = co::typeOf<co::IType>::get();
	itf = sm->getServiceForInstance( memberInfoType, clientInstance );
	EXPECT_EQ( "co.IPort", itf->getInterface()->getFullName() );

	EXPECT_EQ( itf, co::getServiceForInstance( memberInfoType, clientInstance ) );
	EXPECT_EQ( itf, co::getService<co::IMember>( clientInstance ) );
}

TEST( ServiceManagerTests, serviceRemovals )
{
	// --- assumes the services added in 'customServicesPerType' have not been removed ---
	co::IServiceManager* sm = co::getSystem()->getServices();
	co::IInterface* memberInfoType = co::typeOf<co::IMember>::get();

	// remove the service's global instance
	sm->removeService( memberInfoType );

	// getting the service for an IArray should hit an unavailable global instance
	EXPECT_THROW( sm->getServiceForType( memberInfoType, co::typeOf<co::IArray>::get() ),
				 co::MissingServiceException );

	// getting the service for a IStruct should still return an IField
	co::IService* itf = sm->getServiceForType( memberInfoType, co::typeOf<co::IStruct>::get() );
	EXPECT_EQ( "co.IField", itf->getInterface()->getFullName() );

	// remove all custom service instances
	sm->removeServiceForType( memberInfoType, co::typeOf<co::IRecordType>::get() );
	sm->removeServiceForType( memberInfoType, co::typeOf<co::IInterface>::get() );

	// the whole service record should have been removed
	try
	{
		sm->getService( memberInfoType );
		FAIL();
	}
	catch( co::MissingServiceException& e )
	{
		EXPECT_EQ( "unknown service", e.getMessage() );
	}
}
