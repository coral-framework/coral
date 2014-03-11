#ifndef _CO_CLASSTYPE_H_
#define _CO_CLASSTYPE_H_

#include "Type.h"
#include <co/IMember.h>
#include <co/IClassType.h>
#include <co/IRecordType.h>
#include <co/ICompositeType.h>

namespace co {

/*!
	Reusable implementation for all class/record types.
 */
class ClassTypeImpl
{
public:
	inline ClassTypeImpl() : _firstMethodPos( -1 )
	{;}

	/*!
		Appends the given members range to the members list. After calling this
		method the members list must be re-sorted by calling sortMembers().
	 */
	void addMembers( Slice<IMember*> members );

	//! Sorts the members list by (type, name) and updates _firstMethodPos.
	void sortMembers( ICompositeType* owner );

	// ICompositeType methods:
	IMember* getMember( const std::string& name );

	// IRecordType methods:
	TSlice<IField*> getFields();

	// IClassType methods:
	TSlice<IMethod*> getMethods();

protected:
	typedef std::vector<IMemberRef> MembersVector;
	MembersVector _members;	// mixed container: fields first, then methods.
	size_t _firstMethodPos;	// dividing point: position of the first method in _members.
};
	
template<class Base>
class ClassType : public Type<Base>, public ClassTypeImpl
{
public:
	TSlice<IMember*> getMembers() { return _members; }
	IMember* getMember( const std::string& name ) { return ClassTypeImpl::getMember( name ); }
	TSlice<IField*> getFields() { return ClassTypeImpl::getFields(); }
	TSlice<IMethod*> getMethods() { return ClassTypeImpl::getMethods(); }
};

} // namespace co

#endif
