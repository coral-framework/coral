#ifndef _CO_TYPEBUILDER_H_
#define _CO_TYPEBUILDER_H_

#include "Namespace.h"
#include "TypeBuilder_Base.h"
#include <co/RefPtr.h>

namespace co {

// Forward Decls:
class TypeImpl;
class IField;
class IPort;
class IInterface;
class Method;

/*!
	Implements co.ITypeBuilder.
 */
class TypeBuilder : public TypeBuilder_Base
{
public:
	/*!
		Factory method.
		\pre isCustom( kind ).
	 */
	static ITypeBuilder* create( TypeKind kind, INamespace* ns, const std::string& name );

public:
	virtual ~TypeBuilder();

	// Internal methods:

	// Called from IMethodBuilder to add a method definition.
	virtual void addMethod( Method* methodInfo );

	/*
		Called after createType() is called for all types in a transaction,
		to validate the final state of the types before they are committed.		
	 */
	virtual void validate();

	/*
		Executed once all types in the transaction have been validated and
		we're sure that everything is going to be committed.
	 */
	virtual void commit();

	/*
		The transaction failed. Destroys and removes all references to our type.
		The type must not have been committed yet.
	 */
	virtual void rollback();

	// ITypeBuilder methods:
	INamespace* getNamespace();
	TypeKind getKind();
	std::string getTypeName();
	void defineIdentifier( const std::string& name );
	void defineField( const std::string& name, IType* type, bool isReadOnly );
	void defineBaseType( IType* superType );
	void definePort( const std::string& name, IInterface* type, bool isFacet );
	IMethodBuilder* defineMethod( const std::string& name );
	IType* createType();

protected:
	TypeBuilder( TypeKind kind );

	//! Common initialization code, triggers the pre-allocation of our (still empty) type.
	void initialize( INamespace* ns, const std::string& name );

	//! Throws an exception if the type was already created (no further definitions allowed).
	void assertNotCreated();

	/*!
		Template method that guarantees the type to be construction by this builder is allocated.
		Returns true if the type was freshly allocated, or false if it was pre-allocated elsewhere
		(currently only 'co.IService' is pre-allocated).
	 */
	virtual bool allocateType() = 0;

	/*!
		Template method to populate a type with all the provided data.
		If not all required data has been provided, a MissingInputException is raised.
	 */
	virtual void fillType() = 0;

protected:
	TypeKind _kind;
	RefPtr<Namespace> _namespace;
	std::string _name;

	RefPtr<IType> _type;
	bool _typeWasCreated;
};

} // namespace co

#endif
