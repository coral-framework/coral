#include "CSLTester.h"
#include <gtest/gtest.h>

TEST( InterfaceTests, fieldAndMethodClashes )
{
	CSL_TEST_BEGIN( "InterfaceTests.FieldAndMethodClashes.methodAndFieldMemberName" )
	CSL_EXPECT_SEMANTIC_ERROR( "name clash" )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "InterfaceTests.FieldAndMethodClashes.fieldGetterAndMethod" )
	CSL_EXPECT_SEMANTIC_ERROR( "name clash" )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "InterfaceTests.FieldAndMethodClashes.fieldSetterAndMethod" )
	CSL_EXPECT_SEMANTIC_ERROR( "name clash" )
	CSL_TEST_END()

	CSL_TEST( "InterfaceTests.FieldAndMethodClashes.fieldSetterAndMethod_valid" );

	CSL_TEST_BEGIN( "InterfaceTests.FieldAndMethodClashes.methodAndFieldGetter" )
	CSL_EXPECT_SEMANTIC_ERROR( "name clash" )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "InterfaceTests.FieldAndMethodClashes.methodAndFieldSetter" )
	CSL_EXPECT_SEMANTIC_ERROR( "name clash" )
	CSL_TEST_END()

	CSL_TEST( "InterfaceTests.FieldAndMethodClashes.methodAndFieldSetter_valid" );
}

TEST( InterfaceTests, clashesWithSuperType )
{
	CSL_TEST_BEGIN( "InterfaceTests.ClashesWithSuperType.Interface1" )
	CSL_EXPECT_SEMANTIC_ERROR( "name clash" )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "InterfaceTests.ClashesWithSuperType.Interface2" )
	CSL_EXPECT_SEMANTIC_ERROR( "name clash" )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "InterfaceTests.ClashesWithSuperType.Interface3" )
	CSL_EXPECT_SEMANTIC_ERROR( "name clash" )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "InterfaceTests.ClashesWithSuperType.Interface4" )
	CSL_EXPECT_SEMANTIC_ERROR( "name clash" )
	CSL_TEST_END()

	// this test catches the error in a second iteration of the semantic checker
	CSL_TEST_BEGIN( "InterfaceTests.StartFromParent.ClashingParent" )
	CSL_EXPECT_SEMANTIC_ERROR( "name clash" )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "InterfaceTests.DeepInheritanceTree.UserClash" )
	CSL_EXPECT_SEMANTIC_ERROR( "name clash" )
	CSL_TEST_END()
}

TEST( InterfaceTests, firstOderInheritance )
{
	CSL_TEST( "InterfaceTests.FirstOrder.Child" );

	CSL_TEST( "InterfaceTests.FirstOrder.InheritanceOnly" );

	CSL_TEST( "InterfaceTests.StartFromParent.Parent" );
}

TEST( InterfaceTests, deepInheritanceTree )
{
	CSL_TEST( "InterfaceTests.DeepInheritanceTree.User" );
}

TEST( InterfaceTests, invalidDeclarations )
{
	CSL_TEST_BEGIN( "InterfaceTests.InvalidDeclarations.invalidFieldName" )
	CSL_EXPECT_ERROR( "must start with a lowercase letter", "invalidFieldName.csl", 7 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "InterfaceTests.InvalidDeclarations.missingParameterIO" )
	CSL_EXPECT_ERROR( "expected 'in', 'out' or 'inout' before 'string'", "missingParameterIO.csl", 5 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "InterfaceTests.InvalidDeclarations.parameterTypeNotFound" )
	CSL_EXPECT_ERROR( "error loading dependency 'IDontExixst'", "parameterTypeNotFound.csl", 6  )
	CSL_EXPECT_ERROR( "type 'IDontExixst' was not found in the path", "parameterTypeNotFound.csl", 6  )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "InterfaceTests.InvalidDeclarations.parameterNameClash" )
	CSL_EXPECT_ERROR( "parameter 'fooMe' defined twice", "parameterNameClash.csl", 6  )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "InterfaceTests.InvalidDeclarations.missingReturnType" )
	CSL_EXPECT_ERROR( "syntax error near '('", "missingReturnType.csl", 4 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "InterfaceTests.InvalidDeclarations.inexistentReturnType" )
	CSL_EXPECT_ERROR( "error loading dependency 'IDontExist'", "inexistentReturnType.csl", 4 )
	CSL_EXPECT_ERROR( "type 'IDontExist' was not found in the pat", "inexistentReturnType.csl", 4 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "InterfaceTests.InvalidDeclarations.inexistentRaisedException" )
	CSL_EXPECT_ERROR( "type 'IDontExist' was not found in the path", "inexistentRaisedException.csl", 5 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "InterfaceTests.InvalidDeclarations.missingReturnTypeAndMethodNameClash" )
	CSL_EXPECT_ERROR( "syntax error near '('", "missingReturnTypeAndMethodNameClash.csl", 4 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "InterfaceTests.InvalidDeclarations.RaisesInvalidType" )
	CSL_EXPECT_ERROR( "attempt to raise non-exception type", "RaisesInvalidType.csl", 5  )
	CSL_TEST_END()
}

TEST( InterfaceTests, validDeclarations )
{
	CSL_TEST( "InterfaceTests.ValidDeclarations.validInterface" );

	CSL_TEST( "InterfaceTests.ValidDeclarations.emptyDeclaration" );

	CSL_TEST( "InterfaceTests.ValidDeclarations.emptyDeclarationWithComment" );
}

TEST( InterfaceTests, cyclicInheritance )
{
	CSL_TEST_BEGIN( "InterfaceTests.CyclicInheritance.A" )
	CSL_EXPECT_SEMANTIC_ERROR( "cyclic inheritance detected" )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "InterfaceTests.CyclicInheritance.C" )
	CSL_EXPECT_SEMANTIC_ERROR( "cyclic inheritance detected" )
	CSL_TEST_END()
}

TEST( InterfaceTests, invalidParameter )
{
	CSL_TEST_BEGIN( "InterfaceTests.InvalidDeclarations.invalidInterfaceWithComponentAsParameter" )
	CSL_EXPECT_ERROR( "illegal parameter 'comp' of non-data type 'component'", "invalidInterfaceWithComponentAsParameter.csl", 6 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "InterfaceTests.InvalidDeclarations.invalidInterfaceWithExceptionAsParameter" )
	CSL_EXPECT_ERROR( "illegal parameter 'ex' of non-data type 'exception'", "invalidInterfaceWithExceptionAsParameter.csl", 6 )
	CSL_TEST_END()
}
