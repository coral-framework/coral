#include "Field.h"

namespace co {

Field::Field()
	: _type( 0 ), _isReadOnly( true )
{
	// empty
}

Field::~Field()
{
	// empty
}

void Field::setType( IType* type )
{
	_type = type;
}

void Field::setIsReadOnly( bool isReadOnly )
{
	_isReadOnly = isReadOnly;
}

MemberKind Field::getKind()
{
	return MK_FIELD;
}

IType* Field::getType()
{
	return _type;
}

bool Field::getIsReadOnly()
{
	return _isReadOnly;
}

CORAL_EXPORT_COMPONENT( Field, Field );

} // namespace co
