#include "ModuleManager.h"
#include "Module.h"
#include "ModulePartLoader.h"
#include <co/Log.h>
#include <co/Coral.h>
#include <co/ISystem.h>
#include <co/INamespace.h>
#include <co/SystemState.h>
#include <co/ITypeManager.h>
#include <co/IllegalStateException.h>
#include <co/ModuleLoadException.h>
#include <co/IllegalArgumentException.h>
#include <algorithm>
#include <sstream>

namespace co {

ModuleManager::ModuleManager() { _compatibilityChecking = true; }

ModuleManager::~ModuleManager() {
  // empty
}

void ModuleManager::initialize() {
  // install the default IModulePartLoader
  _loaders.push_back(new ModulePartLoader);
}

void ModuleManager::tearDown() {
  // empty
}

static bool sortByIncreasingRank(IModule* a, IModule* b) {
  return a->getRank() < b->getRank();
}

void ModuleManager::updateModules(ModuleState state) {
  size_t numModules = _modules.size();

  std::vector<IModule*> sortedModules;
  sortedModules.reserve(numModules);

  for (size_t i = 0; i < numModules; ++i)
    sortedModules.push_back(_modules[i].get());

  std::sort(sortedModules.begin(), sortedModules.end(), sortByIncreasingRank);

  for (size_t i = 0; i < numModules; ++i) {
    IModule* module = sortedModules[i];
    try {
      updateModule(module, state);
    } catch (std::exception& e) {
      module->abort();

      CORAL_LOG(WARNING) << "Module '" << module->getNamespace()->getFullName()
                         << "' aborted due to exception: " << e.what();
    }
  }
}

TSlice<IModule*> ModuleManager::getModules() { return _modules; }

TSlice<IModulePartLoader*> ModuleManager::getLoaders() { return _loaders; }

bool ModuleManager::getCompatibilityChecking() {
  return _compatibilityChecking;
}

void ModuleManager::setCompatibilityChecking(bool enabled) {
  _compatibilityChecking = enabled;
}

IModule* ModuleManager::findModule(const std::string& moduleName) {
  INamespace* ns = getSystem()->getTypes()->findNamespace(moduleName);
  return ns ? ns->getModule() : NULL;
}

void ModuleManager::installLoader(IModulePartLoader* loader) {
  if (!loader) throw IllegalArgumentException("illegal null loader");

  // assert there are no duplicates
  assert(std::find(_loaders.begin(), _loaders.end(), loader) == _loaders.end());

  _loaders.push_back(loader);
}

void ModuleManager::uninstallLoader(IModulePartLoader* loader) {
  ModulePartLoaderList::iterator newEnd =
      std::remove(_loaders.begin(), _loaders.end(), loader);

  if (newEnd == _loaders.end())
    throw IllegalArgumentException("the specified loader was not found");

  _loaders.erase(newEnd, _loaders.end());
}

bool ModuleManager::isLoadable(const std::string& moduleName) {
  for (ModulePartLoaderList::iterator it = _loaders.begin();
       it != _loaders.end(); ++it)
    if (it->get()->canLoadModulePart(moduleName)) return true;

  return false;
}

IModule* ModuleManager::load(const std::string& moduleName) {
  SystemState systemState = getSystem()->getState();

  if (systemState < SystemState::Initializing)
    throw IllegalStateException(
        "cannot load modules before the system is set up");

  if (systemState > SystemState::Running)
    throw IllegalStateException(
        "cannot load modules while the system is being torn down");

  if (_loaders.empty())
    throw ModuleLoadException("there are no installed module loaders");

  // check if the module was already loaded
  IModule* alreadyLoaded = findModule(moduleName);
  if (alreadyLoaded) return alreadyLoaded;

  /*
    Load and initialize ModuleParts. Notice that once we initialize a part,
    it may register a new ModulePartLoader that must also be considered.
   */

  // the IModule is created on demand
  Module* module = NULL;

  // for error handling: whether a part was being loaded (true) or initialized
  // (false)
  bool wasLoading = true;

  try {
    size_t numLoaders = _loaders.size();
    for (size_t i = 0; i < numLoaders; ++i) {
      IModulePartLoader* loader = _loaders[i].get();
      if (!loader->canLoadModulePart(moduleName)) continue;

      // load the module part
      wasLoading = true;

      RefPtr<IModulePart> part(loader->loadModulePart(moduleName));
      if (!part.isValid())
        throw ModuleLoadException("loader returned a null IModulePart");

      if (!module) module = createModule(moduleName);
      module->addPart(part.get());

      // initialize the module part
      wasLoading = false;
      part->initialize(module);

      // this module part may have added a new IModulePartLoader
      size_t newNumLoaders = _loaders.size();
      assert(newNumLoaders >= numLoaders);
      numLoaders = newNumLoaders;
    }
  } catch (std::exception& e) {
    // any error while loading or initializing a part aborts the whole module
    if (module) module->abort();

    std::stringstream ss;
    if (wasLoading)
      ss << "error loading module '" << moduleName << "': ";
    else
      ss << "exception raised by module '" << moduleName
         << "' during initialization: ";
    ss << e.what();

    throw ModuleLoadException(ss.str());
  }

  if (!module) {
    CORAL_THROW(
        ModuleLoadException,
        "no module loader recognized '"
            << moduleName
            << "' as a module (perhaps it was not compiled in " CORAL_BUILD_MODE
               " mode?)");
  }

  module->initialize();
  syncModuleWithSystemState(module);

  return module;
}

Module* ModuleManager::createModule(const std::string& moduleName) {
  Module* module = new Module;

  // add it to the list of loaded modules
  _modules.push_back(module);

  module->setName(moduleName);

  return module;
}

void ModuleManager::updateModule(IModule* module, ModuleState toState) {
  assert(toState > ModuleState::None && toState < ModuleState::Aborted);

  ModuleState fromState = module->getState();
  if (fromState >= ModuleState::Disposed) return;

  if (fromState < ModuleState::Initialized) module->initialize();

  if (toState >= ModuleState::Integrated && fromState < ModuleState::Integrated)
    module->integrate();

  if (toState >= ModuleState::Disintegrated &&
      fromState < ModuleState::Disintegrated)
    module->disintegrate();

  if (toState >= ModuleState::Disposed) module->dispose();
}

void ModuleManager::syncModuleWithSystemState(IModule* module) {
  try {
    switch (getSystem()->getState()) {
      case SystemState::Initializing:
        // module is already initialized
        break;
      case SystemState::Integrating:
      case SystemState::Running:
        updateModule(module, ModuleState::Integrated);
        break;
      default:
        assert(false);
    }
  } catch (std::exception& e) {
    module->abort();
    CORAL_THROW(ModuleLoadException,
                "exception raised by module '"
                    << module->getNamespace()->getFullName()
                    << "' during update: " << e.what());
  }
}

CORAL_EXPORT_COMPONENT(ModuleManager, ModuleManager);

}  // namespace co
