#include "SignatureCalculator.h"
#include <co/IEnum.h>
#include <co/IArray.h>
#include <co/IMethod.h>
#include <co/IStruct.h>
#include <co/ITypeManager.h>
#include <co/IInterface.h>
#include <co/IField.h>
#include <co/IParameter.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/INativeClass.h>

namespace co {

namespace {
const int8 SIGNATURE_VERSION = 1;  // change this to change all signatures
const int8 DELIMITER = ';';        // delimiter element to avoid ambiguities
}

SignatureCalculator::SignatureCalculator(IType* type) : _type(type) {
  assert(_type);
}

SignatureCalculator::~SignatureCalculator() {
  // empty
}

void SignatureCalculator::calculate() { calculateSignatures(); }

void SignatureCalculator::calculateSignatures() {
  addDataToSignatures(SIGNATURE_VERSION);

  TypeKind kind = _type->getKind();
  addDataToSignatures(kind);

  _fullSignatureHash.add(_type->getFullName());

  switch (kind) {
    case TK_BOOL:
    case TK_INT8:
    case TK_INT16:
    case TK_INT32:
    case TK_INT64:
    case TK_DOUBLE:
      break;
    case TK_ENUM:
      fillEnumSignatureData();
      break;
    case TK_STRING:
    case TK_ANY:
      break;
    case TK_ARRAY:
      fillArraySignatureData();
      break;
    case TK_STRUCT:
      fillStructSignatureData();
      break;
    case TK_NATIVECLASS:
      fillNativeClassSignatureData();
      break;
    case TK_INTERFACE:
      fillInterfaceSignatureData();
      break;
    case TK_COMPONENT:
      fillComponentSignatureData();
      break;
    case TK_EXCEPTION:
      break;
    default:
      assert(false);
  }

  _fullSignatureHash.finish();
  _fullSignatureHash.getDigest(_fullSignature);

  _binarySignatureHash.finish();
  _binarySignatureHash.getDigest(_binarySignature);
}

void SignatureCalculator::fillArraySignatureData() {
  IArray* array = static_cast<IArray*>(_type);
  IType* elementType = array->getElementType();
  _fullSignatureHash.add(elementType->getFullSignature());
  _binarySignatureHash.add(elementType->getBinarySignature());
}

void SignatureCalculator::fillEnumSignatureData() {
  IEnum* enumType = static_cast<IEnum*>(_type);
  TSlice<std::string> range = enumType->getIdentifiers();
  for (; range; range.popFirst()) {
    addDataToSignatures(range.getFirst());

    // add a delimiter to avoid ambiguities
    addDataToSignatures(DELIMITER);
  }
}

void SignatureCalculator::fillStructSignatureData() {
  IStruct* structType = static_cast<IStruct*>(_type);
  TSlice<IField*> fields = structType->getFields();
  for (; fields; fields.popFirst()) {
    IField* ai = fields.getFirst();
    assert(!ai->getIsReadOnly());

    IType* type = ai->getType();
    _fullSignatureHash.add(type->getFullSignature());
    _binarySignatureHash.add(type->getBinarySignature());

    _fullSignatureHash.add(ai->getName());
  }
}

void SignatureCalculator::fillNativeClassSignatureData() {
  INativeClass* nativeType = static_cast<INativeClass*>(_type);
  CORAL_UNUSED(nativeType);
  fillClassTypeData();
}

void SignatureCalculator::fillInterfaceSignatureData() {
  _binarySignatureHash.add(_type->getFullName());

  // add the baseType signature
  IInterface* baseType = static_cast<IInterface*>(_type)->getBaseType();
  if (baseType) {
    _fullSignatureHash.add(baseType->getFullSignature());
    _binarySignatureHash.add(baseType->getBinarySignature());
  }

  fillClassTypeData();
}

void SignatureCalculator::fillComponentSignatureData() {
  IComponent* component = static_cast<IComponent*>(_type);

  TSlice<IPort*> interfaces = component->getPorts();
  for (; interfaces; interfaces.popFirst()) {
    IPort* itf = interfaces.getFirst();

    const int8 isFacet = itf->getIsFacet() ? 1 : 0;
    addDataToSignatures(isFacet);

    IType* type = itf->getType();
    _fullSignatureHash.add(type->getFullSignature());
    _binarySignatureHash.add(type->getBinarySignature());

    addDataToSignatures(itf->getName());
  }
}

void SignatureCalculator::fillClassTypeData() {
  IClassType* classType = static_cast<IClassType*>(_type);

  TSlice<IField*> fields = classType->getFields();
  for (; fields; fields.popFirst()) {
    IField* ai = fields.getFirst();

    const int8 isReadOnly = ai->getIsReadOnly() ? 1 : 0;
    addDataToSignatures(isReadOnly);

    addTypeWithNoSignature(ai->getType());
    addDataToSignatures(ai->getName());
  }

  TSlice<IMethod*> methods = classType->getMethods();
  for (; methods; methods.popFirst()) {
    IMethod* mi = methods.getFirst();

    IType* returnType = mi->getReturnType();
    if (returnType)
      addTypeWithNoSignature(returnType);
    else
      addDataToSignatures(returnType);

    addDataToSignatures(mi->getName());

    TSlice<IParameter*> parameters = mi->getParameters();
    for (; parameters; parameters.popFirst()) {
      IParameter* pi = parameters.getFirst();

      const int8 isIn = pi->getIsIn() ? 1 : 0;
      addDataToSignatures(isIn);

      const int8 isOut = pi->getIsOut() ? 1 : 0;
      addDataToSignatures(isOut);

      addTypeWithNoSignature(pi->getType());

      _fullSignatureHash.add(pi->getName());
    }
  }
}

void SignatureCalculator::addTypeWithNoSignature(IType* type) {
  const int8 kind = type->getKind();
  addDataToSignatures(kind);
  addDataToSignatures(type->getFullName());
}

}  // namespace co
