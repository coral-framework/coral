#ifndef _CO_SIGNATURECALCULATOR_H_
#define _CO_SIGNATURECALCULATOR_H_

#include <co/Platform.h>
#include "../utils/CryptoHash.h"

namespace co {

class IType;
class IMethod;

/*!
	Calculates version hashes for binary or full type compatibility checking.

	The \b binary signature keeps track of changes that break binary compatibility between
	versions of a C++ type mapping, while the \b full signature also keeps track of changes
	that could break source (or reflective) code compatibility (e.g. renaming struct fields).

	\par References:
		- http://techbase.kde.org/Policies/Binary_Compatibility_Issues_With_C++
		- http://techbase.kde.org/Policies/Binary_Compatibility_Examples
		- http://labs.trolltech.com/blogs/2009/08/12/some-thoughts-on-binary-compatibility/

	\par Implementation Details:
		- Cannot raise exceptions.
		- Signatures are computed on demand within the type components.
		- Based on SHA-1 hashing of the data available through the public type API.
		- A type's signature cannot depend on another type's signature, except
			in places where mutual dependencies are guaranteed to never occur.
 */
class SignatureCalculator
{
public:
	//! Type for whom we should calculate, and on which we should set, the signatures.
	SignatureCalculator( IType* type );

	//! Destructor.
	~SignatureCalculator();

	//! Calculates and sets the type's signatures.
	void calculate();

	//! Returns the calculated full signature.
	inline const Uuid& getFullSignature() const { return _fullSignature; }

	//! Returns the calculated binary signature.
	inline const Uuid& getBinarySignature() const { return _binarySignature; }

private:
	template<typename T>
	inline void addDataToSignatures( const T& data )
	{
		_fullSignatureHash.addData( data );
		_binarySignatureHash.addData( data );
	}

	void calculateSignatures();

	void fillArraySignatureData();
	void fillEnumSignatureData();
	void fillStructSignatureData();
	void fillNativeClassSignatureData();
	void fillInterfaceSignatureData();
	void fillComponentSignatureData();
	void fillClassTypeData();

	/*!
		Adds just enough data about a type. For places where using the
		type's signature would imply in mutual dependencies (i.e. method
		parameters and return types).
	 */
	void addTypeWithNoSignature( IType* type );

private:
	IType* _type;

	Uuid _fullSignature;
	Uuid _binarySignature;

	CryptoHash _fullSignatureHash;
	CryptoHash _binarySignatureHash;
};

} // namespace co

#endif
