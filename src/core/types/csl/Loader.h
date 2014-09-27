#ifndef _CO_CSL_LOADER_H_
#define _CO_CSL_LOADER_H_

#include "Error.h"
#include <co/Any.h>
#include <co/Coral.h>
#include <co/IAnnotation.h>
#include <co/ITypeBuilder.h>
#include <co/IMethodBuilder.h>
#include <co/IDocumentation.h>
#include <map>
#include <deque>

namespace co {

class IType;
class INamespace;
class ITypeTransaction;

namespace csl {

class Loader
{
public:
	Loader();
	virtual ~Loader();

	/*!
		Parses the given file. Never throws.
		Returns true on success, or false if there were errors.
	 */
	bool parse( const std::string& filename );

	//! Whether annotations are enabled.
	inline bool hasAnnotations() { return ( _cslFlags & CSL_ANNOTATIONS ) != 0; }

	//! Whether C++ blocks should be loaded.
	inline bool hasCppBlocks() { return ( _cslFlags & CSL_CPPBLOCKS ) != 0; }

	//! Whether documentation should be loaded.
	inline bool hasDoc() { return ( _cslFlags & CSL_DOCUMENTATION ) != 0; }

	//! Error handling:
	inline Error* getError() { return _error.get(); }
	inline const std::string* getCurrentFileName() { return _filename; }
	void pushError( const location& loc, const std::string& message );
	void pushError( const std::string& filename, const std::string& message );
	void pushError( const std::string& message );
	void setError( Error* error );

	//! Memory management:
	co::AnyValue* newAny();
	std::string* newString();
	std::string* newString( const char* cstr );

	//! Parser events
	void onComment( const location& loc, const std::string& text );
	void onCppBlock( const location& loc, const std::string& text );
	void onImport( const location& loc, const std::string& importTypeName );
	void onTypeSpec( const location& kLoc, TypeKind kind, const location& nLoc, const std::string& name );
	void onTypeDecl( const location& loc, const std::string& qualifiedId, bool isArray );
	void onEnumIdentifier( const location& loc, const std::string& name );
	void onBaseType( const location& loc, const std::string& typeName );
	void onPort( const location& loc, bool isFacet, const std::string& name );
	void onField( const location& loc, const std::string& name, bool isReadOnly );
	void onMethod( const location& loc, const std::string& name );
	void onParameter( const location& loc, bool isIn, bool isOut, const std::string& name );
	void onRaises( const location& loc, const std::string& name );
	void onMethodEnd( const location& loc );
	void onAnnotation( const location& loc, const std::string& name, bool hasData );
	void onAnnotationData( const location& loc, const std::string& fieldName, const co::Any& value );

protected:
	//! Template method: creates the ITypeBuilder for the type being parsed.
	virtual ITypeBuilder* createTypeBuilder( const std::string& typeName, TypeKind kind ) = 0;

	/*!
		Template method: resolves a type given its \a typeName and whether it is an array.
		\param typeName may be a full or relative type name.
	 */
	virtual IType* resolveType( const location& loc, const std::string& typeName, bool isArray = false ) = 0;

	//! Reusable method to extract an annotation from an object.
	IAnnotation* getAnnotationFrom( IObject* object );

	/*!
		Creates or gets the default (shared) instance of the given annotation type.
		This instance can be re-used at places where no annotation data is specified.
	 */
	virtual IAnnotation* getDefaultAnnotationInstance( IType* type ) = 0;

	//! Returns the type created by our ITypeBuilder.
	IType* getType();

	/*!
		Retrieves an imported IType* given its local type name (i.e. its alias).
		Returns NULL if the alias does not identify an imported type.
	 */
	IType* findImportedType( const std::string& alias );

private:
	struct ImportInfo
	{
		ImportInfo( const std::string& typeName, const location& location )
			: typeName( typeName ), loc( location ), type( NULL )
		{;}

		std::string typeName;	// full or relative type name
		location loc;			// location of the import clause
		IType* type;			// resolved type (only after calling resolveImports())
	};

	typedef std::map<std::string, ImportInfo> ImportTypeMap;
	ImportTypeMap _importedTypes;

	/*
		Resolves all types added to _importedTypes. This should be called after
		the type builder is created to avoid problems with cyclic dependencies.
	 */
	void resolveImports();

	/*
		Called when a new element (type, member or enum identifier) is parsed.
		This decorates the element with annotations and updates _lastElement.
	 */
	void onElement( const std::string& element );

	// Creates/populates an co::IDocumentation annotation for the type.
	void addDocumentation( const std::string& element, const std::string& text );

	IType* getLastDeclaredType();

private:
	const std::string* _filename;
	RefPtr<Error> _error;

	// base name of the CSL file (should match the type name)
	std::string _cslFileBaseName;

	RefPtr<ITypeBuilder> _typeBuilder;
	RefPtr<IMethodBuilder> _methodBuilder;

	int8 _cslFlags;

	bool _lastDeclTypeIsArray;
	location _lastDeclTypeLocation;
	std::string _lastDeclTypeName;

	// keeps track of the last defined element ("" means the type itself)
	std::string _lastElement;

	// accumulates C++ blocks
	std::string _cppBlock;

	// accumulates documentation
	std::string _docBuffer;

	// documentation annotation
	RefPtr<IDocumentation> _doc;

	// stores annotations until they can be transferred to the type/members
	struct AnnotationRecord
	{
		std::string element;
		std::vector<IAnnotationRef> annotations;
	};
	typedef std::deque<AnnotationRecord> AnnotationRecList;
	std::deque<AnnotationRecord> _annotations;

	// memory pools
	std::deque<co::AnyValue> _anyPool;
	std::deque<std::string> _stringPool;
};

} // namespace csl
} // namespace co

#endif // _CO_CSL_LOADER_H_
