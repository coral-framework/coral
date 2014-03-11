#include "CSLTester.h"
#include <co/IField.h>
#include <co/ISystem.h>
#include <co/ICppBlock.h>
#include <co/IInterface.h>
#include <co/IDocumentation.h>
#include <co/NotSupportedException.h>
#include <gtest/gtest.h>

TEST( TypeLoaderTests, windowsLineBreaks )
{
	CSL_TEST( "TypeLoaderTests.WindowsLineBreaks" );
}

TEST( TypeLoaderTests, namespaceScope )
{
	CSL_TEST( "TypeLoaderTests.Imports.Office" );
}

TEST( TypeLoaderTests, clashingImports )
{
	CSL_TEST_BEGIN( "TypeLoaderTests.Imports.ClashingImports" )
	CSL_EXPECT_ERROR( "conflicts with a previous import", "ClashingImports.csl", 2 )
	CSL_TEST_END()
}

TEST( TypeLoaderTests, validImport )
{
	CSL_TEST( "TypeLoaderTests.Imports.ValidImports" );

	co::IInterface* designer = co::cast<co::IInterface>( TestHelper::type( "TypeLoaderTests.Imports.Roles.Designer") );

	ASSERT_TRUE( designer != NULL );

	co::IField* field = co::cast<co::IField>( designer->getMember( "mainOffice" ) );

	ASSERT_TRUE( field != NULL );

	EXPECT_EQ( "TypeLoaderTests.Imports.Office", field->getType()->getFullName() );
}

TEST( TypeLoaderTests, sameNamespaceImport )
{
	CSL_TEST_BEGIN( "TypeLoaderTests.Imports.SameNamespaceImport" )
	CSL_EXPECT_ERROR( "is in the same namespace and does not require importing", "SameNamespaceImport.csl", 1 )
	CSL_TEST_END()
}

TEST( TypeLoaderTests, cyclicalImport )
{
	CSL_TEST( "TypeLoaderTests.Imports.Chicken" );
}

TEST( TypeLoaderTests, syntaxError )
{
	CSL_TEST_BEGIN( "TypeLoaderTests.SyntaxErrors.struct1" )
	CSL_EXPECT_ERROR( "syntax error near 'structt'", "struct1.csl", 1 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "TypeLoaderTests.SyntaxErrors.struct2" )
	CSL_EXPECT_ERROR( "syntax error near '1'", "struct2.csl", 1 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "TypeLoaderTests.SyntaxErrors.struct3" )
	CSL_EXPECT_ERROR( "syntax error near ';'", "struct3.csl", 4 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "TypeLoaderTests.SyntaxErrors.struct4" )
	CSL_EXPECT_ERROR( "syntax error near 'import'", "struct4.csl", 7 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "TypeLoaderTests.SyntaxErrors.struct5" )
	CSL_EXPECT_ERROR( "syntax error near 'string'", "struct5.csl", 3)
	CSL_TEST_END()

	CSL_TEST_BEGIN( "TypeLoaderTests.SyntaxErrors.struct6" )
	CSL_EXPECT_ERROR( "unknown character '%' (0x25)", "struct6.csl", 3 )
	CSL_TEST_END()
}

std::string getDoc( const char* typeName, const char* memberName )
{
	co::IType* t = co::getType( typeName );
	assert( t );

	co::IDocumentation* doc = t->findAnnotation<co::IDocumentation>();
	if( !doc )
		throw co::NotSupportedException( "no documentation" );

	if( memberName )
		return doc->getDocFor( memberName );

	return doc->getValue();
}

TEST( TypeLoaderTests, enumDocs )
{
	// try getting documentation for a type while documentation is disabled
	EXPECT_THROW( getDoc( "TypeLoaderTests.Documented.SpareEnum", NULL ), co::NotSupportedException );

	// enable documentation
	co::uint8 originalFlags = co::getCSLFlags();
	co::setCSLFlags( co::CSL_DOCUMENTATION );

	CSL_TEST( "TypeLoaderTests.Documented.Enum" );
	
	std::string enumDoc = getDoc( "TypeLoaderTests.Documented.Enum", NULL );
	std::string firstDoc = getDoc( "TypeLoaderTests.Documented.Enum", "first" );
	std::string secondDoc = getDoc( "TypeLoaderTests.Documented.Enum", "second" );
	std::string thirdDoc = getDoc( "TypeLoaderTests.Documented.Enum", "third" );
	
	EXPECT_EQ( "\n This is the Enum Declaration", enumDoc );
	EXPECT_EQ( "\n doc for first.\n second doc for first.", firstDoc );
	EXPECT_EQ( " doc for second.", secondDoc );
	EXPECT_EQ( "", thirdDoc );

	co::setCSLFlags( originalFlags );
}

