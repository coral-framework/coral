#include "TypeManager.h"
#include "Namespace.h"
#include "TypeLoader.h"
#include "ModuleInstaller.h"
#include "TypeTransaction.h"
#include "descriptors/Type.h"
#include "descriptors/ArrayType.h"
#include "descriptors/Interface.h"
#include "../utils/StringTokenizer.h"
#include <co/CSLError.h>
#include <co/IInclude.h>
#include <co/TypeLoadException.h>
#include <co/IllegalArgumentException.h>
#include <sstream>

namespace co {

TypeManager::TypeManager() {
  _rootNS = new Namespace;
  _transaction = new TypeTransaction;
}

TypeManager::~TypeManager() {
  // empty
}

void TypeManager::initialize() {
  defineBuiltInTypes();

  // install the 'co' module
  ModuleInstaller::instance().install();

  /*
    Manually add the necessary annotations to core types.
    In the future we shall revamp reflectors to allow their use before any
    type is created, thus enabling core types to use the core annotations.
   */
  RefPtr<IObject> annotationObject = newInstance("co.IncludeAnnotation");
  IInclude* include = annotationObject->getService<IInclude>();
  include->setValue("co/reserved/Uuid.h");
  getType("co.Uuid")->addAnnotation(include);
}

void TypeManager::tearDown() {
  // uninstall the 'co' module
  ModuleInstaller::instance().uninstall();
}

void TypeManager::addTypeBuilder(ITypeBuilder* tb) {
  static_cast<TypeTransaction*>(_transaction.get())->addTypeBuilder(tb);
}

INamespace* TypeManager::getRootNS() { return _rootNS.get(); }

ITypeTransaction* TypeManager::getTransaction() { return _transaction.get(); }

template <bool create>
INamespace* getOrCreateNS(INamespace* ns, const std::string& fullName) {
  assert(ns);
  std::string token;
  StringTokenizer st(fullName, ".");
  while (st.getNext(token)) {
    INamespace* childNS = ns->findChildNamespace(token);
    if (!childNS) {
      if (create)
        childNS = ns->defineChildNamespace(token);
      else
        return NULL;
    }
    ns = childNS;
  }
  return ns;
}

INamespace* TypeManager::getNamespace(const std::string& fullName) {
  return getOrCreateNS<true>(_rootNS.get(), fullName);
}

INamespace* TypeManager::findNamespace(const std::string& fullName) {
  return getOrCreateNS<false>(_rootNS.get(), fullName);
}

IType* TypeManager::getType(const std::string& typeName) {
  if (typeName.empty())
    CORAL_THROW(IllegalArgumentException, "empty type name");

  // is typeName an array?
  if (typeName[typeName.length() - 1] == ']') {
    // get the elementType by removing the "[]" suffix
    IType* elementType =
        getType(std::string(typeName, 0, typeName.length() - 2));
    return getArrayOf(elementType);
  }

  IType* type = findType(typeName);
  if (type) return type;

  return loadTypeOrThrow(typeName);
}

IType* TypeManager::findType(const std::string& fullName) {
  INamespace* ns = _rootNS.get();
  assert(ns);
  std::string current, next;
  StringTokenizer st(fullName, ".");
  st.getNext(current);
  while (st.getNext(next)) {
    ns = ns->findChildNamespace(current);
    if (!ns) return NULL;
    std::swap(current, next);
  }
  return ns->findType(current);
}

IArray* TypeManager::getArrayOf(IType* elementType) {
  if (!elementType) CORAL_THROW(IllegalArgumentException, "null element type");

  const std::string& elementTypeName = elementType->getName();

  std::string arrayName;
  arrayName.reserve(elementTypeName.length() + 2);
  arrayName.append(elementTypeName);
  arrayName.append("[]");

  Namespace* ns = static_cast<Namespace*>(elementType->getNamespace());

  // try to locate an existing array of this type
  IType* existingArrayType = ns->getType(arrayName);
  if (existingArrayType) {
    assert(existingArrayType->getKind() == TK_ARRAY);
    return static_cast<IArray*>(existingArrayType);
  }

  // otherwise, try to create it
  TypeKind kind = elementType->getKind();
  if (kind == TK_ARRAY)
    CORAL_THROW(IllegalArgumentException, "arrays of arrays are illegal");

  if (!isData(kind))
    CORAL_THROW(IllegalArgumentException, "arrays of " << kind
                                                       << "s are illegal");

  RefPtr<ArrayType> arrayType = new ArrayType;
  arrayType->setType(TK_ARRAY, elementType->getName() + "[]", ns);
  arrayType->setElementType(elementType);

  ns->addType(arrayType.get());

  return arrayType.get();
}

IType* TypeManager::loadType(const std::string& typeName,
                             std::vector<CSLError>& errorStack) {
  IType* type = findType(typeName);
  if (type) return type;

  TypeLoader loader(typeName, this);

  type = loader.loadType();
  if (!type) {
    const csl::Error* currentError = loader.getError();
    while (currentError) {
      errorStack.push_back(CSLError());

      CSLError& cslError = errorStack.back();
      cslError.filename = currentError->getFileName();
      cslError.message = currentError->getMessage();
      cslError.line = currentError->getLine();

      currentError = currentError->getInnerError();
    }
  }

  return type;
}

IType* TypeManager::loadTypeOrThrow(const std::string& fullName) {
  TypeLoader loader(fullName, this);

  IType* type = loader.loadType();
  if (loader.getError())
    CORAL_THROW(TypeLoadException, "could not load type '"
                                       << fullName << "':\n"
                                       << *loader.getError());

  return type;
}

void TypeManager::defineBuiltInTypes() {
  Namespace* rootNS = _rootNS.get();

  // register all basic types in the root namespace:
  for (int8 k = 0; k < TK_COUNT; ++k) {
    TypeComponent* type = static_cast<TypeComponent*>(BASIC_TYPES[k].get());
    if (!type) continue;
    type->setNamespace(rootNS);
    rootNS->addType(type);
  }

  // pre-load the 'co.IService' type and all its dependencies
  Namespace* coNS = static_cast<Namespace*>(rootNS->defineChildNamespace("co"));
  assert(coNS);

  RefPtr<Interface> serviceType = new Interface;
  serviceType->setType(TK_INTERFACE, "IService", coNS);
  coNS->addType(serviceType.get());

  loadTypeOrThrow("co.IService");
}

CORAL_EXPORT_COMPONENT(TypeManager, TypeManager);

}  // namespace co
