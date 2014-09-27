#include "TestHelper.h"

#include <co/Coral.h>
#include <co/RefPtr.h>
#include <co/ISystem.h>
#include <co/INamespace.h>
#include <co/ITypeBuilder.h>
#include <co/ITypeManager.h>
#include <co/ITypeTransaction.h>
#include <co/IllegalNameException.h>
#include <co/MissingInputException.h>
#include <co/IllegalArgumentException.h>
#include <co/NotSupportedException.h>

#include <gtest/gtest.h>

TEST(NamespaceTests, rootNS) {
  co::INamespace* rootNS = co::getSystem()->getTypes()->getRootNS();

  EXPECT_EQ(rootNS->getName(), "");
  EXPECT_EQ(rootNS->getFullName(), "");
  EXPECT_TRUE(rootNS->getParentNamespace() == 0);
}

TEST(NamespaceTests, defineType) {
  co::ITypeManager* tm = co::getSystem()->getTypes();
  co::INamespace* rootNS = tm->getRootNS();

  // make sure these types are not user-definable

  EXPECT_THROW(rootNS->defineType("invalidTK", co::TK_NULL),
               co::IllegalArgumentException);
  EXPECT_THROW(rootNS->defineType("invalidTK", co::TK_ANY),
               co::IllegalArgumentException);
  EXPECT_THROW(rootNS->defineType("invalidTK", co::TK_BOOL),
               co::IllegalArgumentException);
  EXPECT_THROW(rootNS->defineType("invalidTK", co::TK_INT8),
               co::IllegalArgumentException);
  EXPECT_THROW(rootNS->defineType("invalidTK", co::TK_INT16),
               co::IllegalArgumentException);
  EXPECT_THROW(rootNS->defineType("invalidTK", co::TK_INT32),
               co::IllegalArgumentException);
  EXPECT_THROW(rootNS->defineType("invalidTK", co::TK_INT64),
               co::IllegalArgumentException);
  EXPECT_THROW(rootNS->defineType("invalidTK", co::TK_DOUBLE),
               co::IllegalArgumentException);
  EXPECT_THROW(rootNS->defineType("invalidTK", co::TK_STRING),
               co::IllegalArgumentException);
  EXPECT_THROW(rootNS->defineType("invalidTK", co::TK_ARRAY),
               co::IllegalArgumentException);

  // test collision with namespaces
  rootNS->defineChildNamespace("DummyNamespace");
  EXPECT_THROW(rootNS->defineType("DummyNamespace", co::TK_ENUM),
               co::IllegalNameException);

  // test collision with another type
  co::RefPtr<co::ITypeBuilder> tb =
      rootNS->defineType("DummyException", co::TK_EXCEPTION);
  ASSERT_TRUE(TestHelper::type("DummyException") != NULL);

  ASSERT_TRUE(rootNS->findType("nonexistent") == NULL);
  ASSERT_TRUE(rootNS->findType("DummyException") != NULL);
  EXPECT_THROW(rootNS->defineType("DummyException", co::TK_ENUM),
               co::IllegalNameException);

  // test definability of remaining types
  EXPECT_NO_THROW(tb = rootNS->defineType("missingInput1", co::TK_ENUM));
  EXPECT_THROW(tm->getTransaction()->commit(), co::MissingInputException);

  EXPECT_NO_THROW(tb = rootNS->defineType("missingInput2", co::TK_STRUCT));
  EXPECT_THROW(tm->getTransaction()->commit(), co::NotSupportedException);

  EXPECT_NO_THROW(tb = rootNS->defineType("missingInput3", co::TK_NATIVECLASS));
  EXPECT_NO_THROW(tb = rootNS->defineType("missingInput4", co::TK_INTERFACE));
  EXPECT_NO_THROW(tb = rootNS->defineType("missingInput5", co::TK_COMPONENT));

  EXPECT_NO_THROW(tm->getTransaction()->rollback());
}

TEST(NamespaceTests, defineChildNamespace) {
  co::INamespace* rootNS = co::getSystem()->getTypes()->getRootNS();

  co::INamespace* levelOneNS = rootNS->defineChildNamespace("levelOne");

  EXPECT_EQ(levelOneNS->getName(), "levelOne");
  EXPECT_EQ(levelOneNS->getFullName(), "levelOne");
  EXPECT_EQ(levelOneNS->getParentNamespace(), rootNS);
  EXPECT_TRUE(levelOneNS->getTypes().isEmpty());
  EXPECT_TRUE(levelOneNS->getChildNamespaces().isEmpty());

  co::INamespace* levelTwoNS = levelOneNS->defineChildNamespace("levelTwo");

  EXPECT_EQ(levelTwoNS->getName(), "levelTwo");
  EXPECT_EQ(levelTwoNS->getFullName(), "levelOne.levelTwo");
  EXPECT_EQ(levelTwoNS->getParentNamespace(), levelOneNS);
  EXPECT_TRUE(levelTwoNS->getTypes().isEmpty());
  EXPECT_TRUE(levelTwoNS->getChildNamespaces().isEmpty());

  EXPECT_EQ(levelOneNS->getTypes().getSize(), 0);
  EXPECT_EQ(levelOneNS->getChildNamespaces().getSize(), 1);
  EXPECT_EQ(levelOneNS->findType("levelTwo"),
            reinterpret_cast<co::IType*>(NULL));
  EXPECT_EQ(levelOneNS->findChildNamespace("levelTwo"), levelTwoNS);

  EXPECT_EQ(co::getSystem()->getTypes()->findNamespace("levelOne.levelTwo"),
            levelTwoNS);

  EXPECT_NO_THROW(rootNS->findChildNamespace("lalala"));

  EXPECT_THROW(rootNS->defineChildNamespace("levelOne"),
               co::IllegalNameException);
}
