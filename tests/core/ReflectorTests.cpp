#include "TestHelper.h"
#include <co/Coral.h>
#include <co/IPort.h>
#include <co/IField.h>
#include <co/IMethod.h>
#include <co/ISystem.h>
#include <co/IObject.h>
#include <co/CSLError.h>
#include <co/INamespace.h>
#include <co/IReflector.h>
#include <co/ITypeBuilder.h>
#include <co/ITypeManager.h>
#include <co/ICompositeType.h>
#include <co/IllegalCastException.h>
#include <co/MissingInputException.h>
#include <co/ITypeTransaction.h>
#include <co/IllegalArgumentException.h>
#include <co/NotSupportedException.h>
#include <gtest/gtest.h>

TEST(ReflectorTests, basicReflectors) {
  static const char* BASIC_TYPE_NAMES[] = {"any",    "bool",   "int8",
                                           "int16",  "int32",  "int64",
                                           "double", "string", "co.TypeKind"};

  const int NUM_TYPES = CORAL_ARRAY_LENGTH(BASIC_TYPE_NAMES);

  co::IType* types[NUM_TYPES];
  for (int i = 0; i < NUM_TYPES; ++i) {
    types[i] = co::getType(BASIC_TYPE_NAMES[i]);
    ASSERT_TRUE(types[i] != NULL);
  }

  // dummy arguments
  co::Any any;
  co::AnyValue value;
  co::Slice<co::Any> anyRange;

  // create big enough buffers
  const size_t MAX_SIZE = sizeof(co::AnyValue) + sizeof(std::string);
  co::int8 buffer1[MAX_SIZE];
  co::int8 buffer2[MAX_SIZE];

  // make sure all types have a BasicReflector
  for (int i = 0; i < NUM_TYPES; ++i) {
    co::IReflector* reflector = types[i]->getReflector();
    ASSERT_TRUE(reflector != NULL);

    EXPECT_EQ(types[i], reflector->getType());
    EXPECT_GT(reflector->getSize(), co::int32(0));

    ASSERT_NO_THROW(reflector->createValues(buffer1, 1));
    ASSERT_NO_THROW(reflector->createValues(buffer2, 1));

    ASSERT_NO_THROW(reflector->copyValues(buffer2, buffer1, 1));

    ASSERT_NO_THROW(reflector->destroyValues(buffer1, 1));
    ASSERT_NO_THROW(reflector->destroyValues(buffer2, 1));

    EXPECT_THROW(reflector->newInstance(), co::NotSupportedException);
    EXPECT_THROW(reflector->newDynamicProxy(NULL), co::NotSupportedException);
    EXPECT_THROW(reflector->getField(any, NULL, value),
                 co::NotSupportedException);
    EXPECT_THROW(reflector->setField(any, NULL, value),
                 co::NotSupportedException);
    EXPECT_THROW(reflector->invoke(any, NULL, anyRange, value),
                 co::NotSupportedException);
  }
}

TEST(ReflectorTests, arrayReflectors) {
  co::IType* type = co::getType("string[]");
  ASSERT_TRUE(type != NULL);
  ASSERT_EQ(type->getKind(), co::TK_ARRAY);

  co::IReflector* reflector = type->getReflector();
  ASSERT_TRUE(reflector != NULL);

  EXPECT_EQ(type, reflector->getType());
  ASSERT_EQ(reflector->getSize(), sizeof(std::vector<int>));

  typedef std::vector<std::string> StrList;

  StrList listA;
  listA.push_back("one");
  listA.push_back("two");
  listA.push_back("three");
  listA.push_back("end");

  co::int8 buffer[sizeof(std::vector<int>)];
  StrList& listB = *reinterpret_cast<StrList*>(buffer);

  // create an array with 2 strings
  reflector->createValues(buffer, 1);
  ASSERT_EQ(listB.size(), 0);

  // copy array A onto B
  reflector->copyValues(&listA, buffer, 1);
  ASSERT_EQ(listB.size(), 4);
  EXPECT_EQ(listB[0], "one");
  EXPECT_EQ(listB[3], "end");

  // destroy the array
  EXPECT_NO_THROW(reflector->destroyValues(buffer, 1));

  // unsupported methods:
  co::Any any;
  co::AnyValue value;
  co::Slice<co::Any> anyRange;
  EXPECT_THROW(reflector->newInstance(), co::NotSupportedException);
  EXPECT_THROW(reflector->newDynamicProxy(NULL), co::NotSupportedException);
  EXPECT_THROW(reflector->getField(any, NULL, value),
               co::NotSupportedException);
  EXPECT_THROW(reflector->setField(any, NULL, value),
               co::NotSupportedException);
  EXPECT_THROW(reflector->invoke(any, NULL, anyRange, value),
               co::NotSupportedException);
}

