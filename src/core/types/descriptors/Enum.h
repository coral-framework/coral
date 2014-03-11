#ifndef _ENUMTYPE_H_
#define _ENUMTYPE_H_

#include "Type.h"
#include "Enum_Base.h"

namespace co {

/*!
	Implements co.IEnum.
 */
class Enum : public Type<Enum_Base>
{
public:
	virtual ~Enum();

	// internal methods:
	void addIdentifier( const std::string& identifier );

	// IEnum methods:
	TSlice<std::string> getIdentifiers();
	int32 getValueOf( const std::string& identifier );

private:
	typedef std::vector<std::string> IdentifierVector;
	IdentifierVector _identifiers;
};

} // namespace co

#endif
