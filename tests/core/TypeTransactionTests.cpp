#include "TestHelper.h"

#include <co/RefPtr.h>
#include <co/IStruct.h>
#include <co/ISystem.h>
#include <co/INamespace.h>
#include <co/ITypeBuilder.h>
#include <co/ITypeManager.h>
#include <co/ITypeTransaction.h>
#include <co/MissingInputException.h>
#include <co/NotSupportedException.h>

#include <gtest/gtest.h>

TEST( TypeTransactionTests, throwOnCommitAfterSuccessfullCommit )
{
	co::RefPtr<co::ITypeBuilder> builder = TestHelper::createBuilder( co::TK_ENUM, "TypeTransactionTests.throwOnCommitAfterSuccessfullCommit.IEnum" );

	builder->defineIdentifier( "foo" );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->commit() );

	// empty commits / rollbacks
	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->commit() );
	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->rollback() );
}

TEST( TypeTransactionTests, throwOnCommitAfterUnsuccessfullCommit )
{
	co::RefPtr<co::ITypeBuilder> builder = TestHelper::createBuilder( co::TK_ENUM, "TypeTransactionTests.throwOnCommitAfterUnsuccessfullCommit.IEnum" );

	// invalid commit since the enum has no identifier
	EXPECT_THROW( co::getSystem()->getTypes()->getTransaction()->commit(), co::MissingInputException );

	// try to fix it
	builder->defineIdentifier( "foo" );

	// cannot commit again even if the last commit was unsuccessful and the builder was corrected
	EXPECT_THROW( co::getSystem()->getTypes()->getTransaction()->commit(), co::NotSupportedException );
	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->rollback() );
}

TEST( TypeTransactionTests, rollbackWithoutCommit )
{
	co::RefPtr<co::ITypeBuilder> enumBuilder = TestHelper::createBuilder( co::TK_ENUM, "TypeTransactionTests.transactionRollbackTest.IEnum" );
	co::RefPtr<co::ITypeBuilder> structBuilder = TestHelper::createBuilder( co::TK_STRUCT, "TypeTransactionTests.transactionRollbackTest.IStruct" );

	ASSERT_NO_THROW( enumBuilder->defineIdentifier( "foo" ) );
	ASSERT_NO_THROW( structBuilder->defineField( "name", enumBuilder->createType(), false ) );

	// force the creation of an array for the struct
	ASSERT_NO_THROW( co::getSystem()->getTypes()->getArrayOf( structBuilder->createType() ) );

	ASSERT_TRUE( TestHelper::type( "TypeTransactionTests.transactionRollbackTest.IEnum" ) != NULL );
	ASSERT_TRUE( TestHelper::type( "TypeTransactionTests.transactionRollbackTest.IStruct" ) != NULL );
	ASSERT_TRUE( TestHelper::type( "TypeTransactionTests.transactionRollbackTest.IStruct[]" ) != NULL );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->rollback() );

	EXPECT_TRUE( TestHelper::type( "TypeTransactionTests.transactionRollbackTest.IEnum" ) == NULL );
	EXPECT_TRUE( TestHelper::type( "TypeTransactionTests.transactionRollbackTest.IStruct" ) == NULL );
	EXPECT_TRUE( TestHelper::type( "TypeTransactionTests.transactionRollbackTest.IStruct[]" ) == NULL );
}