TEST(ReflectorTests, reflectorComponent) {
  co::IType* type = co::getType("co.INamespace");
  co::IReflector* reflector = type->getReflector();
  co::IObject* reflectorObject = reflector->getProvider();

  // test the reflector's component type
  co::IComponent* component = reflectorObject->getComponent();
  EXPECT_EQ(1, component->getPorts().getSize());
  EXPECT_EQ("reflector", component->getPorts().getFirst()->getName());

  // test the reflector component's facet
  EXPECT_EQ(reflector,
            reflectorObject->getServiceAt(component->getPorts().getFirst()));
}

TEST(ReflectorTests, exceptions) {
  // test raised exception types
  ASSERT_THROW(co::getType("co.INamespace")->getReflector()->raise("msg"),
               co::NotSupportedException);
  ASSERT_THROW(
      co::getType("co.IllegalCastException")->getReflector()->raise("msg"),
      co::IllegalCastException);
  ASSERT_THROW(
      co::getType("co.MissingInputException")->getReflector()->raise("msg"),
      co::MissingInputException);

  // test raised exception messages
  try {
    co::getType("co.IllegalArgumentException")->getReflector()->raise(
        "my exception message!");
    FAIL() << "an exception should have been raised";
  } catch (co::IllegalArgumentException& e) {
    ASSERT_EQ("my exception message!", e.getMessage());
  } catch (...) {
    FAIL() << "wrong exception type";
  }
}

template <typename T> T* getMember(co::IType* type, const char* memberName) {
  assert(type);
  co::ICompositeType* ct = co::cast<co::ICompositeType>(type);
  T* mi = co::cast<T>(ct->getMember(memberName));
  assert(mi);
  return mi;
}

co::IField* getField(co::IType* type, const char* memberName) {
  return getMember<co::IField>(type, memberName);
}

co::IMethod* getMethod(co::IType* type, const char* memberName) {
  return getMember<co::IMethod>(type, memberName);
}

TEST(ReflectorTests, structSimple) {
  co::IType* type = co::getType("co.CSLError");
  co::IReflector* reflector = type->getReflector();
  ASSERT_TRUE(reflector != NULL);

  // --- sanity checks:
  EXPECT_EQ(type, reflector->getType());
  EXPECT_EQ(sizeof(co::CSLError), reflector->getSize());

  // --- in-place allocation:
  co::int8 memoryArea[sizeof(co::CSLError) * 2];

  co::CSLError& s1 = *reinterpret_cast<co::CSLError*>(memoryArea);
  co::CSLError& s2 =
      *reinterpret_cast<co::CSLError*>(memoryArea + sizeof(co::CSLError));

  reflector->createValues(&s1, 1);
  reflector->createValues(&s2, 1);

  EXPECT_EQ(0, s1.line);
  EXPECT_EQ(0, s1.message.length());
  EXPECT_EQ(0, s1.filename.length());

  EXPECT_EQ(0, s2.line);
  EXPECT_EQ(0, s2.message.length());
  EXPECT_EQ(0, s2.filename.length());

  // --- obtain the necessary fields:
  co::IField* lineField = getField(type, "line");
  ASSERT_TRUE(lineField != NULL);

  co::IField* messageField = getField(type, "message");
  ASSERT_TRUE(messageField != NULL);

  co::IField* filenameField = getField(type, "filename");
  ASSERT_TRUE(filenameField != NULL);

  // --- field setting:
  reflector->setField(s1, lineField, 7);
  EXPECT_EQ(7, s1.line);

  try {
    // try to set a string field with an int value (there's no coercion)
    reflector->setField(s1, messageField, 3);
    EXPECT_FALSE(true);
  } catch (co::IllegalCastException& e) {
    EXPECT_EQ("illegal cast from 'in int32' to 'in string'", e.getMessage());
  }

  co::Any a1;
  std::string str("my message");
  a1.set(str);

  reflector->setField(s1, messageField, a1);
  EXPECT_EQ("my message", s1.message);

  str = "my filename";
  reflector->setField(s1, filenameField, a1);
  EXPECT_EQ("my filename", s1.filename);

  // --- field getting:
  co::AnyValue v1, v2;

  reflector->getField(s1, lineField, v1);
  reflector->getField(s2, lineField, v2);
  EXPECT_EQ(7, v1.get<co::int32>());
  EXPECT_EQ(0, v2.get<co::int32>());

  reflector->getField(s1, messageField, v1);
  reflector->getField(s2, messageField, v2);
  EXPECT_EQ("my message", v1.get<const std::string&>());
  EXPECT_EQ("", v2.get<const std::string&>());

  // --- in-place destruction:
  reflector->destroyValues(&s1, 1);
  reflector->destroyValues(&s2, 1);
}

