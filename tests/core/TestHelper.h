#ifndef _TESTHELPER_H_
#define _TESTHELPER_H_

#include <co/Coral.h>

namespace co {
	class IType;
	class ITypeBuilder;
	class ITypeTransaction;
}

class TestHelper
{
public:
	// Creates a typeBuilder for the given fullTypeName, using the given
	// transaction. It creates all namespaces needed to build the type.
	static co::ITypeBuilder* createBuilder( co::TypeKind kind, const std::string& fullTypeName );

	// Gets a system type using fullTypeName
	static co::IType* type( const std::string& fullTypeName );

	// Auxiliary string functions
	inline static bool stringEndsWith( const std::string& str, const std::string& tail )
	{
		return str.rfind( tail ) == ( str.length() - tail.length() );
	}
};

#endif
