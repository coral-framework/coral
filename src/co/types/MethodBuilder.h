#ifndef _CO_METHODBUILDER_H_
#define _CO_METHODBUILDER_H_

#include "MethodBuilder_Base.h"
#include <co/IParameter.h>

namespace co {

class IType;
class IMethod;
class ITypeBuilder;
class IException;

/*!
	Implements co.IMethodBuilder.
 */
class MethodBuilder : public MethodBuilder_Base
{
public:
	MethodBuilder();
	virtual ~MethodBuilder();

	// internal methods:
	void init( ITypeBuilder* typeBuilder, const std::string& name );

	// IMethodBuilder methods:
	ITypeBuilder* getTypeBuilder();
	std::string getMethodName();
	void defineReturnType( IType* type );
	void defineParameter( const std::string& name, IType* type, bool input, bool output );
	void defineException( IException* exception );
	void createMethod();

private:
	std::string _name;
	RefPtr<ITypeBuilder> _typeBuilder;
	RefPtr<IMethod> _createdMethodInfo;

	IType* _returnType;
	std::vector<IParameterRef> _parameters;
	std::vector<IException*> _expectedExceptions;
};

} // namespace co

#endif
