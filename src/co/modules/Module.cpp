#include "Module.h"
#include "../types/Namespace.h"
#include <co/Log.h>
#include <co/Coral.h>
#include <co/ISystem.h>
#include <co/INamespace.h>
#include <co/ITypeManager.h>
#include <co/IModulePartLoader.h>
#include <co/IllegalStateException.h>
#include <co/private/LibraryManager.h>

namespace co {

Module::Module() {
  _state = ModuleState::None;
  _namespace = NULL;
  _rank = 0;
}

Module::~Module() { assert(_state >= ModuleState::Disposed); }

void Module::setName(const std::string& moduleName) {
  _namespace = getSystem()->getTypes()->getNamespace(moduleName);
  static_cast<Namespace*>(_namespace)->setModule(this);
}

void Module::addPart(IModulePart* modulePart) {
  if (_state != ModuleState::None)
    throw IllegalStateException(
        "cannot add a module part to a module after it has been initialized");

  assert(modulePart);
  _parts.push_back(modulePart);
}

ModuleState Module::getState() { return _state; }

INamespace* Module::getNamespace() {
  assert(_namespace);
  return _namespace;
}

TSlice<IModulePart*> Module::getParts() { return _parts; }

int32 Module::getRank() { return _rank; }

void Module::setRank(int32 rank) { _rank = rank; }

void Module::initialize() {
  if (_state != ModuleState::None)
    throw IllegalStateException("the module's state is not 'None'");

  /*
    Notice: all initialize() calls are done (by the module manager)
    interleaved with the loading of module parts, so that a modulePart's
    initialization may add a new ModulePartLoader which is immediately
    used to load a new part for the same module.
   */

  _state = ModuleState::Initialized;
}

void Module::integrate() {
  if (_state != ModuleState::Initialized)
    throw IllegalStateException("the module's state is not 'Initialized'");

  for (Slice<IModulePart*> ar(_parts); ar; ar.popFirst())
    ar.getFirst()->integrate(this);

  _state = ModuleState::Integrated;
}

void Module::disintegrate() {
  if (_state != ModuleState::Integrated)
    throw IllegalStateException("the module's state is not 'Integrated'");

  for (Slice<IModulePart*> ar(_parts); ar; ar.popFirst())
    ar.getFirst()->disintegrate(this);

  _state = ModuleState::Disintegrated;
}

void Module::dispose() {
  if (_state != ModuleState::Disintegrated)
    throw IllegalStateException("the module's state is not 'Disintegrated'");

  for (Slice<IModulePart*> ar(_parts); ar; ar.popFirst())
    ar.getFirst()->dispose(this);

  _parts.clear();
  LibraryManager::flush();

  _state = ModuleState::Disposed;
}

void Module::abort() {
  if (_state >= ModuleState::Disintegrated)
    throw IllegalStateException("the module is being or was already disposed");

  // just ignore exceptions raised by ModuleParts from this point on
  // the purpose of the outer loop is to avoid duplicating the 'catch' code
  for (int i = 0; i < 2; ++i) {
    for (Slice<IModulePart*> ar(_parts); ar; ar.popFirst()) {
      try {
        if (i == 0 && _state >= ModuleState::Integrated)
          ar.getFirst()->disintegrate(this);

        if (i == 1) ar.getFirst()->dispose(this);
      } catch (std::exception& e) {
        CORAL_LOG(ERROR) << "Exception ignored while aborting module '"
                         << _namespace->getFullName() << "': " << e.what();
      }
    }
  }

  _parts.clear();
  LibraryManager::flush();

  _state = ModuleState::Aborted;
}

CORAL_EXPORT_COMPONENT(Module, Module);

}  // namespace co
