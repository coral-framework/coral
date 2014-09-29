#ifndef _CO_INTERFACE_H_
#define _CO_INTERFACE_H_

#include "ClassType.h"
#include "Interface_Base.h"

namespace co {

/*!
	Implements co.IInterface.
 */
class Interface : public ClassType<Interface_Base>
{
public:
	Interface();
	virtual ~Interface();

	// internal methods:
	void setBaseType( IInterface* base );
	void addSubType( IInterface* sub );
	void updateSuperTypes();

	// IType methods:
	bool isA( IType* type );

	// ICompositeType methods:
	IMember* getMember( const std::string& name );

	// IInterface methods:
	IInterface* getBaseType();
	TSlice<IInterface*> getSuperTypes();
	TSlice<IInterface*> getSubTypes();

private:
	IInterface* _baseType;
	size_t _numSuperTypes;
	IInterface** _superTypes;
	std::vector<IInterface*> _subTypes;
};

} // namespace co

#endif