TEST(ReflectorTests, structExceptions) {
  co::IType* type = co::getType("co.CSLError");
  co::IReflector* reflector = type->getReflector();
  ASSERT_TRUE(reflector != NULL);

  co::int8 memoryArea[sizeof(co::CSLError)];
  co::CSLError& s1 = *reinterpret_cast<co::CSLError*>(memoryArea);

  co::AnyValue v1;
  co::IField* lineField = getField(type, "line");

  EXPECT_THROW(reflector->getField(type, lineField, v1),
               co::IllegalArgumentException);
  EXPECT_THROW(reflector->getField(s1, NULL, v1), co::IllegalArgumentException);
}

TEST(ReflectorTests, interfaceNamespace) {
  co::IType* type = co::getType("co.INamespace");
  co::IReflector* reflector = type->getReflector();
  ASSERT_TRUE(reflector != NULL);

  // --- sanity checks:
  EXPECT_EQ(type, reflector->getType());
  EXPECT_EQ(CORAL_POINTER_SIZE, reflector->getSize());

  // --- obtain the necessary member infos:

  // readonly string name;
  co::IField* nameField = getField(type, "name");
  ASSERT_TRUE(nameField != NULL);

  // readonly IType[] types;
  co::IField* typesField = getField(type, "types");
  ASSERT_TRUE(typesField != NULL);

  // IType getType( in string name );
  co::IMethod* getTypeMethod = getMethod(type, "findType");
  ASSERT_TRUE(getTypeMethod != NULL);

  // ITypeBuilder defineType( in string name, in TypeKind typeKind, in
  // ITypeTransaction transaction )
  co::IMethod* defineTypeMethod = getMethod(type, "defineType");
  ASSERT_TRUE(defineTypeMethod != NULL);

  // --- obtain a 'co.INamespace' instance:
  co::INamespace* coNS = co::getSystem()->getTypes()->findNamespace("co");

  // --- field getting:
  co::AnyValue v1, v2;

  reflector->getField(coNS, nameField, v1);
  EXPECT_EQ("co", v1.get<std::string>());

  // cannot 'set' a read-only field
  EXPECT_THROW(reflector->setField(coNS, nameField, v1),
               co::IllegalArgumentException);

  reflector->getField(coNS, typesField, v1);
  EXPECT_TRUE(v1.get<co::Slice<co::IType*>>().getSize() > 10);
  EXPECT_EQ("co.ArrayType",
            v1.get<co::Slice<co::IType*>>().getFirst()->getFullName());

  // --- calling method getType():
  std::string str("INamespace");
  v1.set(str);

  co::Any a1(v1);

  reflector->invoke(coNS, getTypeMethod, co::Slice<co::Any>(&a1, 1), v2);
  EXPECT_EQ(type, v2.get<co::IType*>());

  // calling getType() with no argument should generate an exception
  EXPECT_THROW(reflector->invoke(coNS, getTypeMethod, co::Slice<co::Any>(), v2),
               co::MissingInputException);

  try {
    // try to call getType() passing a bool instead of a string
    v1.set(false);
    reflector->invoke(coNS, getTypeMethod, co::Slice<co::Any>(&a1, 1), v2);
    EXPECT_FALSE(true);
  } catch (co::IllegalCastException& e) {
    EXPECT_EQ(
        "invalid argument #1 to method findType(): illegal cast from 'in bool' "
        "to 'in string'",
        e.getMessage());
  }

  // --- calling method defineType():
  co::Any args[3];
  str = "MyTypeName";

  // passing only 1 arg, when the method requires 2
  args[0].set(str);
  EXPECT_THROW(reflector->invoke(coNS, defineTypeMethod,
                                 co::Slice<co::Any>(args, 1), v1),
               co::MissingInputException);

  // it should work if we call the method with a 3rd, unecessary argument
  args[1].setIn(co::TK_STRUCT);
  args[2].setIn(1337);
  reflector->invoke(coNS, defineTypeMethod, co::Slice<co::Any>(args, 3), v1);

  // now we should be able to retrieve an ITypeBuilder from res
  co::RefPtr<co::ITypeBuilder> builder = v1.get<co::ITypeBuilder*>();
  EXPECT_TRUE(builder.isValid());

  // cleanup
  co::getSystem()->getTypes()->getTransaction()->rollback();
}

