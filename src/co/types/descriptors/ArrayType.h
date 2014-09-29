#ifndef _ARRAYTYPE_H_
#define _ARRAYTYPE_H_

#include "Type.h"
#include "ArrayType_Base.h"

namespace co {

/*!
	Implements co.IArray.
 */
class ArrayType : public Type<ArrayType_Base>
{
public:
	virtual ~ArrayType();

	// internal methods:
	void setElementType( IType* type );

	// co::IArray methods:
	IType* getElementType();

private:
	RefPtr<IType> _elementType;
};

} // namespace co

#endif
