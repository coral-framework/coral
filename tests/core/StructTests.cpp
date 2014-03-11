#include "CSLTester.h"
#include <gtest/gtest.h>

TEST( StructTests, simpleValidStruct )
{
	CSL_TEST( "StructTests.struct_valid" );
}

TEST( StructTests, missingDefinition )
{
	CSL_TEST_BEGIN( "StructTests.struct_missingDefinition" )
	CSL_EXPECT_SEMANTIC_ERROR( "missing struct contents" )
	CSL_TEST_END()
}

TEST( StructTests, conflictingType )
{
	// loads the struct_valid type, in order produce a type conflict
	// it checks if struct_valid is already loaded (to keep independence from
	// the first struct test)
	co::IType* structValid = TestHelper::type( "StructTests.struct_valid" );
	if( structValid == NULL )
	{
		CSL_TEST( "StructTests.struct_valid" );
	}

	// altough it seems that a conflicting type name is a semantic error,
	// it is handled as a syntax error because the type definition cannot be
	// completed by the parser: it tries to create the type but it already exists
	CSL_TEST_BEGIN( "StructTests.struct_valid" )
	CSL_EXPECT_ERROR( "already contains a type called 'struct_valid'", "struct_valid.csl", -1 )
	CSL_TEST_END()
}

TEST( StructTests, inheritance )
{
	// structs don't support inheritance syntax
	CSL_TEST_BEGIN( "StructTests.struct_inheritance" )
	CSL_EXPECT_ERROR( "syntax error near 'extends'", "struct_inheritance.csl", 1 )
	CSL_TEST_END()
}

TEST( StructTests, methodDefinition )
{
	// structs don't support method definitions
	CSL_TEST_BEGIN( "StructTests.struct_methods" )
	CSL_EXPECT_ERROR( "syntax error near '('", "struct_methods.csl", 7 )
	CSL_TEST_END()
}

TEST( StructTests, badFieldName )
{
	CSL_TEST_BEGIN( "StructTests.struct_badFieldName" )
	CSL_EXPECT_ERROR( "unknown character '#' (0x23)", "struct_badFieldName.csl", 3 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "StructTests.struct_badFieldName2" )
	CSL_EXPECT_ERROR( "syntax error near 'name'", "struct_badFieldName2.csl", 3 )
	CSL_TEST_END()
}

TEST( StructTests, cyclicDependencies )
{
	// cyclic dependencies are naturally handled by the compiler
	CSL_TEST( "StructTests.struct_valid2" );
}

TEST( StructTests, readonlyField )
{
	CSL_TEST_BEGIN( "StructTests.readonlyField" )
	CSL_EXPECT_ERROR( "structs cannot have read-only fields", "readonlyField.csl", 4 )
	CSL_TEST_END()
}

TEST( StructTests, selfReference )
{
	CSL_TEST_BEGIN( "StructTests.struct_self_reference" )
	CSL_EXPECT_ERROR( "a struct cannot contain itself recursively", "struct_self_reference.csl", 4 )
	CSL_TEST_END()
}

TEST( StructTests, validUsingInvalid )
{
	CSL_TEST_BEGIN( "StructTests.struct_validUsingAnInvalid" )
	CSL_EXPECT_ERROR( "error loading dependency", "struct_validUsingAnInvalid.csl", 4 )
	CSL_EXPECT_ERROR( "unknown character '#' (0x23)", "struct_invalid.csl", 3 )
	CSL_TEST_END()
}

TEST( StructTests, withInvalidMemberType )
{
	CSL_TEST_BEGIN( "StructTests.struct_invalid_with_component" )
	CSL_EXPECT_ERROR( "components are illegal as field types", "struct_invalid_with_component.csl", 4 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "StructTests.struct_invalid_with_exception" )
	CSL_EXPECT_ERROR( "exceptions are illegal as field types", "struct_invalid_with_exception.csl", 4 )
	CSL_TEST_END()
}
