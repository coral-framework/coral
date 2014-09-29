#include "Loader.h"
#include "scanner.hh"
#include "parser.hh"
#include "Annotations.h"
#include <co/IField.h>
#include <co/IObject.h>
#include <co/ICppBlock.h>
#include <co/IException.h>
#include <co/IInterface.h>
#include <co/INamespace.h>
#include <co/IReflector.h>
#include <co/ITypeBuilder.h>
#include <co/IDocumentation.h>
#include <co/IMethodBuilder.h>
#include <co/ITypeTransaction.h>
#include <co/TypeLoadException.h>
#include <co/IllegalArgumentException.h>
#include <co/private/OS.h>

#ifdef PUSH_ERROR
#undef PUSH_ERROR
#endif

#define PUSH_ERROR( loc, msg ) { std::stringstream errorSS; \
	errorSS << msg; pushError( loc, errorSS.str() ); }

#define CATCH_ERRORS( loc, code ) try { code; } \
	catch( Exception& e ) { pushError( loc, e.getMessage() ); return; }

namespace co {
namespace csl {

Loader::Loader()
{
	_cslFlags = getCSLFlags();
}

Loader::~Loader()
{
	// empty
}

bool Loader::parse( const std::string& filename )
{
	_filename = &filename;

	// extract the CSL file's base name
	size_t startPos = filename.rfind( CORAL_OS_DIR_SEP );
	startPos = ( startPos == std::string::npos ? 0 : startPos + 1 );
	size_t lastDotPos = filename.rfind( '.' );
	_cslFileBaseName.assign( filename, startPos, lastDotPos - startPos );

	// open file
	FILE* file = fopen( filename.c_str(), "rt" );
	if( !file )
	{
		pushError( filename, "could not open file" );
		return false;
	}

	yyscan_t scanner;
	csl_lex_init( &scanner );
	csl_set_in( file, scanner );

	try
	{
		Parser parser( *this, scanner );
		//parser.set_debug_level( 1 );
		if( parser.parse() != 0 )
		{
			assert( getError() );
			if( !getError() )
				pushError( filename, "unexpected parse error" );
		}
	}
	catch( std::exception& e )
	{
		pushError( filename, std::string( "unexpected exception: " ) + e.what() );
	}
	catch( ... )
	{		
		pushError( filename, "unexpected unknown exception" );
	}

	csl_lex_destroy( scanner );
	fclose( file );
	_filename = NULL;

	return !_error.isValid();
}

void Loader::pushError( const location& loc, const std::string& message )
{
	_error = new Error( loc, message, _error.get() );
}

void Loader::pushError( const std::string& filename, const std::string& message )
{
	_error = new Error( filename, message, _error.get() );
}	

void Loader::pushError( const std::string& message )
{
	_error = new Error( message, _error.get() );	
}

void Loader::setError( Error* error )
{
	assert( !_error.isValid() );
	_error = error;	
}

AnyValue* Loader::newAny()
{
	_anyPool.push_back( AnyValue() );
	return &_anyPool.back();
}

std::string* Loader::newString()
{
	_stringPool.push_back( std::string() );
	return &_stringPool.back();
}

std::string* Loader::newString( const char* cstr )
{
	_stringPool.push_back( cstr );
	return &_stringPool.back();
}

void Loader::onComment( const location&, const std::string& text )
{
	assert( hasDoc() );

	if( text.empty() )
		return;

    // ignore a comment if it starts with a dash (useful for separators)
	if( text[0] == '-' )
		return;

	size_t start = 0;

	// is this a post-comment?
    bool isPostComment = false;
	if( !text.empty() && text[0] == '<' )
	{
		start = 1;
		isPostComment = true;
	}

	std::string docText( text, start, text.length() - start );
	if( isPostComment )
		addDocumentation( _lastElement, docText );
	else
    {
		_docBuffer.push_back( '\n' );
		_docBuffer.append( docText );
    }
}

void Loader::onCppBlock( const location&, const std::string& text )
{
	assert( hasCppBlocks() );
	_cppBlock.append( text );
}

void Loader::onImport( const location& loc, const std::string& importTypeName )
{
	// Notice: the type is not loaded now in order to avoid cyclic dependencies.

	if( _typeBuilder.isValid() )
	{
		pushError( loc, "all import clauses must come before the type specification" );
		return;
	}

	size_t lastDotPos = importTypeName.rfind( '.' );
	if( lastDotPos == std::string::npos )
	{
		pushError( loc, "type is in the same namespace and does not require importing" );
		return;
	}

	std::string localTypeName = importTypeName.substr( lastDotPos + 1 );

	// checks whether a type with the same local name was already imported
	ImportTypeMap::iterator it = _importedTypes.find( localTypeName );
	if( it != _importedTypes.end() )
	{
		PUSH_ERROR( loc, "import '" << importTypeName << "' conflicts with a "
			"previous import at line " << it->second.loc.begin.line );
		return;
	}

	_importedTypes.insert( ImportTypeMap::value_type( localTypeName, ImportInfo( importTypeName, loc ) ) );
}

void Loader::onTypeSpec( const location& kLoc, TypeKind kind, const location& nLoc, const std::string& name )
{
	if( _typeBuilder.isValid() )
		pushError( kLoc, "only one type specification is allowed per file" );
	else if( name != _cslFileBaseName )
		pushError( nLoc, "the name of the defined type must match its filename" );
	else
	{
		_typeBuilder = createTypeBuilder( name, kind );
		onElement( std::string() );
		resolveImports();
	}
}

void Loader::onTypeDecl( const location& loc, const std::string& name, bool isArray )
{
	_lastDeclTypeIsArray = isArray;
	_lastDeclTypeName = name;
	_lastDeclTypeLocation = loc;
}

void Loader::onBaseType( const location& loc, const std::string& name )
{
	IType* type = resolveType( loc, name );
	if( getError() )
		return;

	if( !type )
	{
		PUSH_ERROR( loc, "could not load super-type '" << name << "'" );
	}
	else
	{
		CATCH_ERRORS( loc, _typeBuilder->defineBaseType( type ) );
	}
}

void Loader::onEnumIdentifier( const location& loc, const std::string& name )
{
	CATCH_ERRORS( loc, _typeBuilder->defineIdentifier( name ) );
	onElement( name );
}

void Loader::onPort( const location& loc, bool isFacet, const std::string& name )
{
	IType* type = getLastDeclaredType();
	if( getError() )
		return;

	if( type->getKind() != TK_INTERFACE )
	{
		pushError( loc, "an interface type was expected" );
		return;
	}

	CATCH_ERRORS( loc, _typeBuilder->definePort( name, static_cast<IInterface*>( type ), isFacet ) );
	onElement( name );
}

void Loader::onField( const location& loc, const std::string& name, bool isReadOnly )
{
	IType* type = getLastDeclaredType();
	if( getError() )
		return;

	CATCH_ERRORS( loc, _typeBuilder->defineField( name, type, isReadOnly ) );
	onElement( name );
}

void Loader::onMethod( const location& loc, const std::string& name )
{
	CATCH_ERRORS( loc, _methodBuilder = _typeBuilder->defineMethod( name ) );

	IType* returnType = getLastDeclaredType();
	if( getError() )
		return;

	if( returnType != NULL )
		CATCH_ERRORS( loc, _methodBuilder->defineReturnType( returnType ) );

	onElement( name );
}

void Loader::onParameter( const location& loc, bool isIn, bool isOut, const std::string& name )
{
	IType* type = getLastDeclaredType();
	if( getError() )
		return;

	CATCH_ERRORS( loc, _methodBuilder->defineParameter( name, type, isIn, isOut ) );
}

void Loader::onRaises( const location& loc, const std::string& name )
{
	IType* type = resolveType( loc, name );
	if( getError() )
		return;

	if( !type )
	{
		PUSH_ERROR( loc, "error loading exception type '" << name << "'" );
	}
	else if( type->getKind() != TK_EXCEPTION )
	{
		PUSH_ERROR( loc, "attempt to raise non-exception type '" << type->getFullName() << "'" );
	}
	else
	{
		CATCH_ERRORS( loc, _methodBuilder->defineException( static_cast<IException*>( type ) ) );
	}
}

void Loader::onMethodEnd( const location& loc )
{
	CATCH_ERRORS( loc, _methodBuilder->createMethod() );
}

void Loader::onAnnotation( const location& loc, const std::string& name, bool hasData )
{
	std::string componentName;
	componentName.reserve( name.size() + 10 );
	componentName.append( name );
	componentName.append( "Annotation" );

	IType* type = resolveType( loc, componentName );
	if( !type )
	{
		PUSH_ERROR( loc, "error loading annotation type '" << componentName << "'" );
		return;
	}

	if( type->getKind() != TK_COMPONENT )
	{
		PUSH_ERROR( loc, "annotation type '" << componentName << "' is not a component" );
		return;
	}

	try
	{
		RefPtr<IAnnotation> annotation;
		if( hasData )
		{
			RefPtr<IObject> object( type->getReflector()->newInstance() );
			annotation = getAnnotationFrom( object.get() );
		}
		else
		{
			annotation = getDefaultAnnotationInstance( type );
		}

		if( _annotations.empty() )
			_annotations.push_back( AnnotationRecord() );
		_annotations.back().annotations.push_back( annotation.get() );
	}
	catch( Exception& e )
	{
		pushError( loc, e.getMessage() );
	}
}

void Loader::onAnnotationData( const location& loc, const std::string& fieldName, const Any& value )
{
	IAnnotation* annotation = _annotations.back().annotations.back().get();
	try
	{
		IInterface* itf = annotation->getInterface();
		IMember* m = itf->getMember( fieldName );
        if( !m || m->getKind() != MemberKind::Field )
		{
			PUSH_ERROR( loc, "annotation type '" << itf->getFullName() <<
							"' has no field named '" << fieldName << "'" );
			return;
		}

		ICompositeType* ct = m->getOwner();
		IReflector* reflector = ct->getReflector();
		if( !reflector )
		{
			PUSH_ERROR( loc, "annotation type '" << ct->getFullName() << "' has no reflector" );
			return;
		}

		reflector->setField( annotation, static_cast<IField*>( m ), value );
	}
	catch( Exception& e )
	{
		pushError( loc, e.getMessage() );
		PUSH_ERROR( loc, "error setting annotation field '" << fieldName << "'" );
	}
}

IAnnotation* Loader::getAnnotationFrom( IObject* object )
{
	IAnnotation* annotation = object->getService<IAnnotation>();
	if( !annotation )
		CORAL_THROW( Exception, "component '" << object->getComponent()->getName()
						<< "' does not provide an annotation" );
	return annotation;
}

IType* Loader::getType()
{
	assert( _typeBuilder.isValid() );
	IType* type = _typeBuilder->createType();

	if( !_annotations.empty() )
	{
		_annotations.pop_back();

		AnnotationRecList::iterator it = _annotations.begin();
		type->setAnnotations( it->annotations );

		for( ++it; it != _annotations.end(); ++it )
		{
			IMember* m = static_cast<ICompositeType*>( type )->getMember( it->element );
			assert( m );
			m->setAnnotations( it->annotations );
		}
	}

	if( _doc.isValid() )
		type->addAnnotation( _doc.get() );

	if( !_cppBlock.empty() )
	{
		RefPtr<ICppBlock> cppBlock = new CppBlockAnnotation;
		cppBlock->setValue( _cppBlock );
		type->addAnnotation( cppBlock.get() );
	}

	return type;
}

IType* Loader::findImportedType( const std::string& alias )
{
	ImportTypeMap::iterator it = _importedTypes.find( alias );
	if( it != _importedTypes.end() )
		return it->second.type;
	return NULL;
}

void Loader::resolveImports()
{
	assert( _typeBuilder.isValid() );

	for( ImportTypeMap::iterator it = _importedTypes.begin(); it != _importedTypes.end(); ++it )
	{
		ImportInfo& ii = it->second;
		ii.type = resolveType( ii.loc, ii.typeName.c_str() );
		if( getError() )
			return;

		if( ii.type == NULL )
		{
			PUSH_ERROR( ii.loc, "could not import '" << ii.typeName << "'" );
			return;
		}
	}
}

void Loader::onElement( const std::string& element )
{
	if( !_docBuffer.empty() )
	{
		addDocumentation( element, _docBuffer );
		_docBuffer.clear();
	}

	// if we got new annotations...
	if( !_annotations.empty() && !_annotations.back().annotations.empty() )
	{
		// mark the current annotation record and create a new one
		_annotations.back().element = element;
		_annotations.push_back( AnnotationRecord() );
	}

	_lastElement = element;
}

void Loader::addDocumentation( const std::string& element, const std::string& text )
{
	if( !_doc.isValid() )
		_doc = new DocumentationAnnotation;

	if( element.empty() )
		_doc->setValue( text );
	else
		_doc->addDocFor( element, text );
}

IType* Loader::getLastDeclaredType()
{
	if( _lastDeclTypeName == "void" )
		return NULL;

	IType* type = resolveType( _lastDeclTypeLocation, _lastDeclTypeName, _lastDeclTypeIsArray );
	if( !type && !_lastDeclTypeIsArray )
		PUSH_ERROR( _lastDeclTypeLocation, "error loading dependency '" << _lastDeclTypeName << "'" );

	return type;
}

} // namespace csl
} // namespace co
