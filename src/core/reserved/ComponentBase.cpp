#include "ComponentBase.h"
#include "../types/BasicReflector.h"
#include <co/Log.h>
#include <co/Coral.h>
#include <co/RefPtr.h>
#include <co/ISystem.h>
#include <co/INamespace.h>
#include <co/ITypeBuilder.h>
#include <co/ITypeManager.h>
#include <co/ICompositeType.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>
#include <co/ITypeTransaction.h>
#include <co/NoSuchPortException.h>

namespace co {

ComponentBase::~ComponentBase() {
  if (_refCount != 0) {
    CORAL_LOG(FATAL) << "Deleting component " << this
                     << " with a reference count of " << _refCount << ".";
  }
}

IInterface* ComponentBase::getInterface() { return typeOf<IObject>::get(); }

IPort* ComponentBase::getFacet() {
  // the 'object' facet is implicit and has no IPort
  return NULL;
}

void ComponentBase::checkValidPort(IPort* port) {
  if (!port) throw NoSuchPortException("illegal null port");

  IComponent* myType = getComponent();
  ICompositeType* owner = port->getOwner();
  if (owner != static_cast<ICompositeType*>(myType))
    CORAL_THROW(NoSuchPortException, "port '"
                                         << port->getName() << "' belongs to "
                                         << owner->getFullName() << ", not to "
                                         << myType->getFullName());

  assert(port->getIndex() < static_cast<int16>(myType->getPorts().getSize()));
}

void ComponentBase::checkValidReceptacle(IPort* receptacle) {
  checkValidPort(receptacle);
  if (receptacle->getIsFacet())
    throw NoSuchPortException("receptacle expected, got facet");
}

void ComponentBase::raiseUnexpectedPortIndex() {
  assert(false);
  throw NoSuchPortException("unexpected invalid port index");
}

IComponent* ComponentBase::getOrCreateInternalComponent(
    const char* componentName, const char* interfaceName,
    const char* facetName) {
  std::string fullTypeName(componentName);

  // get the component if it already exists
  ITypeManager* tm = getSystem()->getTypes();
  IType* type = tm->findType(fullTypeName);
  if (type) {
    assert(type->getKind() == TK_COMPONENT);
    return static_cast<IComponent*>(type);
  }

  // otherwise, create the component
  IType* itfType = getType(interfaceName);
  assert(itfType->getKind() == TK_INTERFACE);

  size_t lastDotPos = fullTypeName.rfind('.');
  assert(
      lastDotPos !=
      std::string::npos);  // componentName must be specified with a namespace

  std::string namespaceName(fullTypeName.substr(0, lastDotPos));
  std::string localTypeName(fullTypeName.substr(lastDotPos + 1));

  INamespace* ns = tm->findNamespace(namespaceName);
  assert(ns);  // the namespace should have been created before

  RefPtr<ITypeBuilder> tb = ns->defineType(localTypeName, TK_COMPONENT);
  tb->definePort(facetName, static_cast<IInterface*>(itfType), true);

  try {
    tm->getTransaction()->commit();
  } catch (std::exception&) {
    tm->getTransaction()->rollback();
    throw;
  }

  type = tb->createType();
  assert(type && type->getKind() == TK_COMPONENT);

  // set the component with a basic reflector
  type->setReflector(BasicReflector::create(type));

  return static_cast<IComponent*>(type);
}

}  // namespace 'co'
