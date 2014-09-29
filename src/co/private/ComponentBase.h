#ifndef _CO_PRIVATE_COMPONENTBASE_H_
#define _CO_PRIVATE_COMPONENTBASE_H_

#include <co/IPort.h>
#include <co/IObject.h>
#include <co/IInterface.h>

namespace co {

/*!
  \brief Base class for all components.
  Provides the implicit 'object' (co::IObject) facet.
 */
class CORAL_EXPORT ComponentBase : public IObject {
 public:
  ComponentBase() : _refCount(0) { ; }

  virtual ~ComponentBase();

  // co::IService methods for the 'object' (co::IObject) facet
  IInterface* getInterface();
  IPort* getFacet();

 protected:
  inline void incrementRefCount() { ++_refCount; }

  inline void decrementRefCount() {
    if (--_refCount <= 0) delete this;
  }

  //! Raises co::NoSuchPortException if \a port isn't one of the component's
  // interfaces.
  void checkValidPort(IPort* port);

  //! Similar to checkValidPort(), but also raises an exception if \a receptacle
  // is a facet.
  void checkValidReceptacle(IPort* receptacle);

  //! Raises a NoSuchPortException for cases that "should never happen" (e.g.
  // invalid port index).
  void raiseUnexpectedPortIndex();

  /*!
    Utility method to dynamically define simple, internal component types (i.e.
    components with a single facet and no reflector). Used internally by the
    core
    for all Reflectors and default IModulePart components.

    \param componentName fully-qualified name of the IComponent.
    \param interfaceName type of the component's sole facet.
    \param facetName name of the component's sole facet.
   */
  IComponent* getOrCreateInternalComponent(const char* componentName,
                                           const char* interfaceName,
                                           const char* facetName);

 private:
  int32 _refCount;
};

}  // namespace co

/*!
  Macro that should be called once for every component implementation class.
  \note Both the class and the macro call should be within the module's
  namespace.
 */
#define CORAL_EXPORT_COMPONENT(ClassName, ComponentName)                \
  co::int32 __##ComponentName##_getSize() { return sizeof(ClassName); } \
  co::IObject* __##ComponentName##_newInstance() { return new ClassName; }

/*!
  Macro that should be called once for each module's main co::IModulePart class.
  \note Both the class and the macro call should be within the module's
  namespace.
 */
#define CORAL_EXPORT_MODULE_PART(ClassName) \
  co::IModulePart* createModulePart() { return new ClassName; }

#endif  // _CO_PRIVATE_COMPONENTBASE_H_
