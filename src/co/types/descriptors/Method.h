#ifndef _CO_METHOD_H_
#define _CO_METHOD_H_

#include "Member.h"
#include "Method_Base.h"
#include <co/IException.h>
#include <co/IParameter.h>

namespace co {

/*!
	Implements co.IMethod.
 */
class Method : public Member<Method_Base>
{
public:
	virtual ~Method();

	// internal methods:
	void setReturnType( IType* returnType );
	void setParameters( Slice<IParameter*> parameters );
	void setExceptions( Slice<IException*> exceptions );

	// IMember methods:
	MemberKind getKind();

	// IMethod methods:
	IType* getReturnType();
	TSlice<IParameter*> getParameters();
	TSlice<IException*> getExceptions();

private:
	IType* _returnType;

	typedef std::vector<IParameterRef> ParameterVector;
	ParameterVector _parameters;

	typedef std::vector<IException*> ExceptionVector;
	ExceptionVector _exceptions;
};

} // namespace co

#endif
