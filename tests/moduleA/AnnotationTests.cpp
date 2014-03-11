#include <co/Coral.h>
#include <co/Exception.h>
#include <moduleA/ITestAnnotation.h>
#include <gtest/gtest.h>

TEST( AnnotationTests, tonsOfDataTests )
{
	co::IType* t = co::getType( "moduleA.tests.AnnotationData" );

	moduleA::ITestAnnotation* ta = t->findAnnotation<moduleA::ITestAnnotation>();
	ASSERT_TRUE( ta != NULL );

	EXPECT_EQ( 5.5, ta->getValue() );
	EXPECT_TRUE( ta->getB() );
	EXPECT_EQ( 0, ta->getI8() );
	EXPECT_EQ( co::uint8( 300 ), ta->getU8() );
	EXPECT_EQ( -0.00075f, ta->getFlt() );
	EXPECT_EQ( std::string( "Hello\nWorld\0This is awesome! More.", 34 ), ta->getStr() );
	EXPECT_EQ( 9, ta->getI32() );
}

TEST( AnnotationTests, deepNestedErrors )
{
	try
	{
		co::getType( "moduleA.tests.DeepErrors" );
		FAIL() << "exception now thrown";
	}
	catch( co::Exception& e )
	{
		const std::string& msg = e.getMessage();
		std::string msgStart( "could not load type 'moduleA.tests.DeepErrors':" );
		std::string msgEnd( "FaultyType.csl:1:18: annotation type 'co.ICppBlock' has no field named 'raise'" );
		EXPECT_EQ( 0, msg.find( msgStart ) );
		EXPECT_EQ( msg.size() - msgEnd.size(), msg.rfind( msgEnd ) );
	}
}
