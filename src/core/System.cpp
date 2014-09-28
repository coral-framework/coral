#include "System.h"
#include <co/IllegalStateException.h>
#include <co/reserved/LibraryManager.h>

namespace co {

System::System() {
  _state = SystemState::None;
  _types = new TypeManager;
  _modules = new ModuleManager;
  _services = new ServiceManager;
}

System::~System() {
  // empty
}

void System::initialize() {
  _types->initialize();
  _modules->initialize();
  _services->initialize();
}

SystemState System::getState() { return _state; }

ITypeManager* System::getTypes() { return _types.get(); }

IModuleManager* System::getModules() { return _modules.get(); }

IServiceManager* System::getServices() { return _services.get(); }

void System::setup(Slice<std::string> requiredModules) {
  if (_state != SystemState::None)
    throw IllegalStateException("the system's state is not 'None'");

  _state = SystemState::Initializing;

  try {
    for (; requiredModules; requiredModules.popFirst())
      _modules->load(requiredModules.getFirst());
  } catch (...) {
    _state = SystemState::None;
    throw;
  }

  _state = SystemState::Integrating;

  _modules->updateModules(ModuleState::Integrated);

  _state = SystemState::Running;
}

void System::tearDown() {
  if (_state != SystemState::Running)
    throw IllegalStateException(
        "the system's state is not 'Running'");

  // dispose all modules
  _modules->updateModules(ModuleState::Disintegrated);
  _modules->updateModules(ModuleState::Disposed);

  // release the remaining service instances
  _services->tearDown();
  _modules->tearDown();
  _types->tearDown();

  // make sure all released library instances are unloaded
  LibraryManager::flush();

  _state = SystemState::None;
}

CORAL_EXPORT_COMPONENT(System, System);

}  // namespace co