TEST( TypeLoaderTests, structDocs )
{
	co::uint8 originalFlags = co::getCSLFlags();
	co::setCSLFlags( co::CSL_DOCUMENTATION );

	CSL_TEST( "TypeLoaderTests.Documented.Struct" );

	const std::string& structDoc = getDoc( "TypeLoaderTests.Documented.Struct", NULL );
	const std::string& nameDoc = getDoc( "TypeLoaderTests.Documented.Struct", "name" );
	const std::string& fooDoc = getDoc( "TypeLoaderTests.Documented.Struct", "foo" );

	EXPECT_EQ( "\nStruct Declaration", structDoc );
	EXPECT_EQ( "\n struct member.\n extra doc.", nameDoc );
	EXPECT_EQ( " postDoc.", fooDoc );

	co::setCSLFlags( originalFlags );
}

TEST( TypeLoaderTests, interfaceDocs )
{
	co::uint8 originalFlags = co::getCSLFlags();
	co::setCSLFlags( co::CSL_DOCUMENTATION );
	
	CSL_TEST( "TypeLoaderTests.Documented.Interface" );
	
	const std::string& interfaceDoc = getDoc( "TypeLoaderTests.Documented.Interface", NULL );
	const std::string& nameDoc = getDoc( "TypeLoaderTests.Documented.Interface", "name" );
	const std::string& fooDoc = getDoc( "TypeLoaderTests.Documented.Interface", "foo" );
	
	EXPECT_EQ( "\n This is the interface Declaration\n\n\tThis could be a multi-line comment\n ", interfaceDoc );
	EXPECT_EQ( "\n and this is the field doc.", nameDoc );
	EXPECT_EQ( "\n foo method declaration.\n using separated.\n documentation lines.", fooDoc );

	co::setCSLFlags( originalFlags );
}

TEST( TypeLoaderTests, componentDocs )
{
	co::uint8 originalFlags = co::getCSLFlags();
	co::setCSLFlags( co::CSL_DOCUMENTATION );

	CSL_TEST( "TypeLoaderTests.Documented.Component" );

	const std::string& componentDoc = getDoc( "TypeLoaderTests.Documented.Component", NULL );
	const std::string& providedDoc = getDoc( "TypeLoaderTests.Documented.Component", "providedInterface" );
	const std::string& requiredDoc = getDoc( "TypeLoaderTests.Documented.Component", "requiredInterface" );
	const std::string& notDoc = getDoc( "TypeLoaderTests.Documented.Component", "providedInterfaceNotDocumented" );

	EXPECT_EQ( "\n Component Declaration", componentDoc );
	EXPECT_EQ( "\n provided", providedDoc );
	EXPECT_EQ( " Required interface", requiredDoc );
	EXPECT_EQ( "", notDoc );

	co::setCSLFlags( originalFlags );
}

TEST( TypeLoaderTests, nestedErrors )
{
	CSL_TEST_BEGIN( "TypeLoaderTests.NestedErrors.Struct1" )
	CSL_EXPECT_ERROR( "error loading dependency 'Struct2'", "Struct1.csl", 3 )
	CSL_EXPECT_ERROR( "error loading dependency 'Struct3'", "Struct2.csl", 3 )
	CSL_EXPECT_ERROR( "error loading dependency 'Struct4'", "Struct3.csl", 3 )
	CSL_EXPECT_ERROR( "error loading dependency 'Struct5'", "Struct4.csl", 3 )
	CSL_EXPECT_ERROR( "was not found", "", 3 )
	CSL_TEST_END()
}

