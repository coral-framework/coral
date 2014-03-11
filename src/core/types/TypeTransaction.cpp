#include "TypeTransaction.h"
#include "TypeBuilder.h"
#include <co/Log.h>
#include <co/Coral.h>
#include <co/IType.h>
#include <co/ISystem.h>
#include <co/MissingInputException.h>
#include <co/NotSupportedException.h>
#include <sstream>
#include <iostream>

namespace co {

TypeTransaction::TypeTransaction()
{
	_commitAttempted = false;
	_commitSucceeded = false;
	_rolledBack = false;
}

TypeTransaction::~TypeTransaction()
{
	if( !_builders.empty() )
	{
		CORAL_LOG(FATAL) << "co.TypeTransaction not committed nor rolled back.";
	}
}

void TypeTransaction::addTypeBuilder( ITypeBuilder* typeBuilder )
{
	_builders.push_back( typeBuilder );
}

TSlice<ITypeBuilder*> TypeTransaction::getTypeBuilders()
{
	return _builders;
}

void TypeTransaction::commit()
{
	if( _builders.empty() )
		return;

	if( _commitAttempted )
	{
		assert( !_commitSucceeded && !_rolledBack );
		throw NotSupportedException( "a commit has failed, the transaction must be rolled back" );
	}

	_commitAttempted = true;

	// create all types not yet created
	for( TypeBuilderList::iterator it = _builders.begin(); it != _builders.end(); ++it )
		it->get()->createType();

	// validate all types
	for( TypeBuilderList::iterator it = _builders.begin(); it != _builders.end(); ++it )
		static_cast<TypeBuilder*>( it->get() )->validate();

	// commit all types
	for( TypeBuilderList::iterator it = _builders.begin(); it != _builders.end(); ++it )
		static_cast<TypeBuilder*>( it->get() )->commit();

	_commitSucceeded = true;

	reset();
}

void TypeTransaction::rollback()
{
	assert( !_commitSucceeded && !_rolledBack );

	// destroy all types
	for( TypeBuilderList::iterator it = _builders.begin(); it != _builders.end(); ++it )
		static_cast<TypeBuilder*>( it->get() )->rollback();

	_rolledBack = true;

	reset();
}

void TypeTransaction::reset()
{
	// make sure we are in a valid state to reset
	assert( _commitSucceeded || _rolledBack );

	_builders.clear();
	_commitAttempted = _commitSucceeded = _rolledBack = false;
}

CORAL_EXPORT_COMPONENT( TypeTransaction, TypeTransaction );

} // namespace co
