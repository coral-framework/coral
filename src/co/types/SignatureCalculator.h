#ifndef _CO_SIGNATURECALCULATOR_H_
#define _CO_SIGNATURECALCULATOR_H_

#include <co/Platform.h>
#include "SHA1.h"

namespace co {

class IType;
class IMethod;

/*!
  Calculates version hashes for binary or full type compatibility checking.

  The binary signature keeps track of changes that break binary compatibility
  between versions of a C++ type mapping, while the full signature also
  keeps track of changes that could break code compatibility (e.g. renaming
  fields).

  Implementation Details:
    - Cannot raise exceptions.
    - Signatures are computed on demand within the type components.
    - Based on SHA-1 hashing of the data available through the public type API.
    - A type's signature cannot depend on another type's signature, except
      in places where mutual dependencies are guaranteed to never occur.
 */
class SignatureCalculator {
 public:
  // Type for which we should calculate the signatures.
  SignatureCalculator(IType* type);

  //! Destructor.
  ~SignatureCalculator();

  //! Calculates and sets the type's signatures.
  void calculate();

  //! Returns the calculated full signature.
  inline const std::string& getFullSignature() const { return _fullSignature; }

  //! Returns the calculated binary signature.
  inline const std::string& getBinarySignature() const { return _binarySignature; }

 private:
  template <typename T> inline void addDataToSignatures(const T& data) {
    _fullSignatureHash.add(data);
    _binarySignatureHash.add(data);
  }

  void calculateSignatures();

  void fillArraySignatureData();
  void fillEnumSignatureData();
  void fillStructSignatureData();
  void fillNativeClassSignatureData();
  void fillInterfaceSignatureData();
  void fillComponentSignatureData();
  void fillClassTypeData();

  // Adds just enough data about a type. For places where using the
  // type's signature would imply in mutual dependencies (i.e. method
  // parameters and return types).
  void addTypeWithNoSignature(IType* type);

 private:
  IType* _type;

  std::string _fullSignature;
  std::string _binarySignature;

  SHA1 _fullSignatureHash;
  SHA1 _binarySignatureHash;
};

}  // namespace co

#endif
