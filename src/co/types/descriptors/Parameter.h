#ifndef _PARAMETERINFO_H_
#define _PARAMETERINFO_H_

#include "Parameter_Base.h"
#include <co/RefPtr.h>
#include <co/Platform.h>

namespace co {

/*!
	Implements co.IParameter.
 */
class Parameter : public Parameter_Base
{
public:
	Parameter();
	virtual ~Parameter();

	// internal methods:
	void init( const std::string& name, IType* type, bool isIn, bool isOut );
	void setName( const std::string& name );
	void setType( IType* type );
	void setIsIn( bool isIn );
	void setIsOut( bool isOut );

	// IParameter methods
	std::string getName();
	IType* getType();
	bool getIsIn();
	bool getIsOut();

private:
	std::string _name;
	IType* _type;
	bool _isIn;
	bool _isOut;
};

} // namespace co

#endif
