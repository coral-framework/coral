#include <co/IType.h>
#include <co/Coral.h>
#include <co/ISystem.h>
#include <co/CSLError.h>
#include <co/IArray.h>
#include <co/INamespace.h>
#include <co/ITypeManager.h>
#include <co/TypeLoadException.h>
#include <co/IllegalArgumentException.h>
#include <gtest/gtest.h>

TEST( TypeManagerTests, preDefinedTypes )
{
	co::ITypeManager* tm = co::getSystem()->getTypes();

	// these are global predefined types:
	EXPECT_TRUE( tm->findType( "any" ) != NULL );
	EXPECT_TRUE( tm->findType( "bool" ) != NULL );
	EXPECT_TRUE( tm->findType( "int8" ) != NULL );
	EXPECT_TRUE( tm->findType( "int16" ) != NULL );
	EXPECT_TRUE( tm->findType( "int32" ) != NULL );
	EXPECT_TRUE( tm->findType( "int64" ) != NULL );
	EXPECT_TRUE( tm->findType( "double" ) != NULL );
	EXPECT_TRUE( tm->findType( "string" ) != NULL );

	// these are NOT predefined types:
	EXPECT_TRUE( tm->findType( "void" ) == NULL );
}

TEST( TypeManagerTests, findType )
{
	co::ITypeManager* tm = co::getSystem()->getTypes();

	EXPECT_TRUE( tm->findType( "TypeManagerTests.BarStruct" ) == NULL );
	EXPECT_TRUE( tm->findType( "TypeManagerTests.IFoo" ) == NULL );

	EXPECT_TRUE( tm->getType( "TypeManagerTests.BarStruct" ) != NULL );

	EXPECT_TRUE( tm->findType( "TypeManagerTests.BarStruct" ) != NULL );
	EXPECT_TRUE( tm->findType( "TypeManagerTests.IFoo" ) != NULL );
}

TEST( TypeManagerTests, getType )
{
	co::ITypeManager* tm = co::getSystem()->getTypes();

	EXPECT_TRUE( tm->getType( "co.Uuid" ) != NULL );

	EXPECT_THROW( tm->getType( "" ), co::IllegalArgumentException );
	EXPECT_THROW( tm->getType( "TypeManagerTests.Unexistent" ), co::TypeLoadException );
}

TEST( TypeManagerTests, arrays )
{
	co::ITypeManager* tm = co::getSystem()->getTypes();

	EXPECT_TRUE( tm->findType( "TypeManagerTests.IFoo[]" ) == NULL );
	EXPECT_TRUE( tm->getType( "TypeManagerTests.IFoo[]" ) != NULL );
	EXPECT_TRUE( tm->findType( "TypeManagerTests.IFoo[]" ) != NULL );

	EXPECT_TRUE( tm->findType( "TypeManagerTests.BarStruct[]" ) == NULL );
	EXPECT_EQ( tm->getArrayOf( tm->getType( "TypeManagerTests.BarStruct" ) ),
				static_cast<co::IArray*>( tm->getType( "TypeManagerTests.BarStruct[]" ) ) );
	EXPECT_TRUE( tm->findType( "TypeManagerTests.BarStruct[]" ) != NULL );

	EXPECT_THROW( tm->getArrayOf( NULL ), co::IllegalArgumentException );
	EXPECT_THROW( tm->getType( "TypeManagerTests.BarStruct[][]" ), co::IllegalArgumentException );
	EXPECT_THROW( tm->getType( "co.IllegalArgumentException[]" ), co::IllegalArgumentException );
	EXPECT_THROW( tm->getType( "TypeManagerTests.FooComponent[]" ), co::IllegalArgumentException );
}

TEST( TypeManagerTests, loadType )
{
	co::ITypeManager* tm = co::getSystem()->getTypes();

	std::vector<co::CSLError> errorStack;
	EXPECT_TRUE( tm->loadType( "TypeManagerTests.ErroneousStruct", errorStack ) == NULL );

	EXPECT_EQ( 2, errorStack.size() );

	EXPECT_EQ( 3, errorStack[0].line );
	EXPECT_EQ( "error loading dependency 'ErroneousStruct2'", errorStack[0].message );

	EXPECT_EQ( 3, errorStack[1].line );
	EXPECT_EQ( "syntax error near 'syntax'", errorStack[1].message );
}

struct TypeStatistics
{
	int numNamespaces;
	int numTypes;
	int numCustomTypes;
	int numTypesWithReflector;

	TypeStatistics()
		: numNamespaces( 0 ), numTypes( 0 ), numCustomTypes( 0 ), numTypesWithReflector( 0 )
	{;}

	void traverse( co::INamespace* ns )
	{
		++numNamespaces;

		for( co::TSlice<co::IType*> r( ns->getTypes() ); r; r.popFirst() )
		{
			++numTypes;

			co::IType* type = r.getFirst();
			if( isCustom( type->getKind() ) )
				++numCustomTypes;

			if( type->getReflector() )
			   ++numTypesWithReflector;
		}

		for( co::TSlice<co::INamespace*> r( ns->getChildNamespaces() ); r; r.popFirst() )
			traverse( r.getFirst() );
	}
};

TEST( TypeManagerTests, defaultTypeChecks )
{
	TypeStatistics stats;
	stats.traverse( co::getSystem()->getTypes()->findNamespace( "co" ) );
	EXPECT_EQ( stats.numTypes, stats.numTypesWithReflector );
}
