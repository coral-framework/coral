#include <gtest/gtest.h>

#include <co/Any.h>
#include <co/CSLError.h>

/*****************************************************************************/
/*  Performance / Portability Tests                                          */
/*****************************************************************************/

TEST(AnyValueTests, sizeOf) {
  EXPECT_EQ(sizeof(co::AnyValue), sizeof(co::Any));
}

/*****************************************************************************/
/*	Tests for the Temporary Objects API                                      */
/*****************************************************************************/

template <typename T> void testValue(const T& sample) {
  co::AnyValue a1;
  ASSERT_NE(a1.create<T>(), sample);

  a1.getAny().put(sample);
  ASSERT_EQ(a1.get<T>(), sample);

  co::AnyValue a2(a1);
  EXPECT_EQ(a2.get<T&>(), sample);
  EXPECT_EQ(a1.get<T&>(), a2.get<T&>());

  EXPECT_TRUE(a1.isValid());
  EXPECT_TRUE(a2.isValid());
  a1.clear();

  EXPECT_TRUE(a1.isNull());
  EXPECT_TRUE(a2.isValid());
}

// TEST( AnyValueTests, nativeClassValue )
//{
//	testValue<co::Uuid>( co::Uuid::createRandom() );
//}

TEST(AnyValueTests, structValue) {
  co::CSLError cslError;
  cslError.filename = "filename";
  cslError.message = "msg";
  cslError.line = 3;
  testValue<co::CSLError>(cslError);
}

/*****************************************************************************/
/*	Tests for co::AnyValue::swap()                                           */
/*****************************************************************************/

TEST(AnyValueTests, swapTemporaryObjects) {
  co::IType* type = co::typeOf<co::IType>::get();
  co::AnyValue a(type);

  co::AnyValue b;
  b.create<std::string>() = "hello world";

  EXPECT_EQ(type, a.get<co::IType*>());
  EXPECT_EQ("hello world", b.get<const std::string&>());

  a.swap(b);

  EXPECT_EQ("hello world", a.get<const std::string&>());
  EXPECT_EQ(type, b.get<co::IType*>());
}
