#include "PseudoService.h"
#include <co/Slice.h>
#include <co/IType.h>
#include <gtest/gtest.h>

/*****************************************************************************/
/* Basic Slice Tests                                                         */
/*****************************************************************************/

TEST( SliceTests, staticIntArray )
{
	int array[] = { 3, 5, 8, 13, 20 };

	int i = 0;
	for( co::Slice<int> r( array ); r; r.popFirst() )
	{
		EXPECT_EQ( array[i++], r.getFirst() );
	}
}

TEST( SliceTests, staticConstIntArray )
{
	const int array[] = { 3, 5, 8, 13, 20 };
	
	co::Slice<int> r( array );
	EXPECT_EQ( r.getSize(), 5 );

	int i = 0;
	for( ; r; r.popFirst() )
	{
		EXPECT_EQ( array[i++], r.getFirst() );
	}
}

TEST( SliceTests, intStdVector )
{
	std::vector<int> v;
	v.push_back( 1 );
	v.push_back( 13 );
	v.push_back( 20 );
	v.push_back( 40 );

	int i = 0;
	for( co::Slice<int> r( v ); r; r.popFirst() )
	{
		EXPECT_EQ( v[i++], r.getFirst() );
	}
}

TEST( SliceTests, constIntStdVector )
{
	std::vector<int> v;
	v.push_back( 1 );
	v.push_back( 13 );
	v.push_back( 20 );
	v.push_back( 40 );
	
	const std::vector<int> cv( v );
	
	int i = 0;
	for( co::Slice<int> r( cv ); r; r.popFirst() )
	{
		EXPECT_EQ( v[i++], r.getFirst() );
	}
}

TEST( SliceTests, stringStdVector )
{
	std::vector<std::string> strList;
	strList.push_back( "Um" );
	strList.push_back( "Dois" );
	strList.push_back( "Tres" );

	co::Slice<std::string> r( strList );

	EXPECT_EQ( r.getLast(), "Tres" );
	r.popLast();

	EXPECT_EQ( r.getLast(), "Dois" );
	r.popLast();

	EXPECT_EQ( r.getLast(), "Um" );
	r.popLast();

	EXPECT_TRUE( r.isEmpty() );
}

/*****************************************************************************/
/* TSlice tests with temporary objects                                       */
/*****************************************************************************/

TEST( TSliceTests, staticArray )
{
	int array[] = { 3, 5, 8, 13, 20 };
	co::TSlice<int> ts( array );
	EXPECT_EQ( ts.getSize(), 5 );
	EXPECT_FALSE( ts.getTemporary() );
}

TEST( TSliceTests, localVector )
{
	std::vector<int> vec( 5, -1 );
	co::TSlice<int> ts( vec );
	EXPECT_EQ( ts.getSize(), 5 );
	EXPECT_FALSE( ts.getTemporary() );
}

TEST( TSliceTests, localRefVector )
{
	std::vector<co::ITypeRef> vec( 5 );
	co::TSlice<co::IType*> ts( vec );
	EXPECT_EQ( ts.getSize(), 5 );
	EXPECT_FALSE( ts.getTemporary() );
}

struct TestTemporary : public co::ITemporary
{
	bool* setWhenDestroyed;

	TestTemporary( bool* setWhenDestroyed )
		: setWhenDestroyed( setWhenDestroyed ) {;}

	virtual ~TestTemporary() { *setWhenDestroyed = true; }
};

TEST( TSliceTests, temporaryObject )
{
	bool wasDestroyed( false );
	std::vector<int> vec( 5, -1 );
	{
		co::TSlice<int> ts( vec, new TestTemporary( &wasDestroyed ) );
		EXPECT_EQ( ts.getSize(), 5 );
		EXPECT_TRUE( ts.getTemporary() );
		EXPECT_FALSE( wasDestroyed );
	}
	EXPECT_TRUE( wasDestroyed );
}

TEST( TSliceTests, temporaryVector )
{
	std::vector<int> vec( 5, -1 );
	EXPECT_EQ( vec.size(), 5 );
	EXPECT_EQ( vec[4], -1 );

	co::TSlice<int> ts( co::moveToSlice<int>( vec ) );
	EXPECT_TRUE( ts.getTemporary() );
	EXPECT_TRUE( vec.empty() );
	EXPECT_EQ( ts.getSize(), 5 );
	EXPECT_EQ( ts[4], -1 );
}

TEST( TSliceTests, temporaryRefVector )
{
	bool wasDestroyed( false );

	PseudoService* service = new PseudoService( &wasDestroyed );
	std::vector<co::RefPtr<PseudoService>> vec( 5, service );
	EXPECT_EQ( 5, service->getRefCount() );
	{
		co::TSlice<PseudoService*> ts( co::moveToSlice<PseudoService*>( vec ) );
		ASSERT_FALSE( wasDestroyed );
		EXPECT_EQ( 5, service->getRefCount() );
	}
	EXPECT_TRUE( wasDestroyed );
}
