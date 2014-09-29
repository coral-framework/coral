#include "ModulePartLoader.h"
#include "Library.h"
#include <co/Coral.h>
#include <co/IType.h>
#include <co/IModule.h>
#include <co/ISystem.h>
#include <co/INamespace.h>
#include <co/IModulePart.h>
#include <co/IModuleManager.h>
#include <co/TypeLoadException.h>
#include <co/ModuleLoadException.h>
#include <co/IllegalArgumentException.h>
#include <co/private/OS.h>
#include <co/private/StringTokenizer.h>
#include <co/private/LibraryManager.h>
#include <cstdio>
#include <sstream>

namespace co {

//---- Module Bootstrap Functions ----------------------------------------------

struct TypeDependency {
  const char* name;
  const char* binarySignature;
};

typedef const char* (*QueryVerificationDataFunction)();
typedef const TypeDependency* (*QueryDependenciesFunction)();
typedef IModulePart* (*ModulePartInstanceFunction)();

struct BootstrapFunctions {
  QueryVerificationDataFunction queryVerificationData;
  QueryDependenciesFunction queryDependencies;
  ModulePartInstanceFunction modulePartInstance;
};

//---- Default IModulePartLoader -----------------------------------------------

ModulePartLoader::ModulePartLoader() {
  // empty
}

ModulePartLoader::~ModulePartLoader() {
  // empty
}

bool ModulePartLoader::canLoadModulePart(const std::string& moduleName) {
  return locateModuleLibrary(moduleName);
}

IModulePart* ModulePartLoader::loadModulePart(const std::string& moduleName) {
  std::string libraryFilename;
  if (!locateModuleLibrary(moduleName, &libraryFilename))
    throw ModuleLoadException("unexpected missing module library file");

  RefPtr<Library> lib(new Library(libraryFilename));
  lib->setLoadHints(Library::GlobalSymbolsHint);
  lib->load();

  // resolve module bootstrap functions
  BootstrapFunctions bf;
  resolveModuleFunctions(lib.get(), bf);

  // sanity check the module
  const char* verificationData = bf.queryVerificationData();
  if (!checkVerificationData(verificationData))
    throw ModuleLoadException("library contains malformed verification data");

  checkTypeDependencies(bf.queryDependencies());

  IModulePart* part = bf.modulePartInstance();
  if (!part) throw ModuleLoadException("library provided a null IModulePart");

  LibraryManager::add(part, lib.get());

  return part;
}

#if defined(CORAL_OS_WINDOWS)
#define MODULE_LIB_EXT ".dll"
#else
#define MODULE_LIB_EXT ".so"
#endif

bool ModulePartLoader::locateModuleLibrary(const std::string& moduleName,
                                           std::string* filename) {
  std::string fileName;

#ifndef CORAL_NDEBUG
  // 'module.name' => 'module/name/module.name_debug.dll'
  fileName.reserve(moduleName.length() + 6 + 4);
  fileName.assign(moduleName);
  fileName.append("_debug" MODULE_LIB_EXT);
#else
  // 'module.name' => 'module/name/module.name.dll'
  fileName.reserve(moduleName.length() + 4);
  fileName.assign(moduleName);
  fileName.append(MODULE_LIB_EXT);
#endif

  std::string modulePath(moduleName);
  OS::convertDotsToDirSeps(modulePath);

  return OS::searchFile3(getPaths(), Slice<std::string>(&modulePath, 1),
                         Slice<std::string>(&fileName, 1),
                         filename ? *filename : modulePath);
}

void ModulePartLoader::resolveModuleFunctions(Library* lib,
                                              BootstrapFunctions& bf) {
  bf.queryVerificationData = reinterpret_cast<QueryVerificationDataFunction>(
      lib->resolve("coral_module_query_verification_data"));
  bf.queryDependencies = reinterpret_cast<QueryDependenciesFunction>(
      lib->resolve("coral_module_query_dependencies"));
  bf.modulePartInstance = reinterpret_cast<ModulePartInstanceFunction>(
      lib->resolve("coral_module_part_instance"));

  if (!bf.queryVerificationData || !bf.queryDependencies ||
      !bf.modulePartInstance)
    CORAL_THROW(
        ModuleLoadException,
        "incompatible library: '"
            << lib->getFileName()
            << "' does not properly export the module bootstrap functions");
}

bool ModulePartLoader::checkVerificationData(const std::string& data) {
  std::string str;
  StringTokenizer st(data, "=\n");
  if ((!st.getNext(str) || str != "pattern") ||
      (!st.getNext(str) || str != "CORAL_MODULE_VERIFICATION_DATA") ||
      (!st.getNext(str) || str != "version" || !st.getNext(str)))
    return false;

  // Coral version compatibility check: the module's Coral version
  // must be the same or older. The 'patch' number is ignored, since
  // patches are required to maintain full backwards compatibility.
  int major, minor;
  if (sscanf(str.c_str(), " %d . %d . %*d", &major, &minor) != 2)
    CORAL_THROW(ModuleLoadException, "malformed version string '"
                                         << str
                                         << "' in module verification data");

  if (major > CORAL_VERSION_MAJOR || minor > CORAL_VERSION_MINOR)
    CORAL_THROW(ModuleLoadException,
                "module was compiled for a newer Coral version ("
                    << major << '.' << minor
                    << "; running: " CORAL_VERSION_STR ")");

  // check the build key; currently it must be a perfect match
  if (!st.getNext(str) || str != "buildkey" || !st.getNext(str)) return false;

  if (str != CORAL_BUILD_KEY)
    CORAL_THROW(ModuleLoadException, "incompatible build keys (module: "
                                         << str
                                         << "; core: " CORAL_BUILD_KEY ")");

  // check the build mode
  if (!st.getNext(str) || str != "buildmode" || !st.getNext(str)) return false;

  // in release mode we cannot load modules built in debug mode, and vice versa
  if (str != CORAL_BUILD_MODE) {
    CORAL_THROW(ModuleLoadException, "incompatible build modes (module: "
                                         << str
                                         << "; core: " CORAL_BUILD_MODE ")");
  }

  return true;
}

void ModulePartLoader::checkTypeDependencies(const TypeDependency* td) {
  std::string currentBinarySignature;
  std::vector<IType*> incompatibleTypes;

  try {
    while (td->name && td->binarySignature) {
      IType* type = getType(td->name);
      if (type->getBinarySignature() != td->binarySignature)
        incompatibleTypes.push_back(type);
      ++td;
    }
  } catch (TypeLoadException& e) {
    CORAL_THROW(ModuleLoadException, "could not load required type '"
                                         << td->name
                                         << "': " << e.getMessage());
  } catch (std::exception& e) {
    CORAL_THROW(ModuleLoadException,
                "type dependency check error: " << e.what());
  }

  if (!getSystem()->getModules()->getCompatibilityChecking())
    return;  // binary interface check errors are disabled

  size_t numIncompatibleTypes = incompatibleTypes.size();
  if (numIncompatibleTypes == 0) return;

  std::stringstream ss;

  if (numIncompatibleTypes == 1)
    ss << "the module was compiled with a different, binary-incompatible "
          "version of '";
  else
    ss << "the module was compiled with different, binary-incompatible "
          "versions of the following types: ";

  for (size_t i = 0; i < numIncompatibleTypes; ++i) {
    if (i > 0) ss << ", ";
    ss << incompatibleTypes[i]->getFullName();
  }

  if (numIncompatibleTypes == 1) ss << '\'';

  throw ModuleLoadException(ss.str());
}

CORAL_EXPORT_COMPONENT(ModulePartLoader, ModulePartLoader);

}  // namespace co
