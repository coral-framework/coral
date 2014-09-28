#include <gtest/gtest.h>
#include <co/Coral.h>
#include <co/IPort.h>
#include <co/IField.h>
#include <co/IMethod.h>
#include <co/IObject.h>
#include <co/ISystem.h>
#include <co/IComponent.h>
#include <co/ITypeManager.h>
#include <co/IModuleManager.h>
#include <co/ModuleLoadException.h>
#include <co/MissingInputException.h>
#include <co/IllegalStateException.h>
#include <co/IllegalArgumentException.h>
#include <lua/IState.h>
#include <lua/Exception.h>
#include <lua/IInterceptor.h>
#include <moduleA/IHuman.h>

#define ASSERT_SUCCESS(moduleName)                   \
  try {                                              \
    co::getSystem()->getModules()->load(moduleName); \
  } catch (const std::exception& e) {                \
    FAIL() << e.what();                              \
  }

#define ASSERT_EXCEPTION(code, expectedType, expectedMsg)                      \
  try {                                                                        \
    code;                                                                      \
    FAIL() << "an exception should have been raised";                          \
  } catch (const expectedType& e) {                                            \
    std::string raisedMsg(e.what());                                           \
    if (raisedMsg.find(expectedMsg) == std::string::npos)                      \
      FAIL() << "raised message (\"" << raisedMsg                              \
             << "\") does not contain the expected message (\"" << expectedMsg \
             << "\")";                                                         \
  } catch (std::exception & e) {                                               \
    FAIL() << "wrong exception type (" #expectedType                           \
              " expected): " << e.what();                                      \
  }

#define ASSERT_LUA_EXCEPTION(code, expectedMsg) \
  ASSERT_EXCEPTION(code, lua::Exception, expectedMsg)

#define ASSERT_ERROR(moduleName, expectedMsg)                       \
  ASSERT_EXCEPTION(co::getSystem()->getModules()->load(moduleName), \
                   co::ModuleLoadException, expectedMsg)

// --- Initialization --- //

TEST(LuaTests, setup) {
  try {
    co::getSystem()->setup(std::vector<std::string>{"lua"});
  } catch (std::exception& e) {
    FAIL() << "Could not setup() the system: " << e.what();
  }
}

// --- Sanity Tests --- //

TEST(LuaTests, moduleInvalidScriptName) {
  ASSERT_ERROR(
      "lua.moduleInvalidScriptName",
      "no module loader recognized 'lua.moduleInvalidScriptName' as a module");
}

TEST(LuaTests, moduleInvalidReturn) {
  ASSERT_ERROR("lua.moduleInvalidReturn",
               "the module script must return a table");
}

TEST(LuaTests, moduleValid) { ASSERT_SUCCESS("lua.moduleValid"); }

TEST(LuaTests, assertEq) {
  ASSERT_ERROR("lua.assertEq", "ASSERT_EQ failed: 1 != 2");
}

TEST(LuaTests, assertErrorNoError) {
  ASSERT_ERROR(
      "lua.assertErrorNoError",
      "ASSERT_ERROR failed: no error was raised, though one was expected");
}

TEST(LuaTests, assertErrorMismatch) {
  ASSERT_ERROR("lua.assertErrorMismatch",
               "does not contain the expected message (\"expected message\")");
}

TEST(LuaTests, assertTrue) {
  ASSERT_ERROR("lua.assertTrue", "ASSERT_TRUE failed (nil value)");
}

// --- Log Tests --- //

TEST(LuaTests, log) { ASSERT_SUCCESS("lua.log"); }

// --- Package Tests --- //

TEST(LuaTests, coPackage) { ASSERT_SUCCESS("lua.coPackage"); }

// --- Array Binding Tests --- //

TEST(LuaTests, array) { ASSERT_SUCCESS("lua.array"); }

// --- Component Binding Tests --- //

TEST(LuaTests, componentManipulation) {
  ASSERT_SUCCESS("lua.componentManipulation");
}

// --- Interface Binding Tests --- //

TEST(LuaTests, interfaceFields) { ASSERT_SUCCESS("lua.interfaceFields"); }

