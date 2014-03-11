#ifndef _CO_PORT_H_
#define _CO_PORT_H_

#include "Member.h"
#include "Port_Base.h"

namespace co {

/*!
	Implements co.IPort.
 */
class Port : public Member<Port_Base>
{
public:
	Port();
	virtual ~Port();

	// internal methods:
	void setType( IInterface* type );
	void setIsFacet( bool isFacet );

	// IMember methods:
	MemberKind getKind();

	// IPort methods:
	IInterface* getType();
	bool getIsFacet();

private:
	IInterface* _type;
	bool _isFacet;
};

} // namespace co

#endif