//TEST(ReflectorTests, nativeClass) {
//  co::IType* type = co::getType("co.Uuid");
//  co::IReflector* reflector = type->getReflector();
//  ASSERT_TRUE(reflector != NULL);
//
//  // --- sanity checks:
//  EXPECT_EQ(type, reflector->getType());
//  EXPECT_EQ(sizeof(co::Uuid), reflector->getSize());
//
//  // --- in-place allocation:
//  co::Uuid u1;
//  co::int8 memoryArea[sizeof(co::Uuid)] = {0x01, 0x23, 0x45};
//
//  co::Uuid& u2 = *reinterpret_cast<co::Uuid*>(memoryArea);
//
//  reflector->createValues(&u2, 1);
//
//  EXPECT_EQ(u1, u2);
//
//  // --- obtain the necessary member infos:
//  co::IField* isNullField = getField(type, "isNull");
//  ASSERT_TRUE(isNullField != NULL);
//
//  co::IMethod* createRandomMethod = getMethod(type, "createRandom");
//  ASSERT_TRUE(createRandomMethod != NULL);
//
//  co::IMethod* clearMethod = getMethod(type, "clear");
//  ASSERT_TRUE(clearMethod != NULL);
//
//  co::IMethod* getStringMethod = getMethod(type, "getString");
//  ASSERT_TRUE(getStringMethod != NULL);
//
//  co::IMethod* setStringMethod = getMethod(type, "setString");
//  ASSERT_TRUE(setStringMethod != NULL);
//
//  // --- both freshly constructed Uuids should be null; check this using
//  // 'isNull' and 'getString'
//  co::AnyValue v1, v2;
//
//  reflector->getField(u1, isNullField, v1);
//  EXPECT_TRUE(v1.get<bool>());
//
//  std::string str;
//  co::Any a1(str);
//
//  reflector->invoke(u2, getStringMethod, co::Slice<co::Any>(&a1, 1), v2);
//  EXPECT_EQ("00000000-0000-0000-0000000000000000", str);
//
//  // --- randomize u1, then copy its value to u2 via get/setString
//  reflector->invoke(u1, createRandomMethod, co::Slice<co::Any>(), v1);
//  reflector->getField(u1, isNullField, v1);
//  EXPECT_FALSE(v1.get<bool>());
//
//  EXPECT_NE(u1, u2);
//
//  v1.set(str);
//  reflector->invoke(u1, getStringMethod, co::Slice<co::Any>(&a1, 1), v2);
//  reflector->invoke(u2, setStringMethod, co::Slice<co::Any>(&a1, 1), v2);
//
//  EXPECT_EQ(u1, u2);
//
//  // --- clear u1 and it should be null again
//  reflector->invoke(u1, clearMethod, co::Slice<co::Any>(), v1);
//  reflector->getField(u1, isNullField, v1);
//  EXPECT_TRUE(v1.get<bool>());
//  EXPECT_NE(u1, u2);
//
//  // --- set u2 with an invalid string and it should become null as well
//  str = "{invalid}";
//  v1 = str;
//  reflector->invoke(u2, setStringMethod, co::Slice<co::Any>(&a1, 1), v2);
//  EXPECT_EQ(u1, u2);
//
//  // --- in-place destruction:
//  reflector->destroyValues(&u2, 1);
//}
