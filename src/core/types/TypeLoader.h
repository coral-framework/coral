#ifndef _CO_TYPELOADER_H_
#define _CO_TYPELOADER_H_

#include "csl/Loader.h"
#include <co/RefPtr.h>

namespace co {

class IType;
class INamespace;
class ITypeBuilder;
class ITypeManager;
class TypeManager;

/*!
	Locates and loads a co::IType from a CSL file in the filesystem.

	The type is identified by the fully-qualified \a typeName passed in the
	constructor. The CSL file where the type is specified should reside in one of
	the type repositories specified in the 'path' parameter, also in the constructor.

	Please notice that a single call to loadType() may cause multiple types to be loaded,
	since all dependencies of the loaded type will be automatically loaded in a
	depth-first, recursive manner.

	All search for dependencies and type creation is done within the context of a given
	ITypeManager, which should also be passed in the constructor.
 */
class CORAL_EXPORT TypeLoader : public csl::Loader
{
public:
	/*!
		Creates a loader for the type with the given \a fullTypeName.
		Types are looked up and created within the context of a \a typeManager.
	 */
	TypeLoader( const std::string& fullTypeName, ITypeManager* typeManager );

	//! Destructor.
	virtual ~TypeLoader();

	/*!
		Effectively loads the type (and its dependencies) from the filesystem.

		The CSL file is parsed and any type dependencies are loaded	recursively. If the
		file is found in the filesystem and no error occurs while parsing the CSL files,
		the loaded co::IType instance is returned. Otherwise, this method returns NULL, and
		the exact error stack can be obtained through the getError() method.

		\warning If any error occurs while loading a type, this method returns NULL
			and all partially constructed types are rolled back.
	 */
	IType* loadType();

private:
	// Creates a 'non-root' loader for loading typeh dependencies recursively.
	TypeLoader( const std::string& fullTypeName, TypeLoader* parent );

	// Returns whether this is a root loader.
	inline bool isRootLoader() const { return _parentLoader == NULL; }

	// Template method implementation: instantiates a ITypeBuilder for use by the Parser.
	ITypeBuilder* createTypeBuilder( const std::string& typeName, TypeKind kind );

	/*
		Template method implementation: finds or loads a type dependency, given the
		type's full or relative name. Here's the algorithm:
		  1) search for an imported type aliased as 'typeName';
		  2) search for a type named 'typeName' in the current namespace;
		  3) search for a type named 'typeName' in the root namespace;
		  4) try to load the type using the current path.
		  5) return NULL in case of error parsing the CSL file or if the file
			 is not found. Use getError() to access the error stack.
	 */
	IType* resolveType( const csl::location& loc, const std::string& typeName, bool isArray = false );

	// Template method implementation: creates or gets a cached default annotation instance.
	IAnnotation* getDefaultAnnotationInstance( IType* type );

	/*
		Searches an existing type with the passed \a typeName. The name can be fully qualified
		or relative to the current type's location. Returns NULL if the type is not found.
	 */
	IType* findDependency( const std::string& typeName );

	/*
		Tries to load the type with the passed \a typeName. The name can be fully qualified
		or relative to the current type's location. An exception is thrown if the type's CSL
		file is not found. Otherwise, this method may return NULL if there's an error parsing
		the CSL file.
	 */
	IType* loadDependency( const std::string& typeName );

	// Searches for a CSL file in the Coral Path. Returns true if the file was found.
	bool findCSL( const std::string& typeName, std::string& fullPath, std::string& relativePath );

private:
	std::string _fullTypeName;
	TypeManager* _typeManager;
	TypeLoader* _parentLoader;
	INamespace* _namespace;

	typedef std::map<IType*, RefPtr<IAnnotation> > DefaultAnnotationMap;
	DefaultAnnotationMap _defaultAnnotationInstances;
};

} // namespace co

#endif
