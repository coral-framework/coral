#ifndef _TYPE_H_
#define _TYPE_H_

#include "Type_Base.h"
#include <co/INamespace.h>
#include <co/IReflector.h>
#include <co/IAnnotation.h>

namespace co {

IAnnotation* findAnnotation( Slice<IAnnotation*> annotations, IInterface* annotationType );

/*!
	Re-usable implementation of co::IAnnotated.
 */
template<class Base>
class Annotated : public Base
{
public:
	TSlice<IAnnotation*> getAnnotations()
	{
		return _annotations;
	}

	void setAnnotations( Slice<IAnnotation*> annotations )
	{
		return co::assign( annotations, _annotations );
	}

	inline void addAnnotation( IAnnotation* annotation )
	{
		_annotations.push_back( annotation );
	}

	IAnnotation* findAnnotation( IInterface* requestedType )
	{
		return co::findAnnotation( _annotations, requestedType );
	}

private:
	std::vector<IAnnotationRef> _annotations;
};

/*!
	Re-usable implementation of IType.
 */
class TypeImpl
{
public:
	inline TypeImpl() : _namespace( NULL ), _kind( TK_NULL ),
		_hasSignatures( false ), _isCalculatingSignatures( false )
	{;}

	// Places the type into its namespace (kind/name must already be set).
	void setNamespace( INamespace* ns );

	// Defines the type's kind, name and places it into its namespace.
	void setType( TypeKind kind, const std::string& name, INamespace* ns );

	IReflector* getReflector( IType* myType );
	void setReflector( IReflector* reflector );

protected:
	void calculateSignatures( IType* myType );

protected:
	INamespace* _namespace;
	std::string _name;
	std::string _fullName;
	TypeKind _kind;

	bool _hasSignatures;			// whether _fullSignature and _binarySignature are valid
	bool _isCalculatingSignatures;	// to detect cycles in the signatures algorithm

	std::string _fullSignature;
	std::string _binarySignature;

	RefPtr<IReflector> _reflector;
};

template<class Base>
class Type : public Annotated<Base>, public TypeImpl
{
public:
	std::string getName() { return _name; }
	std::string getFullName() { return _fullName; }
	INamespace* getNamespace() { return _namespace; }
	TypeKind getKind() { return _kind; }

	std::string getFullSignature()
	{
		if( !_hasSignatures )
			calculateSignatures( this );
		return _fullSignature;
	}
	
	std::string getBinarySignature()
	{
		if( !_hasSignatures )
			calculateSignatures( this );
		return _binarySignature;
	}

	IReflector* getReflector() { return TypeImpl::getReflector( this ); }
	void setReflector( IReflector* reflector ) { _reflector = reflector; }

	IReflector* getCurrentReflector() { return _reflector.get(); }

	bool isA( IType* type ) { return this == type; }
};

/*!
	Implements co.Type.
 */
class TypeComponent : public Type<Type_Base>
{
public:
	// Default constructor.
	inline TypeComponent() {;}

	// Constructor for basic types in the root namespace.
	TypeComponent( TypeKind kind );

	virtual ~TypeComponent();
};

} // namespace co

#endif
