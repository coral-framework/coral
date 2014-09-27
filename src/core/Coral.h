#ifndef _CO_CORAL_H_
#define _CO_CORAL_H_

#include <co/Common.h>
#include <co/IService.h>
#include <co/IInterface.h>

namespace co {

// Forward Decls:
class IObject;
class ISystem;

//---- Coral Path --------------------------------------------------------------

// Returns the list of type repositories in use by the system.
// All paths are guaranteed to be unique absolute pathnames.
CORAL_EXPORT Slice<std::string> getPaths();

// Adds one or more type repositories for use by the system.
// The passed string may contain a single directory or a list of directories,
// separated by comma or semicolon (or colon, on UNIX systems).
// This function will normalize and make all directory paths absolute.
CORAL_EXPORT void addPath(const std::string& path);

//---- CSL Flags ---------------------------------------------------------------

// Flags for toggling optional language features.
enum CSLFlags {
  CSL_ANNOTATIONS = 1,   // Load and process annotations.
  CSL_CPPBLOCKS = 2,     // Load C++ blocks as 'co.CppBlock' annotations.
  CSL_DOCUMENTATION = 4  // Load comments as 'co.Documentation' annotations.
};

// Returns the current state of the CSL flags.
// Default is CSL_ANNOTATIONS (so comments and C++ blocks are ignored).
CORAL_EXPORT int8 getCSLFlags();

// Changes the state of the CSL flags.
CORAL_EXPORT void setCSLFlags(int8 flags);

//---- Bootstrap and Shutdown --------------------------------------------------

// Returns the primary co.ISystem service.
// The first call to this function initializes the Coral framework.
CORAL_EXPORT ISystem* getSystem();

// Tears down and destroys the system, releasing all memory and resources.
CORAL_EXPORT void shutdown();

//---- General Utility Functions -----------------------------------------------

// Retrieves or loads a type by name.
// This calls: co::getSystem()->getTypes()->getType(fullName)
// See co::ITypeManager::getType() for the list of exceptions this may throw.
CORAL_EXPORT IType* getType(const std::string& fullName);

// Gets or creates an IArray of element type 'elem'.
// This calls: co::getSystem()->getTypes()->getArrayOf(elem)
// See co::ITypeManager::getArrayOf() for the list of exceptions this may throw.
CORAL_EXPORT IArray* getArrayOf(IType* elem);

// Instantiates a component by full type name.
// This calls: co::getType(fullName)->getReflector()->newInstance()
// This function may raise all exceptions raised by the aforementioned methods.
CORAL_EXPORT IObject* newInstance(const std::string& fullName);

// Gets the best provider of 'serviceType' for clients of type 'clientType'.
// If 'clientType' is null this will retrieve the service's global instance.
// See co::IServiceManager::getServiceForType() for the list of exceptions.
CORAL_EXPORT IService* getServiceForType(IInterface* serviceType,
                                         IInterface* clientType);

// Gets the best provider of 'serviceType' for the given 'clientInstance'.
// This calls: co::getSystem()->getServices()->getServiceForInstance()
// See co::IServiceManager::getServiceForInstance() for the list of exceptions.
CORAL_EXPORT IService* getServiceForInstance(IInterface* serviceType,
                                             IService* clientInstance);

// Gets the global service of type T.
// See co::IServiceManager::getService() for the list of exceptions.
template <typename T> inline T* getService() {
  return static_cast<T*>(getServiceForType(typeOf<T>::get(), NULL));
}

// Gets a service of type T specialized for a type 'clientType'.
// See co::IServiceManager::getServiceForType() for the list of exceptions.
template <typename T> inline T* getService(IInterface* clientType) {
  return static_cast<T*>(getServiceForType(typeOf<T>::get(), clientType));
}

// Gets a service of type T specialized for the instance 'clientInstance'.
// See co::IServiceManager::getServiceForInstance() for the list of exceptions.
template <typename T> inline T* getService(IService* clientInstance) {
  return static_cast<T*>(
      getServiceForInstance(typeOf<T>::get(), clientInstance));
}

// Searches for a file within a module in the Coral path.
// This function returns the absolute path to a module resource file.
CORAL_EXPORT bool findFile(const std::string& moduleName,
                           const std::string& fileName, std::string& filePath);

}  // namespace co

#endif  // _CO_CORAL_H_
