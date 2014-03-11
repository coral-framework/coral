#include "TestHelper.h"

#include <co/Coral.h>
#include <co/RefPtr.h>
#include <co/IMethod.h>
#include <co/ISystem.h>
#include <co/IException.h>
#include <co/INamespace.h>
#include <co/IInterface.h>
#include <co/IParameter.h>
#include <co/ITypeBuilder.h>
#include <co/ITypeManager.h>
#include <co/IMethodBuilder.h>
#include <co/ITypeTransaction.h>
#include <co/IllegalNameException.h>
#include <co/MissingInputException.h>
#include <co/IllegalArgumentException.h>
#include <co/NotSupportedException.h>

#include <gtest/gtest.h>

TEST( MethodBuilderTests, theTest )
{
	co::RefPtr<co::ITypeBuilder> typeBuilder = TestHelper::createBuilder( co::TK_INTERFACE, "MethodBuilderbuilderTest.NewInterface" );

	co::IType* stringType = TestHelper::type( "string" );

	co::RefPtr<co::ITypeBuilder> exbuilder = TestHelper::createBuilder( co::TK_EXCEPTION, "MethodBuilderbuilderTest.NewException" );
	co::IException* testException = co::cast<co::IException>( exbuilder->createType() );

	co::RefPtr<co::IMethodBuilder> mb = typeBuilder->defineMethod( "testMethod" );

	EXPECT_THROW( mb->defineException( NULL ), co::IllegalArgumentException );
	EXPECT_THROW( mb->defineReturnType( NULL ), co::IllegalArgumentException );
	EXPECT_THROW( mb->defineParameter( "ok", NULL, true, true ) , co::IllegalArgumentException );
	EXPECT_THROW( mb->defineParameter( "ok", stringType, false, false ) , co::IllegalArgumentException );

	mb->defineParameter( "p1", stringType, false, true );
	mb->defineParameter( "p2", stringType, true, true );
	mb->defineParameter( "p3", stringType, true, false );

	mb->defineException( testException );

	mb->defineReturnType( stringType );

	EXPECT_THROW( mb->defineParameter( "p1", stringType, true, false ) , co::IllegalNameException ); // clash

	EXPECT_NO_THROW( mb->createMethod() );

	co::IInterface* itf = co::cast<co::IInterface>( typeBuilder->createType() );
	ASSERT_TRUE( itf != NULL );

	co::IMethod* mInfo = co::cast<co::IMethod>( itf->getMember( "testMethod" ) );
	ASSERT_TRUE( mInfo != NULL );

	ASSERT_EQ( 3, mInfo->getParameters().getSize() );

	co::TSlice<co::IParameter*> params = mInfo->getParameters();
	co::IParameter* p = params.getFirst();
	ASSERT_EQ( "p1", p->getName() );
	ASSERT_EQ( stringType, p->getType() );
	ASSERT_FALSE( p->getIsIn() );
	ASSERT_TRUE( p->getIsOut() );

	params.popFirst();
	p = params.getFirst();
	ASSERT_EQ( "p2", p->getName() );
	ASSERT_EQ( stringType, p->getType() );
	ASSERT_TRUE( p->getIsIn() );
	ASSERT_TRUE( p->getIsOut() );

	params.popFirst();
	p = params.getFirst();
	ASSERT_EQ( "p3", p->getName() );
	ASSERT_EQ( stringType, p->getType() );
	ASSERT_TRUE( p->getIsIn() );
	ASSERT_FALSE( p->getIsOut() );

	ASSERT_EQ( stringType, mInfo->getReturnType() );

	ASSERT_EQ( testException , mInfo->getExceptions().getFirst() );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->commit() );

	EXPECT_THROW( mb->createMethod(), co::NotSupportedException );
}
