#include <gtest/gtest.h>

//---- SHA1 --------------------------------------------------------------------

#include <co/types/SHA1.h>

TEST(SHA1Tests, strings) {
  std::string digest;

  co::SHA1 sha1;
  sha1.add("The quick brown fox jumps over the lazy dog");
  sha1.finish();
  sha1.getDigest(digest);
  EXPECT_EQ("2fd4e1c67a2d28fced849ee1bb76e7391b93eb12", digest);

  sha1.reset();
  sha1.add("The quick brown fox jumps over the lazy cog");
  sha1.finish();
  sha1.getDigest(digest);
  EXPECT_EQ("de9f2c7fd25e1b3afad3e85a0bd17d9b100db4b3", digest);

  sha1.reset();
  sha1.add("");
  sha1.finish();
  sha1.getDigest(digest);
  EXPECT_EQ("da39a3ee5e6b4b0d3255bfef95601890afd80709", digest);
}

//---- SignatureCalculator -----------------------------------------------------

#include "TestHelper.h"
#include <co/IType.h>
#include <co/Coral.h>
#include <co/RefPtr.h>
#include <co/ISystem.h>
#include <co/IException.h>
#include <co/IInterface.h>
#include <co/INamespace.h>
#include <co/ITypeBuilder.h>
#include <co/ITypeManager.h>
#include <co/IMethodBuilder.h>
#include <co/ITypeTransaction.h>
#include <co/IllegalNameException.h>
#include <co/IllegalArgumentException.h>
#include <co/MissingInputException.h>

static co::IInterface* createDummyInterface(const std::string& name) {
  co::RefPtr<co::ITypeBuilder> builder =
      TestHelper::createBuilder(co::TK_INTERFACE, name);
  builder->defineField("dummyAtr", TestHelper::type("string"), false);

  return co::cast<co::IInterface>(builder->createType());
}

TEST(SignatureCalculatorTests, binarySignatureIndependsOnFieldDefineOrder) {
  co::IType* stringType = TestHelper::type("string");

  co::RefPtr<co::ITypeBuilder> builder = TestHelper::createBuilder(
      co::TK_INTERFACE, "createBinarySignatureTests.First");

  builder->defineField("a", stringType, false);
  builder->defineField("b", stringType, true);
  builder->defineField("c", stringType, true);
  builder->defineField("d", stringType, false);

  co::IType* first = builder->createType();

  co::RefPtr<co::ITypeBuilder> builder2 = TestHelper::createBuilder(
      co::TK_INTERFACE, "createBinarySignatureTests.Second");

  builder2->defineField("c", stringType, true);
  builder2->defineField("b", stringType, true);
  builder2->defineField("d", stringType, false);
  builder2->defineField("a", stringType, false);

  co::IType* second = builder2->createType();

  ASSERT_NO_THROW(co::getSystem()->getTypes()->getTransaction()->commit());

  EXPECT_FALSE(first->getBinarySignature().empty());
  EXPECT_TRUE(second->getBinarySignature() == second->getBinarySignature());
}

TEST(SignatureCalculatorTests, arraySignatures) {
  co::IType* stringType = TestHelper::type("string");
  co::IType* int32Type = TestHelper::type("int32");
  co::IType* interfaceTest =
      createDummyInterface("ArraySignaturesTest.IService");
  co::IType* interfaceCopy =
      createDummyInterface("ArraySignaturesTest.InterfaceCopy");

  co::ITypeManager* tm = co::getSystem()->getTypes();
  tm->getArrayOf(stringType);
  tm->getArrayOf(int32Type);
  tm->getArrayOf(interfaceTest);
  tm->getArrayOf(interfaceCopy);

  ASSERT_NO_THROW(tm->getTransaction()->commit());

  co::IType* first = tm->findType("string[]");
  co::IType* second = tm->findType("int32[]");
  co::IType* third = tm->findType("ArraySignaturesTest.IService[]");
  co::IType* fourth = tm->findType("ArraySignaturesTest.InterfaceCopy[]");

  EXPECT_EQ(40, first->getBinarySignature().size());
  EXPECT_EQ(40, second->getBinarySignature().size());
  EXPECT_EQ(40, third->getBinarySignature().size());
  EXPECT_EQ(40, fourth->getBinarySignature().size());

  EXPECT_TRUE(first->getBinarySignature() != second->getBinarySignature());
  EXPECT_TRUE(first->getBinarySignature() != third->getBinarySignature());
  EXPECT_TRUE(second->getBinarySignature() != third->getBinarySignature());

  EXPECT_TRUE(interfaceTest->getBinarySignature() !=
              interfaceCopy->getBinarySignature());
  EXPECT_TRUE(interfaceTest->getFullSignature() !=
              interfaceCopy->getFullSignature());

  EXPECT_TRUE(third->getBinarySignature() != fourth->getBinarySignature());
  EXPECT_TRUE(third->getFullSignature() != fourth->getFullSignature());
}

