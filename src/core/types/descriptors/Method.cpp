#include "Method.h"

namespace co {

Method::~Method()
{
	// empty
}

void Method::setReturnType( IType* returnType )
{
	_returnType = returnType;
}

void Method::setParameters( Slice<IParameter*> parameters )
{
	assert( _parameters.size() == 0 );
	_parameters.reserve( parameters.getSize() );
	for( ; parameters; parameters.popFirst() )
		_parameters.push_back( parameters.getFirst() );
}

void Method::setExceptions( Slice<IException*> exceptions )
{
	assert( _exceptions.size() == 0 );
	_exceptions.reserve( exceptions.getSize() );
	for( ; exceptions; exceptions.popFirst() )
		_exceptions.push_back( exceptions.getFirst() );
}

MemberKind Method::getKind()
{
	return MK_METHOD;
}

IType* Method::getReturnType()
{
	return _returnType;
}

TSlice<IParameter*> Method::getParameters()
{
	return _parameters;
}

TSlice<IException*> Method::getExceptions()
{
	return _exceptions;
}

CORAL_EXPORT_COMPONENT( Method, Method );

} // namespace co
