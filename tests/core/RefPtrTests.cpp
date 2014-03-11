#include "PseudoService.h"
#include <gtest/gtest.h>

TEST( RefPtrTests, size )
{
	EXPECT_EQ( sizeof(void*), sizeof(co::RefPtr<PseudoService>) );
}

TEST( RefPtrTests, validity )
{
	co::RefPtr<PseudoService> ptr;
	EXPECT_TRUE( !ptr );
	EXPECT_FALSE( ptr.isValid() );

	ptr = new PseudoService;
	EXPECT_FALSE( !ptr );
	EXPECT_TRUE( ptr.isValid() );

	ptr = NULL;
	EXPECT_TRUE( !ptr );
	EXPECT_FALSE( ptr.isValid() );
}

TEST( RefPtrTests, release )
{
	bool objDestroyed = false;
	PseudoService *obj = new PseudoService( &objDestroyed );
	{
		obj->serviceRetain();
		co::RefPtr<PseudoService> ptr = obj;
	}

	ASSERT_FALSE( objDestroyed );
	obj->serviceRelease();
	EXPECT_TRUE( objDestroyed );

	bool anonymousInstanceDestroyed = false;
	{
		co::RefPtr<PseudoService>( new PseudoService( &anonymousInstanceDestroyed ) );
	}
	EXPECT_TRUE( anonymousInstanceDestroyed );

}

TEST( RefPtrTests, swap )
{
	bool objDestroyed = false;
	PseudoService *obj = new PseudoService( &objDestroyed );

	co::RefPtr<PseudoService> ptr1;
	{
		using std::swap;
		co::RefPtr<PseudoService> ptr2( obj );
		ptr2.swap( ptr1 );
		swap( ptr1, ptr2 );
		ptr2.swap( ptr1 );
	}

	ASSERT_FALSE( objDestroyed );
	ptr1 = 0;
	EXPECT_TRUE( objDestroyed );
}

struct TripleRefs
{
	co::RefPtr<PseudoService> ref1;
	co::RefPtr<PseudoService> ref2;
	co::RefPtr<PseudoService> ref3;
};

TEST( RefPtrTests, structFields )
{
	bool destroyedA = false;
	PseudoService *objA = new PseudoService( &destroyedA );

	bool destroyedB = false;
	PseudoService *objB = new PseudoService( &destroyedB );

	TripleRefs structOne;

	EXPECT_EQ( objA->getRefCount(), 0 );
	EXPECT_EQ( objB->getRefCount(), 0 );

	structOne.ref1 = objA;
	structOne.ref2 = objA;
	structOne.ref3 = objB;

	EXPECT_EQ( objA->getRefCount(), 2 );
	EXPECT_EQ( objB->getRefCount(), 1 );

	TripleRefs structTwo;
	structTwo = structOne;

	structTwo.ref2 = objB;

	EXPECT_EQ( objA->getRefCount(), 3 );
	EXPECT_EQ( objB->getRefCount(), 3 );

	TripleRefs structThree( structOne );
	structOne = structTwo;

	EXPECT_EQ( objA->getRefCount(), 4 );
	EXPECT_EQ( objB->getRefCount(), 5 );

	TripleRefs emptyStruct;
	structThree = emptyStruct;

	EXPECT_EQ( objA->getRefCount(), 2 );
	EXPECT_EQ( objB->getRefCount(), 4 );
}

TEST( RefPtrTests, sliceUpcast )
{
	PseudoService o1, o2, o3;
	o1.serviceRetain();
	o2.serviceRetain();
	o3.serviceRetain();

	std::vector<co::RefPtr<PseudoService> > refVec;
	refVec.push_back( &o1 );
	refVec.push_back( &o2 );
	refVec.push_back( &o3 );

	co::Slice<co::IService*> range( refVec );
	EXPECT_EQ( range.getSize(), 3 );
}
