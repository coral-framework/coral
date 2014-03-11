#include "System.h"
#include <co/IllegalStateException.h>
#include <co/reserved/LibraryManager.h>

namespace co {

System::System()
{
	_state = SystemState_None;
	_types = new TypeManager;
	_modules = new ModuleManager;
	_services = new ServiceManager;
}

System::~System()
{
	// empty
}

void System::initialize()
{
	_types->initialize();
	_modules->initialize();
	_services->initialize();
}

SystemState System::getState()
{
	return _state;
}

ITypeManager* System::getTypes()
{
	return _types.get();
}

IModuleManager* System::getModules()
{
	return _modules.get();
}

IServiceManager* System::getServices()
{
	return _services.get();
}

void System::setupBase( Slice<std::string> requiredModules )
{
	if( _state != SystemState_None )
		throw IllegalStateException( "the system's state is not SystemState_None" );

	_state = SystemState_Initializing;

	try
	{
		for( ; requiredModules; requiredModules.popFirst() )
			_modules->load( requiredModules.getFirst() );
	}
	catch( ... )
	{
		_state = SystemState_None;
		throw;
	}

	_state = SystemState_Integrating;

	_modules->updateModules( ModuleState_Integrated );

	_state = SystemState_Integrated;
}

void System::setupPresentation()
{
	if( _state != SystemState_Integrated )
		throw IllegalStateException( "the system's state is not SystemState_Integrated" );

	_state = SystemState_IntegratingPresentation;

	_modules->updateModules( ModuleState_PresentationIntegrated );

	_state = SystemState_Running;
}

void System::tearDown()
{
	if( _state != SystemState_Running )
		throw IllegalStateException( "the system's state is not SystemState_Running" );

	// dispose all modules
	_modules->updateModules( ModuleState_Disintegrated );
	_modules->updateModules( ModuleState_Disposed );

	// release the remaining service instances
	_services->tearDown();
	_modules->tearDown();
	_types->tearDown();

	// make sure all released library instances are unloaded
	LibraryManager::flush();

	_state = SystemState_None;
}

CORAL_EXPORT_COMPONENT( System, System );

} // namespace co
