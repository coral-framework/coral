#ifndef _CO_NAMESPACE_H_
#define _CO_NAMESPACE_H_

#include "Namespace_Base.h"
#include <co/IType.h>
#include <co/IModule.h>

namespace co {

/*!
	Implements co.INamespace.
 */
class Namespace : public Namespace_Base
{
public:
	Namespace();
	virtual ~Namespace();

	// internal methods:
	void setParentAndName( INamespace* parent, const std::string& name );
	void setModule( IModule* module );
	void addType( IType* type );
	void removeType( IType* type );

	// INamespace methods:
	std::string getName();
	std::string getFullName();
	INamespace* getParentNamespace();
	TSlice<IType*> getTypes();
	TSlice<INamespace*> getChildNamespaces();
	IModule* getModule();
	IType* getType( const std::string& name );
	INamespace* getChildNamespace( const std::string& name );
	ITypeBuilder* defineType( const std::string& name, TypeKind kind );
	INamespace* defineChildNamespace( const std::string& name );

private:
	std::string getNamespaceName();
	void throwClashingType( const std::string& name );
	void throwClashingNamespace( const std::string& name );

	inline static int typeComparator( const std::string& name, IType* type )
	{
		return name.compare( type->getName() );
	}

	inline IType* findType( const std::string& name )
	{
		size_t pos;
		if( binarySearch( Slice<IType*>( _types ), name, typeComparator, pos ) )
			return _types[pos].get();
		return NULL;
	}

	inline static int namespaceComparator( const std::string& name, INamespace* ns )
	{
		return name.compare( ns->getName() );
	}

	inline INamespace* findChildNamespace( const std::string& name )
	{
		size_t pos;
		if( binarySearch( Slice<INamespace*>( _childNamespaces ), name, namespaceComparator, pos ) )
			return _childNamespaces[pos].get();
		return NULL;
	}

private:
	std::string _name;
	std::string _fullName;
	INamespace* _parent;
	IModuleRef _module;
	std::vector<ITypeRef> _types;
	std::vector<INamespaceRef> _childNamespaces;
};

} // namespace co

#endif
