#include <co/Coral.h>
#include <co/IArray.h>
#include <co/Exception.h>
#include <co/TypeTraits.h>
#include <co/IInterface.h>
#include <gtest/gtest.h>

TEST(TypeTraitsTests, kindOf) {
  EXPECT_TRUE(co::kindOf<std::exception>::kind == co::TK_NULL);

  EXPECT_TRUE(co::kindOf<bool>::kind == co::TK_BOOL);

  EXPECT_TRUE(co::kindOf<int>::kind == co::TK_INT32);

  EXPECT_TRUE(co::kindOf<co::int8>::kind == co::TK_INT8);

  EXPECT_TRUE(co::kindOf<double>::kind == co::TK_DOUBLE);

  EXPECT_TRUE(co::kindOf<std::string>::kind == co::TK_STRING);
  EXPECT_TRUE(co::kindOf<const char*>::kind == co::TK_NULL);

  EXPECT_TRUE(co::kindOf<co::TypeKind>::kind == co::TK_ENUM);

  EXPECT_TRUE(co::kindOf<co::Exception>::kind == co::TK_EXCEPTION);

  EXPECT_TRUE(co::kindOf<co::Slice<float>>::kind == co::TK_ARRAY);
  EXPECT_TRUE(co::kindOf<co::Slice<std::string>>::kind == co::TK_ARRAY);
  EXPECT_TRUE(co::kindOf<std::vector<co::IInterfaceRef>>::kind == co::TK_ARRAY);
  //  EXPECT_TRUE(co::kindOf<std::vector<const co::Uuid*>>::kind == co::TK_ARRAY);
}

TEST(TypeTraitsTests, nameOf) {
  EXPECT_STREQ("bool", co::nameOf<bool>::get());

  EXPECT_STREQ("int8", co::nameOf<co::int8>::get());

  EXPECT_STREQ("double", co::nameOf<double>::get());

  EXPECT_STREQ("string", co::nameOf<std::string>::get());

  EXPECT_STREQ("co.TypeKind", co::nameOf<co::TypeKind>::get());

  EXPECT_STREQ("null", co::nameOf<std::exception>::get());

  EXPECT_STREQ("int16[]", co::nameOf<co::Slice<short>>::get());
  EXPECT_STREQ("string[]", co::nameOf<co::Slice<std::string>>::get());
  EXPECT_STREQ("co.IInterface[]",
               co::nameOf<std::vector<co::IInterfaceRef>>::get());
  //EXPECT_STREQ("co.Uuid[]", co::nameOf<std::vector<co::Uuid>>::get());
}

TEST(TypeTraitsTests, typeOf) {
  EXPECT_TRUE(co::typeOf<co::int8>::get()->getKind() == co::TK_INT8);

  EXPECT_TRUE(co::typeOf<double>::get()->getKind() == co::TK_DOUBLE);

  EXPECT_TRUE(co::typeOf<std::string>::get()->getKind() == co::TK_STRING);

  EXPECT_EQ(co::TK_DOUBLE,
            co::typeOf<co::Slice<double>>::get()->getElementType()->getKind());
  EXPECT_EQ(
      co::TK_STRING,
      co::typeOf<co::Slice<std::string>>::get()->getElementType()->getKind());

  co::IArray* at;
  ASSERT_TRUE(NULL != (at = co::typeOf<std::vector<co::IInterfaceRef>>::get()));
  EXPECT_EQ("co.IInterface", at->getElementType()->getFullName());

  //ASSERT_TRUE(NULL != (at = co::typeOf<std::vector<co::Uuid>>::get()));
  //EXPECT_EQ("co.Uuid", at->getElementType()->getFullName());
}
