#include "ArrayType.h"

namespace co {

ArrayType::~ArrayType()
{
	// empty
}

void ArrayType::setElementType( IType* type )
{
	_elementType = type;
}

IType* ArrayType::getElementType()
{
	return _elementType.get();
}

CORAL_EXPORT_COMPONENT( ArrayType, ArrayType );

} // namespace co