TEST(LuaTests, interfaceMethods) { ASSERT_SUCCESS("lua.interfaceMethods"); }

// --- Native Class Binding Tests --- //

TEST(LuaTests, nativeClass) { ASSERT_SUCCESS("lua.nativeClass"); }

// --- Struct Binding Tests --- //

TEST(LuaTests, struct) { ASSERT_SUCCESS("lua.struct"); }

// --- Lua Component Tests --- //

TEST(LuaTests, component) {
  ASSERT_SUCCESS("lua.component");

  // test if __init() is called when instantiating the component from C++
  co::RefPtr<co::IObject> object = co::newInstance("lua.bat.Component");
  moduleA::IHuman* batman = object->getService<moduleA::IHuman>();
  ASSERT_TRUE(batman != NULL);
  EXPECT_EQ("Batman", batman->getName());
}

// --- In/Out Any/Array/Struct Parameter Tests --- //

TEST(LuaTests, inOutParams) { ASSERT_SUCCESS("lua.inOutParams"); }

// --- Services Tests --- //

TEST(LuaTests, services) { ASSERT_SUCCESS("lua.services"); }

// --- lua::IState Tests --- //

TEST(LuaTests, IState) { ASSERT_TRUE(co::getService<lua::IState>() != NULL); }

TEST(LuaTests, findScript) {
  co::RefPtr<lua::IState> luaState = co::getService<lua::IState>();
  std::string filename;

  ASSERT_TRUE(luaState->findScript("lua.test", filename));
  ASSERT_FALSE(luaState->findScript("lua.doesNotExist", filename));
}

TEST(LuaTests, callFunction) {
  co::RefPtr<lua::IState> luaState = co::getService<lua::IState>();

  co::int32 intVec[4] = {-1, -1, -1, -1};

  co::int32 numRes;
  co::Any args[4], results[6];
  co::Slice<co::Any> resRange(results, 6);

  /*
    Test calling a function returned by a module.
    lua.plain.sum returns the summation of its arguments.
   */
  args[0].setIn(1);
  args[1].setIn(2);
  args[2].setIn(3);
  args[3].setIn(5);

  results[0] = intVec[0];
  results[1] = intVec[1];
  results[2] = intVec[2];
  results[3] = intVec[3];

  ASSERT_NO_THROW(numRes =
                      luaState->call("lua.scripts.sum", "",
                                     co::Slice<co::Any>(args, 4), resRange));

  ASSERT_EQ(1, numRes);
  ASSERT_EQ(11, intVec[0]);

  /*
    Test calling functions from a table returned by a module.
   */

  // passthrough() simply returns all of its args.
  ASSERT_NO_THROW(numRes =
                      luaState->call("lua.scripts.manyFunctions", "passthrough",
                                     co::Slice<co::Any>(args, 4), resRange));

  ASSERT_EQ(4, numRes);
  ASSERT_EQ(1, intVec[0]);
  ASSERT_EQ(2, intVec[1]);
  ASSERT_EQ(3, intVec[2]);
  ASSERT_EQ(5, intVec[3]);

  // constants() returns some constants.
  bool b1, b2;
  double d1;
  std::string str1, str2;
  co::AnyValue any1, any2;

  results[0] = any1;
  results[1] = b1;
  results[2] = b2;
  results[3] = d1;
  results[4] = str1;
  results[5] = any2;

  ASSERT_NO_THROW(numRes =
                      luaState->call("lua.scripts.manyFunctions", "constants",
                                     co::Slice<co::Any>(), resRange));

  ASSERT_EQ(6, numRes);
  ASSERT_TRUE(!any1.isValid());
  ASSERT_EQ(true, b1);
  ASSERT_EQ(false, b2);
  ASSERT_EQ(3.14, d1);
  ASSERT_EQ("hello", str1);
  ASSERT_EQ(co::getSystem(), any2.get<co::IService*>());

  // test prunning the results
  results[0] = str1;
  results[1] = str2;

  ASSERT_NO_THROW(numRes = luaState->call("lua.scripts.manyFunctions",
                                          "constants2", co::Slice<co::Any>(),
                                          co::Slice<co::Any>(results, 2)));

  ASSERT_EQ(5, numRes);
  ASSERT_EQ("one", str1);
  ASSERT_EQ("two", str2);
}

