#include "Coral.h"
#include "System.h"
#include <co/Log.h>
#include <co/RefPtr.h>
#include <co/IReflector.h>
#include <co/private/OS.h>
#include <co/private/LibraryManager.h>
#include <co/private/StringTokenizer.h>
#include <algorithm>

namespace co {

#ifdef CORAL_OS_WINDOWS
static const char* PATH_SEPARATORS = ";,";
#else
static const char* PATH_SEPARATORS = ";:,";
#endif

static std::vector<std::string> sg_paths;
static int8 sg_cslFlags(CSL_ANNOTATIONS);
static RefPtr<System> sg_system;
static ITypeManager* sg_typeManager(NULL);
static IServiceManager* sg_serviceManager(NULL);

inline ITypeManager* getTypeManager() {
  if (!sg_typeManager) sg_typeManager = getSystem()->getTypes();
  return sg_typeManager;
}

Slice<std::string> getPaths() { return sg_paths; }

void addPath(const std::string& path) {
  std::string dirPath;
  StringTokenizer st(path, PATH_SEPARATORS);
  while (st.getNext(dirPath)) {
    assert(!dirPath.empty());

    // normalize & absolutize the path
    OS::makeAbs(dirPath);

    if (!OS::isDir(dirPath)) {
      CORAL_LOG(WARNING) << "cannot add '" << dirPath
                         << "' to the Coral path (not a dir)";
      continue;
    }

    // Check whether the dir is not in the CORAL_PATH already.
    // This makes addPath() quadratic, but should not be a problem.
    if (std::find(sg_paths.begin(), sg_paths.end(), dirPath) == sg_paths.end())
      sg_paths.push_back(dirPath);
  }
}

int8 getCSLFlags() { return sg_cslFlags; }

void setCSLFlags(int8 flags) { sg_cslFlags = flags; }

ISystem* getSystem() {
  if (!sg_system) {
    sg_system = new System;
    sg_system->initialize();
  }
  return sg_system.get();
}

void shutdown() {
  if (!sg_system.isValid()) return;

  // tear down the system if it's still running
  if (sg_system->getState() == SystemState::Running) sg_system->tearDown();

  // release the main system interfaces
  sg_serviceManager = NULL;
  sg_typeManager = NULL;
  sg_system = NULL;

  // flush all released libraries
  LibraryManager::flush();
}

IType* getType(const std::string& fullName) {
  return getTypeManager()->getType(fullName);
}

IArray* getArrayOf(IType* elementType) {
  return getTypeManager()->getArrayOf(elementType);
}

IObject* newInstance(const std::string& fullName) {
  IType* type = getType(fullName);
  assert(type);
  IReflector* reflector = type->getReflector();
  assert(reflector);
  return reflector->newInstance();
}

inline IServiceManager* getServices() {
  if (!sg_serviceManager) sg_serviceManager = getSystem()->getServices();
  return sg_serviceManager;
}

IService* getServiceForType(IInterface* serviceType, IInterface* clientType) {
  if (clientType)
    return getServices()->getServiceForType(serviceType, clientType);
  else
    return getServices()->getService(serviceType);
}

IService* getServiceForInstance(IInterface* serviceType,
                                IService* clientInstance) {
  return getServices()->getServiceForInstance(serviceType, clientInstance);
}

bool findFile(const std::string& moduleName, const std::string& fileName,
              std::string& filePath) {
  std::string modulePath(moduleName);
  OS::convertDotsToDirSeps(modulePath);
  return OS::searchFile3(getPaths(), Slice<std::string>(&modulePath, 1),
                         Slice<std::string>(&fileName, 1), filePath);
}

}  // namespace co
