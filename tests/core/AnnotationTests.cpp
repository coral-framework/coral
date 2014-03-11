#include "CSLTester.h"
#include <co/IMember.h>
#include <co/ICppBlock.h>
#include <co/IInterface.h>
#include <co/IDocumentation.h>
#include <gtest/gtest.h>

TEST( AnnotationTests, annotationTypeContainsErrors )
{
	CSL_TEST_BEGIN( "AnnotationTests.TypeDoesNotExist" )
	CSL_EXPECT_ERROR( "error loading annotation type 'NonExistingTypeAnnotation'", "TypeDoesNotExist.csl", 1 )
	CSL_EXPECT_ERROR( "type 'NonExistingTypeAnnotation' was not found in the path", "TypeDoesNotExist.csl", 1 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "AnnotationTests.TypeContainsErrors" )
	CSL_EXPECT_ERROR( "error loading annotation type 'AnnotationTests.InvalidAnnotation'",
						"TypeContainsErrors.csl", 1 )
	CSL_EXPECT_ERROR( "error loading dependency 'IContainErrors'", "InvalidAnnotation.csl", 3 )
	CSL_EXPECT_ERROR( "syntax error near 'error'", "IContainErrors.csl", 3 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "AnnotationTests.TrailingComma" )
	CSL_EXPECT_ERROR( "syntax error near ')'", "TrailingComma.csl", 2 )
	CSL_TEST_END()
}

TEST( AnnotationTests, annotationTypeNotAComponent )
{
	CSL_TEST_BEGIN( "AnnotationTests.TypeNotAComponent" )
	CSL_EXPECT_ERROR( "annotation type 'AnnotationTests.IAmNotAComponentAnnotation' is not a component", "TypeNotAComponent.csl", 1 )
	CSL_TEST_END()
}

TEST( AnnotationTests, annotationTypeReflectorError )
{
	CSL_TEST_BEGIN( "AnnotationTests.TypeHasNoReflector" )
	CSL_EXPECT_ERROR( "could not load a reflector for 'AnnotationTests.NoReflectorAnnotation'", "TypeHasNoReflector.csl", 1 )
	CSL_TEST_END()
}

TEST( AnnotationTests, cppBlock )
{
	co::IType* t = CSL_TEST( "AnnotationTests.CppBlock" );
	ASSERT_TRUE( t != NULL );

	co::ICppBlock* cppBlock = t->findAnnotation<co::ICppBlock>();
	co::IDocumentation* doc = t->findAnnotation<co::IDocumentation>();

	ASSERT_TRUE( cppBlock != NULL );
	ASSERT_TRUE( doc == NULL );

	EXPECT_EQ( "At the type", cppBlock->getValue() );

	co::ICompositeType* ct = static_cast<co::ICompositeType*>( t );
	co::IMember* m = ct->getMember( "fieldOne" );
	ASSERT_TRUE( m != NULL );
	cppBlock = m->findAnnotation<co::ICppBlock>();
	ASSERT_TRUE( cppBlock != NULL );
	EXPECT_EQ( "at fieldOne", cppBlock->getValue() );

	m = ct->getMember( "fieldTwo" );
	ASSERT_TRUE( m != NULL );
	cppBlock = m->findAnnotation<co::ICppBlock>();
	ASSERT_TRUE( cppBlock != NULL );
	EXPECT_EQ( "at fieldTwo", cppBlock->getValue() );

	m = ct->getMember( "methodOne" );
	ASSERT_TRUE( m != NULL );
	cppBlock = m->findAnnotation<co::ICppBlock>();
	ASSERT_TRUE( cppBlock != NULL );
	EXPECT_EQ( "at methodOne", cppBlock->getValue() );

	m = ct->getMember( "methodTwo" );
	ASSERT_TRUE( m != NULL );
	cppBlock = m->findAnnotation<co::ICppBlock>();
	ASSERT_TRUE( cppBlock != NULL );
	EXPECT_EQ( "at methodTwo", cppBlock->getValue() );
}

TEST( AnnotationTests, documentation )
{
	co::IType* t = CSL_TEST( "AnnotationTests.Documentation" );
	ASSERT_TRUE( t != NULL );

	co::ICppBlock* cppBlock = t->findAnnotation<co::ICppBlock>();
	co::IDocumentation* doc = t->findAnnotation<co::IDocumentation>();

	ASSERT_TRUE( cppBlock == NULL );
	ASSERT_TRUE( doc != NULL );

	EXPECT_EQ( "First annotation", doc->getValue() );

	co::TSlice<co::IAnnotation*> annotations = t->getAnnotations();
	ASSERT_EQ( 3, annotations.getSize() );
	ASSERT_EQ( "co.IDocumentation", annotations[0]->getInterface()->getFullName() );
	ASSERT_EQ( "co.IDocumentation", annotations[1]->getInterface()->getFullName() );
	ASSERT_EQ( "co.IDocumentation", annotations[2]->getInterface()->getFullName() );

	ASSERT_EQ( "Second annotation", static_cast<co::IDocumentation*>( annotations[1] )->getValue() );
	ASSERT_EQ( "Third annotation", static_cast<co::IDocumentation*>( annotations[2] )->getValue() );
}

TEST( AnnotationTests, instanceSharing )
{
	co::IType* t = CSL_TEST( "AnnotationTests.InstanceSharing" );
	ASSERT_TRUE( t != NULL );

	co::ICompositeType* ct = static_cast<co::ICompositeType*>( t );

	co::ICppBlock* cppBlock = ct->findAnnotation<co::ICppBlock>();
	ASSERT_TRUE( cppBlock != NULL );

	co::IMember* fieldOne = ct->getMember( "one" );
	ASSERT_TRUE( fieldOne != NULL );

	co::IMember* fieldTwo = ct->getMember( "two" );
	ASSERT_TRUE( fieldTwo != NULL );

	co::IMember* fieldThree = ct->getMember( "three" );
	ASSERT_TRUE( fieldThree != NULL );

	EXPECT_EQ( cppBlock, fieldOne->findAnnotation<co::ICppBlock>() );
	EXPECT_EQ( cppBlock, fieldTwo->findAnnotation<co::ICppBlock>() );
	EXPECT_EQ( cppBlock, fieldThree->findAnnotation<co::ICppBlock>() );
}

TEST( AnnotationTests, invalidValueType )
{
	CSL_TEST_BEGIN( "AnnotationTests.InvalidValueType1" )
	CSL_EXPECT_ERROR( "error setting annotation field 'value'", "InvalidValueType1.csl", 1 )
	CSL_EXPECT_ERROR( "illegal cast from 'in double' to 'in string'", "InvalidValueType1.csl", 1 )
	CSL_TEST_END()
}
