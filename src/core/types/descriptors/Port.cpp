#include "Port.h"

namespace co {

Port::Port() : _type( 0 ), _isFacet( true )
{
	// empty
}


Port::~Port()
{
	// empty
}

void Port::setType( IInterface* type )
{
	_type = type;
}

void Port::setIsFacet( bool isFacet )
{
	_isFacet = isFacet;
}

MemberKind Port::getKind()
{
  return MemberKind::Port;
}

IInterface* Port::getType()
{
	return _type;
}

bool Port::getIsFacet()
{
	return _isFacet;
}

CORAL_EXPORT_COMPONENT( Port, Port );

} // namespace co