TEST(SignatureCalculatorTests, enumSignature) {
  co::RefPtr<co::ITypeBuilder> builder =
      TestHelper::createBuilder(co::TK_ENUM, "EnumSignatureTest.First");

  builder->defineIdentifier("Test_1");
  builder->defineIdentifier("Test_2");
  builder->defineIdentifier("Test_3");

  co::IType* first = builder->createType();

  co::RefPtr<co::ITypeBuilder> builder2 =
      TestHelper::createBuilder(co::TK_ENUM, "EnumSignatureTest.FirstCopy");
  builder2->defineIdentifier("Test_1");
  builder2->defineIdentifier("Test_2");
  builder2->defineIdentifier("Test_3");

  co::IType* firstCopy = builder2->createType();

  co::RefPtr<co::ITypeBuilder> builder3 =
      TestHelper::createBuilder(co::TK_ENUM, "EnumSignatureTest.FirstSwitch");
  builder3->defineIdentifier("Test_1");
  builder3->defineIdentifier("Test_3");
  builder3->defineIdentifier("Test_2");
  builder3->createType();

  co::IType* firstSwitchIdentifiers = builder3->createType();

  co::RefPtr<co::ITypeBuilder> builder4 =
      TestHelper::createBuilder(co::TK_ENUM, "EnumSignatureTest.FirstShift");
  builder4->defineIdentifier("Test_1");
  builder4->defineIdentifier("Tes");
  builder4->defineIdentifier("t_2Test_3");
  builder4->createType();

  co::IType* firstIdentifersShift = builder4->createType();

  co::getSystem()->getTypes()->getTransaction()->commit();

  EXPECT_TRUE(first->getBinarySignature() !=
              firstIdentifersShift->getBinarySignature());
  EXPECT_TRUE(first->getFullSignature() !=
              firstIdentifersShift->getFullSignature());
  EXPECT_TRUE(first->getFullSignature() != first->getBinarySignature());
  EXPECT_TRUE(first->getBinarySignature() !=
              firstSwitchIdentifiers->getBinarySignature());
  EXPECT_TRUE(first->getFullSignature() !=
              firstSwitchIdentifiers->getFullSignature());
  EXPECT_TRUE(first->getBinarySignature() == firstCopy->getBinarySignature());
  EXPECT_TRUE(first->getFullSignature() != firstCopy->getFullSignature());
}

TEST(SignatureCalculatorTests, methodSignature) {
  co::RefPtr<co::ITypeBuilder> builder1 =
      TestHelper::createBuilder(co::TK_INTERFACE, "MethodSignatureTest.First");
  co::RefPtr<co::IMethodBuilder> methodBuilder1 = builder1->defineMethod("Ask");
  methodBuilder1->defineParameter("question", TestHelper::type("string"), true,
                                  false);
  methodBuilder1->createMethod();
  co::IInterface* first = co::cast<co::IInterface>(builder1->createType());

  // variations with diferent signature
  co::RefPtr<co::ITypeBuilder> var1builder = TestHelper::createBuilder(
      co::TK_INTERFACE, "MethodSignatureTest.Variation1");
  co::RefPtr<co::IMethodBuilder> var1methodBuilder =
      var1builder->defineMethod("Ask");
  var1methodBuilder->defineParameter("question", TestHelper::type("string"),
                                     true, true);
  var1methodBuilder->createMethod();
  co::IInterface* variation1 =
      co::cast<co::IInterface>(var1builder->createType());

  EXPECT_TRUE(first->getBinarySignature() != variation1->getBinarySignature());
  EXPECT_TRUE(first->getFullSignature() != variation1->getFullSignature());

  co::RefPtr<co::ITypeBuilder> var2builder = TestHelper::createBuilder(
      co::TK_INTERFACE, "MethodSignatureTest.Variation2");
  co::RefPtr<co::IMethodBuilder> var2methodBuilder =
      var2builder->defineMethod("Ask");
  var2methodBuilder->defineParameter("question", TestHelper::type("string"),
                                     false, true);
  var2methodBuilder->createMethod();
  co::IInterface* variation2 =
      co::cast<co::IInterface>(var2builder->createType());

  EXPECT_TRUE(first->getBinarySignature() != variation2->getBinarySignature());
  EXPECT_TRUE(first->getFullSignature() != variation2->getFullSignature());

  co::RefPtr<co::ITypeBuilder> var3builder = TestHelper::createBuilder(
      co::TK_INTERFACE, "MethodSignatureTest.Variation3");
  co::RefPtr<co::IMethodBuilder> var3methodBuilder =
      var3builder->defineMethod("Ask");
  var3methodBuilder->defineParameter("question", TestHelper::type("int32"),
                                     true, false);
  var3methodBuilder->createMethod();
  co::IInterface* variation3 =
      co::cast<co::IInterface>(var3builder->createType());

  EXPECT_TRUE(first->getBinarySignature() != variation3->getBinarySignature());
  EXPECT_TRUE(first->getFullSignature() != variation3->getFullSignature());

  co::RefPtr<co::ITypeBuilder> var4builder = TestHelper::createBuilder(
      co::TK_INTERFACE, "MethodSignatureTest.Variation4");
  co::RefPtr<co::IMethodBuilder> var4methodBuilder =
      var4builder->defineMethod("Ask");
  var4methodBuilder->defineParameter("question", TestHelper::type("string"),
                                     true, false);
  var4methodBuilder->defineReturnType(TestHelper::type("string"));
  var4methodBuilder->createMethod();
  co::IInterface* variation4 =
      co::cast<co::IInterface>(var4builder->createType());

  EXPECT_TRUE(first->getBinarySignature() != variation4->getBinarySignature());
  EXPECT_TRUE(first->getFullSignature() != variation4->getFullSignature());

  co::RefPtr<co::ITypeBuilder> var5builder = TestHelper::createBuilder(
      co::TK_INTERFACE, "MethodSignatureTest.Variation5");
  co::RefPtr<co::IMethodBuilder> var5methodBuilder =
      var5builder->defineMethod("Ask");
  var5methodBuilder->defineParameter("question", TestHelper::type("string"),
                                     true, false);
  var5methodBuilder->defineReturnType(TestHelper::type("int32"));
  var5methodBuilder->createMethod();
  co::IInterface* variation5 =
      co::cast<co::IInterface>(var5builder->createType());

  EXPECT_TRUE(first->getBinarySignature() != variation5->getBinarySignature());
  EXPECT_TRUE(first->getFullSignature() != variation5->getFullSignature());
  EXPECT_TRUE(variation4->getBinarySignature() !=
              variation5->getBinarySignature());

  EXPECT_NO_THROW(co::getSystem()->getTypes()->getTransaction()->commit());
}

