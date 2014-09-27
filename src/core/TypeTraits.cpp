#include "TypeTraits.h"
#include "types/descriptors/Type.h"
#include <co/Coral.h>
#include <co/IPort.h>
#include <co/IObject.h>
#include <co/IInterface.h>
#include <co/IComponent.h>
#include <co/NoSuchPortException.h>
#include <co/IllegalCastException.h>
#include <sstream>

namespace co {

const std::string TK_STRINGS[] = {
    "null",      "bool",         "int8",         "int16",     "int32",
    "int64",     "double",       "enum",         "string",    "any",
    "array",     "struct",       "native class", "interface", "component",
    "exception", "<off by one!>"};

RefPtr<IType> BASIC_TYPES[] = {
    new TypeComponent(TK_NULL),   new TypeComponent(TK_BOOL),
    new TypeComponent(TK_INT8),   new TypeComponent(TK_INT16),
    new TypeComponent(TK_INT32),  new TypeComponent(TK_INT64),
    new TypeComponent(TK_DOUBLE), NULL,  // TK_ENUM
    new TypeComponent(TK_STRING), new TypeComponent(TK_ANY),
    NULL,  // TK_ARRAY
    NULL,  // TK_STRUCT
    NULL,  // TK_NATIVECLASS
    NULL,  // TK_INTERFACE
    NULL,  // TK_COMPONENT
    NULL,  // TK_EXCEPTION
    NULL   // off by one
};

IType* getTypeByLiteralName(const char* fullName) { return getType(fullName); }

TypeKind getKind(IType* type) {
  assert(type);
  return type->getKind();
}

IService* findServiceByType(IObject* object, IInterface* type) {
  assert(object && type);
  TSlice<IPort*> facets = object->getComponent()->getFacets();
  for (; facets; facets.popFirst())
    if (facets.getFirst()->getType()->isA(type))
      return object->getServiceAt(facets.getFirst());
  return NULL;
}

IService* getServiceByType(IObject* object, IInterface* type) {
  IService* res = findServiceByType(object, type);
  if (!res)
    CORAL_THROW(NoSuchPortException,
                "component " << object->getComponent()->getFullName()
                             << " has no facet of type '" << type->getFullName()
                             << "'")
  return res;
}

IService* findServiceByName(IObject* object, const std::string& portName) {
  assert(object);
  IMember* port = object->getComponent()->getMember(portName);
  return port ? object->getServiceAt(static_cast<IPort*>(port)) : NULL;
}

IService* getServiceByName(IObject* object, const std::string& portName) {
  IService* res = findServiceByName(object, portName);
  if (!res)
    CORAL_THROW(NoSuchPortException,
                "component " << object->getComponent()->getFullName()
                             << " has no port named '" << portName << "'");
  return res;
}

void setServiceByName(IObject* object, const std::string& receptacleName,
                      IService* service) {
  assert(object);
  IComponent* component = object->getComponent();
  IMember* port = component->getMember(receptacleName);
  if (!port)
    CORAL_THROW(NoSuchPortException, "no such receptacle '"
                                         << receptacleName << "' in component '"
                                         << component->getFullName() << "'");
  object->setServiceAt(static_cast<IPort*>(port), service);
}

bool isA(IService* service, IInterface* type) {
  assert(type);
  return !service || service->getInterface()->isA(type);
}

void ensureIsA(IService* service, IInterface* type) {
  assert(type);
  if (service && !service->getInterface()->isA(type))
    CORAL_THROW(IllegalCastException,
                "illegal cast from '" << service->getInterface()->getFullName()
                                      << "' to '" << type->getFullName()
                                      << "'");
}

}  // namespace co