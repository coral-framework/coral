#include "CSLTester.h"
#include <gtest/gtest.h>

TEST( ArrayTests, invalidArrayTypes )
{
	CSL_TEST_BEGIN( "ArrayTests.invalidExceptionArray" )
	CSL_EXPECT_ERROR( "arrays of exceptions are illegal", "invalidExceptionArray.csl", 6 )
	CSL_TEST_END()

	CSL_TEST_BEGIN( "ArrayTests.invalidComponentArray" )
	CSL_EXPECT_ERROR( "arrays of components are illegal", "invalidComponentArray.csl", 6 )
	CSL_TEST_END()
}