TEST(SignatureCalculatorTests, fieldSignature) {
  co::RefPtr<co::ITypeBuilder> builder1 =
      TestHelper::createBuilder(co::TK_INTERFACE, "FieldSignatureTest.First");
  builder1->defineField("name", TestHelper::type("string"), true);
  co::IInterface* first = co::cast<co::IInterface>(builder1->createType());

  // variations with the same binary signature
  co::RefPtr<co::ITypeBuilder> builder2 = TestHelper::createBuilder(
      co::TK_INTERFACE, "FieldSignatureTest.SameBinary1");
  builder2->defineField("name", TestHelper::type("string"), true);
  co::IInterface* sameBinary1 =
      co::cast<co::IInterface>(builder2->createType());

  co::RefPtr<co::ITypeBuilder> builder3 = TestHelper::createBuilder(
      co::TK_INTERFACE, "FieldSignatureTest.SameBinary2");
  builder3->defineField("fullName", TestHelper::type("string"), true);
  co::IInterface* sameBinary2 =
      co::cast<co::IInterface>(builder3->createType());

  // variations with diferent signature
  co::RefPtr<co::ITypeBuilder> builder4 = TestHelper::createBuilder(
      co::TK_INTERFACE, "FieldSignatureTest.Variation1");
  builder4->defineField("fullName", TestHelper::type("string"), false);
  co::IInterface* variation1 = co::cast<co::IInterface>(builder4->createType());

  co::RefPtr<co::ITypeBuilder> builder5 = TestHelper::createBuilder(
      co::TK_INTERFACE, "FieldSignatureTest.Variation2");
  builder5->defineField("name", TestHelper::type("int32"), false);
  co::IInterface* variation2 = co::cast<co::IInterface>(builder5->createType());

  ASSERT_NO_THROW(co::getSystem()->getTypes()->getTransaction()->commit());

  EXPECT_TRUE(first->getBinarySignature() != sameBinary1->getBinarySignature());
  EXPECT_TRUE(first->getFullSignature() != sameBinary1->getFullSignature());
  EXPECT_TRUE(first->getBinarySignature() != sameBinary2->getBinarySignature());
  EXPECT_TRUE(first->getFullSignature() != sameBinary2->getFullSignature());

  EXPECT_TRUE(first->getFullSignature() != variation1->getFullSignature());
  EXPECT_TRUE(first->getBinarySignature() != variation1->getBinarySignature());
  EXPECT_TRUE(first->getBinarySignature() != variation2->getBinarySignature());
  EXPECT_TRUE(first->getFullSignature() != variation2->getFullSignature());
}
