#include "CSLTester.h"

#include <gtest/gtest.h>

TEST( NativeClassTests, validNativeClasses )
{
	CSL_TEST( "NativeClassTests.TestNativeClass1" );
	CSL_TEST( "NativeClassTests.TestNativeClass2" );
	CSL_TEST( "NativeClassTests.TestNativeClass3" );
}

TEST( NativeClassTests, emptyNativeClass )
{
	CSL_TEST( "NativeClassTests.EmptyNativeClass" );
}

TEST( NativeClassTests, invalidNativeClasses )
{
	CSL_TEST_BEGIN( "NativeClassTests.TestNativeClass_invalid" )
	CSL_EXPECT_ERROR( "syntax error near 'native'", "TestNativeClass_invalid.csl", 1 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "NativeClassTests.TestNativeClass_invalid2" )
	CSL_EXPECT_ERROR( "syntax error near 'nativeclass'", "TestNativeClass_invalid2.csl", 1 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "NativeClassTests.TestNativeClass_invalid3" )
	CSL_EXPECT_ERROR( "syntax error near 'native'", "TestNativeClass_invalid3.csl", 1 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "NativeClassTests.TestNativeClass_invalid4" )
	CSL_EXPECT_ERROR( "syntax error near '('", "TestNativeClass_invalid4.csl", 1 )
	CSL_TEST_END()
}