TEST(LuaTests, callFunctionErrors) {
  co::RefPtr<lua::IState> luaState = co::getService<lua::IState>();
  co::Slice<co::Any> empty;

  ASSERT_LUA_EXCEPTION(
      luaState->call("lua.scripts.doesNotExist", "nope", empty, empty),
      "module 'lua.scripts.doesNotExist' not found");

  ASSERT_LUA_EXCEPTION(
      luaState->call("lua.scripts.sum", "notATable", empty, empty),
      "module did not return a table");

  ASSERT_LUA_EXCEPTION(
      luaState->call("lua.scripts.manyFunctions", "", empty, empty),
      "attempt to call a table value");

  ASSERT_LUA_EXCEPTION(
      luaState->call("lua.scripts.manyFunctions", "doesNotExist", empty, empty),
      "no such function");

  ASSERT_LUA_EXCEPTION(
      luaState->call("lua.scripts.manyFunctions", "raiseError", empty, empty),
      "here is the error");
}

TEST(LuaTests, exceptionTypes) {
  co::RefPtr<lua::IState> luaState = co::getService<lua::IState>();

  std::string exceptionType;
  std::string message;
  co::Any args[2];
  args[0].set<const std::string&>(exceptionType);
  args[1].set<const std::string&>(message);

  co::Slice<co::Any> empty;
  co::Slice<co::Any> argRange(args, 2);

  exceptionType = "no type given";
  message = "ignored";
  ASSERT_EXCEPTION(luaState->call("lua.scripts.manyFunctions", "raiseError",
                                  argRange, empty),
                   lua::Exception, "no type given");

  exceptionType = "co.IllegalArgumentException";
  message = "and here is the msg";
  ASSERT_EXCEPTION(
      luaState->call("lua.scripts.manyFunctions", "raise", argRange, empty),
      co::IllegalArgumentException, "and here is the msg");

  exceptionType = "co.MissingInputException";
  message = "message here";
  ASSERT_EXCEPTION(
      luaState->call("lua.scripts.manyFunctions", "raise", argRange, empty),
      co::MissingInputException, "message here");

  exceptionType = "invalid type";
  message = "message";
  ASSERT_EXCEPTION(
      luaState->call("lua.scripts.manyFunctions", "raise", argRange, empty),
      lua::Exception,
      "error throwing an exception of type 'invalid type' from Lua");
}

TEST(LuaTests, preserveExceptionType) {
  co::RefPtr<lua::IState> luaState = co::getService<lua::IState>();

  co::Slice<co::Any> empty;

  ASSERT_EXCEPTION(luaState->call("lua.scripts.manyFunctions",
                                  "causeIllegalStateException", empty, empty),
                   co::IllegalStateException, "not 'None'");
}

class PseudoInterceptor : public lua::IInterceptor {
 public:
  PseudoInterceptor() { ; }

  co::IInterface* getInterface() { return 0; }
  co::IObject* getProvider() { return 0; }
  co::IPort* getFacet() { return 0; }
  void serviceRetain() { ; }
  void serviceRelease() { ; }

  struct CallInfo {
    co::IService* service;
    co::IMember* member;
    CallInfo(co::IService* service, co::IMember* member)
        : service(service), member(member) {
      ;
    }
  };

  std::vector<CallInfo> calls;
  std::vector<co::IService*> retained;
  std::vector<co::IService*> released;

  void serviceRetained(co::IService* service) { retained.push_back(service); }

  void serviceReleased(co::IService* service) { released.push_back(service); }

  void postGetField(co::IService* service, co::IField* field, const co::Any&) {
    calls.push_back(CallInfo(service, field));
  }

  void postSetField(co::IService* service, co::IField* field, const co::Any&) {
    calls.push_back(CallInfo(service, field));
  }

  void postInvoke(co::IService* service, co::IMethod* method,
                  co::Slice<co::Any>, const co::Any&) {
    calls.push_back(CallInfo(service, method));
  }