TEST( TypeLoaderTests, fileNameDivergence )
{
	CSL_TEST_BEGIN( "TypeLoaderTests.FileNameDivergence.diferentName" )
	CSL_EXPECT_ERROR( "the name of the defined type must match its filename", "diferentName.csl", 2 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "TypeLoaderTests.FileNameDivergence.diferentCaseName" )
	CSL_EXPECT_ERROR( "the name of the defined type must match its filename", "diferentCaseName.csl", 2 )
	CSL_TEST_END()
}

TEST( TypeLoaderTests, multipleSpecifications )
{
	CSL_TEST_BEGIN( "TypeLoaderTests.InvalidDeclarations.multipleTypes" )
	CSL_EXPECT_ERROR( "only one type specification is allowed per file", "multipleTypes.csl", 6 )
	CSL_TEST_END()
}

TEST( TypeLoaderTests, inexistantArrayElementType )
{
	CSL_TEST_BEGIN( "TypeLoaderTests.InvalidDeclarations.inexistantArrayElementType" )
	CSL_EXPECT_ERROR( "error loading array element type", "inexistantArrayElementType.csl", 4 )
	CSL_EXPECT_ERROR( "'IDontExist' was not found", "", 4 )
	CSL_TEST_END()
}

TEST( TypeLoaderTests, arrayTypeAutoCreation )
{
	CSL_TEST( "TypeLoaderTests.ValidDeclarations.arrayTypeAutoCreation" );
	EXPECT_TRUE( TestHelper::type( "TypeLoaderTests.ValidDeclarations.BaseClasses.ArrayElement[]" ) != NULL );
}

TEST( TypeLoaderTests, inexistantType )
{
	CSL_TEST_BEGIN( "TypeLoaderTests.InexistantType" )
	CSL_EXPECT_ERROR( "'TypeLoaderTests.InexistantType' was not found", "", -1 )
	CSL_TEST_END()
}

TEST( TypeLoaderTests, importAfterTypeSpecification )
{
	CSL_TEST_BEGIN( "TypeLoaderTests.InvalidDeclarations.includeAfterTypeSpecifications" )
	CSL_EXPECT_ERROR( "import clauses must come before the type specification", "includeAfterTypeSpecifications.csl", 7 )
	CSL_TEST_END()
}

TEST( TypeLoaderTests, singleCppBlock )
{
	// try getting a C++ block while loading of C++ blocks is disabled
	co::IInterface* it = co::cast<co::IInterface>( co::getType( "TypeLoaderTests.CppBlocks.Spare" ) );
	ASSERT_TRUE( it != NULL );
	ASSERT_TRUE( it->findAnnotation<co::ICppBlock>() == NULL );

	// enable C++ blocks
	co::uint8 originalFlags = co::getCSLFlags();
	co::setCSLFlags( co::CSL_CPPBLOCKS );

	CSL_TEST( "TypeLoaderTests.CppBlocks.Single" );

	it = co::cast<co::IInterface>( co::getType( "TypeLoaderTests.CppBlocks.Single" ) );
	ASSERT_TRUE( it != NULL );

	co::ICppBlock* cppBlock = it->findAnnotation<co::ICppBlock>();
	ASSERT_TRUE( cppBlock != NULL );

	const std::string& str = cppBlock->getValue();
	EXPECT_TRUE( str.find( "\t//This code block was injected by the compiler using the '<c++' tag" ) != std::string::npos );
	EXPECT_TRUE( str.find( "\tvoid myInjectedFoo() {;}" ) != std::string::npos );

	co::setCSLFlags( originalFlags );
}

TEST( TypeLoaderTests, multipleCppBlocks )
{
	co::uint8 originalFlags = co::getCSLFlags();
	co::setCSLFlags( co::CSL_CPPBLOCKS );

	CSL_TEST( "TypeLoaderTests.CppBlocks.Multiple" );

	co::IInterface* it = co::cast<co::IInterface>( co::getType( "TypeLoaderTests.CppBlocks.Multiple" ) );
	ASSERT_TRUE( it != NULL );

	const std::string& str = it->findAnnotation<co::ICppBlock>()->getValue();
	EXPECT_TRUE( str.find( "\tvoid myInjectedFoo() {;}" ) != std::string::npos );
	EXPECT_TRUE( str.find( "\tvoid myLastInjectedFoo() {;}" ) != std::string::npos );

	co::setCSLFlags( originalFlags );
}
