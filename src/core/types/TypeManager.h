#ifndef _CO_TYPEMANAGER_H_
#define _CO_TYPEMANAGER_H_

#include "Namespace.h"
#include "TypeManager_Base.h"
#include <co/RefPtr.h>

namespace co {

class TypeBuilder;

/*!
	Implements co.ITypeManager.
 */
class TypeManager : public TypeManager_Base
{
public:
	TypeManager();
	virtual ~TypeManager();

	// internal methods:
	void initialize();
	void tearDown();
	void addTypeBuilder( ITypeBuilder* tb );

	// ITypeManager methods:
	INamespace* getRootNS();
	ITypeTransaction* getTransaction();
	INamespace* getNamespace( const std::string& fullName );
	INamespace* findNamespace( const std::string& fullName );
	IType* getType( const std::string& typeName );
	IType* findType( const std::string& fullName );
	IArray* getArrayOf( IType* elementType );
	IType* loadType( const std::string& typeName, std::vector<CSLError>& errorStack );

private:
	IType* loadTypeOrThrow( const std::string& fullName );
	IArray* defineArrayType( IType* elementType );
	void defineBuiltInTypes();

private:
	RefPtr<Namespace> _rootNS;
	RefPtr<ITypeTransaction> _transaction;
};

} // namespace co

#endif