  void postGetService(co::IObject* object, co::IPort* port, co::IService*) {
    calls.push_back(CallInfo(object, port));
  }

  void postSetService(co::IObject* object, co::IPort* port, co::IService*) {
    calls.push_back(CallInfo(object, port));
  }
} sg_interceptor;

TEST(LuaTests, interceptor) {
  // install the interceptor
  lua::IState* luaState = co::getService<lua::IState>();
  luaState->collectGarbage();
  luaState->addInterceptor(&sg_interceptor);

  // test the serviceRetained() notifications
  co::RefPtr<co::IObject> obj = co::newInstance("moduleA.TestAnnotation");
  co::IService* annotation = obj->getService("annotation");

  ASSERT_EQ(0, sg_interceptor.retained.size());
  ASSERT_EQ(0, sg_interceptor.released.size());

  std::string facetName = annotation->getFacet()->getName();
  co::Any args[] = {obj.get(), facetName};
  luaState->call("lua.interceptor", "get", co::Slice<co::Any>(args, 2),
                 co::Slice<co::Any>());

  ASSERT_GE(sg_interceptor.retained.size(), 2);
  EXPECT_EQ(sg_interceptor.retained[0], obj.get());
  EXPECT_EQ(sg_interceptor.retained[1], annotation);

  // test the serviceReleased() notifications
  ASSERT_EQ(sg_interceptor.released.size(), 0);
  luaState->collectGarbage();
  ASSERT_GE(sg_interceptor.released.size(), 2);
  EXPECT_EQ(sg_interceptor.released[0], annotation);
  EXPECT_EQ(sg_interceptor.released[1], obj.get());

  // test the service call notifications
  sg_interceptor.calls.clear();
  luaState->call("lua.interceptor", "serviceCalls", co::Slice<co::Any>(),
                 co::Slice<co::Any>());

  ASSERT_GE(sg_interceptor.calls.size(), 1);
  EXPECT_EQ(sg_interceptor.calls[0].service, co::getSystem());
  EXPECT_EQ(sg_interceptor.calls[0].member,
            co::typeOf<co::ISystem>::get()->getMember("types"));

  ASSERT_GE(sg_interceptor.calls.size(), 2);
  EXPECT_EQ(sg_interceptor.calls[1].service, co::getSystem()->getTypes());
  EXPECT_EQ(sg_interceptor.calls[1].member,
            co::typeOf<co::ITypeManager>::get()->getMember("getType"));

  ASSERT_GE(sg_interceptor.calls.size(), 3);
  EXPECT_EQ(sg_interceptor.calls[2].service, co::getType("int32[]"));
  EXPECT_EQ(sg_interceptor.calls[2].member,
            co::typeOf<co::IType>::get()->getMember("reflector"));

  // test the object call notifications
  sg_interceptor.calls.clear();
  luaState->call("lua.interceptor", "objectCalls", co::Slice<co::Any>(),
                 co::Slice<co::Any>());

  ASSERT_GE(sg_interceptor.calls.size(), 3);
  EXPECT_EQ(sg_interceptor.calls[0].service, sg_interceptor.calls[2].service);

  co::IComponent* ct =
      static_cast<co::IComponent*>(co::getType("moduleA.TestComponent"));
  EXPECT_EQ(sg_interceptor.calls[0].member, ct->getMember("transaction"));
  EXPECT_EQ(sg_interceptor.calls[2].member, ct->getMember("itf"));

  // remove the interceptor
  luaState->removeInterceptor(&sg_interceptor);
  sg_interceptor.calls.clear();
  sg_interceptor.retained.clear();
  sg_interceptor.released.clear();

  // make sure it is not intercepting anything anymore
  luaState->call("lua.interceptor", "moreCalls", co::Slice<co::Any>(),
                 co::Slice<co::Any>());

  EXPECT_EQ(0, sg_interceptor.calls.size());
  EXPECT_EQ(0, sg_interceptor.retained.size());
  EXPECT_EQ(0, sg_interceptor.released.size());
}
