#include "CSLTester.h"
#include <co/Exception.h>
#include <gtest/gtest.h>

TEST( ExceptionTests, throwTest )
{
	EXPECT_THROW( throw co::Exception( "message" ), co::Exception );
}

TEST( ExceptionTests, messageTest )
{
	try
	{
		throw co::Exception( "a message" );
	}
	catch( co::Exception& e )
	{
		EXPECT_TRUE( e.getMessage() == "a message" );
		e.setMessage( "another message" );
		EXPECT_TRUE( e.getMessage() == "another message" );
		return;
	}
	EXPECT_FALSE( "Catch failed!" );
}

TEST( ExceptionTests, whatTest )
{
	std::string message = "what message";
	try
	{
		throw co::Exception( message );
	}
	catch( co::Exception& e )
	{
		EXPECT_TRUE( message == e.what() );
		return;
	}
	EXPECT_FALSE( "Catch failed!" );
}

TEST( ExceptionTests, simpleValidException )
{
	CSL_TEST( "ExceptionTests.exception_valid" );
}

TEST( ExceptionTests, invalidExceptions )
{
	CSL_TEST_BEGIN( "ExceptionTests.exception_invalid1" )
	CSL_EXPECT_ERROR( "syntax error near '{'", "exception_invalid1.csl", 2 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "ExceptionTests.exception_invalid2" )
	CSL_EXPECT_ERROR( "syntax error near '3'", "exception_invalid2.csl", 1 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "ExceptionTests.exception_invalid3" )
	CSL_EXPECT_ERROR( "syntax error near '{'", "exception_invalid3.csl", 3 )
	CSL_TEST_END()
}

TEST( ExceptionTests, exceptionInheritance )
{
	CSL_TEST_BEGIN( "ExceptionTests.exception_inheritance" )
	CSL_EXPECT_ERROR( "syntax error near 'extends'", "exception_inheritance.csl", 1 )
	CSL_TEST_END()
}
