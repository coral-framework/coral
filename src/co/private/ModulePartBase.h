#ifndef _CO_PRIVATE_MODULEPARTBASE_H_
#define _CO_PRIVATE_MODULEPARTBASE_H_

#include "ComponentBase.h"
#include <co/RefPtr.h>
#include <co/IModule.h>
#include <co/IModulePart.h>
#include <co/IModulePartLoader.h>

namespace co {

//! ModulePartBase provides a facet named 'part', of type co::IModulePart.
class CORAL_EXPORT ModulePartBase_co_ModulePart : public IModulePart {
 public:
  virtual IInterface* getInterface();
  virtual IPort* getFacet();
};

/*!
  Auxiliary base class used in generated code to implement the default
  co::IModulePart of a module.
 */
class CORAL_EXPORT ModulePartBase : public ComponentBase,
                                    public ModulePartBase_co_ModulePart {
 public:
  ModulePartBase();
  virtual ~ModulePartBase();

  // co::IService methods:
  IObject* getProvider();
  void serviceRetain();
  void serviceRelease();

  // co::IObject methods:
  IComponent* getComponent();
  IService* getServiceAt(IPort*);
  void setServiceAt(IPort*, IService*);

  // co::IModulePart methods:
  void initialize(co::IModule* module);
  void integrate(co::IModule* module);
  void integratePresentation(co::IModule* module);
  void disintegrate(co::IModule* module);
  void dispose(co::IModule* module);
};

}  // namespace co

#endif  // _CO_PRIVATE_MODULEPARTBASE_H_
