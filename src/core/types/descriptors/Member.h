#ifndef _CO_MEMBER_H_
#define _CO_MEMBER_H_

#include "Type.h"
#include <co/IMember.h>

namespace co {

/*!
	Re-usable implementation of co::IMember.
 */
template<class Base>
class Member : public Annotated<Base>
{
public:
	Member() : _owner( 0 ), _index( -1 )
	{;}

	// internal methods:
	void setName( const std::string& name )
	{
		_name = name;
	}

	void setOwner( ICompositeType* owner, size_t index )
	{
		_owner = owner;

		assert( index < UINT32_MAX );
		_index = static_cast<uint16>( index );
	}

	// co::IMember methods:
	std::string getName() { return _name; }
	ICompositeType* getOwner() { return _owner; }
	uint16 getIndex() { return _index; }

private:
	std::string _name;
	ICompositeType* _owner;
	uint16 _index;
};

} // namespace co

#endif
