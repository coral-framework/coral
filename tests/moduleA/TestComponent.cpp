#include "TestComponent_Base.h"
#include <moduleA/TestStruct.h>
#include <moduleA/IDummy.h>
#include <cassert>

namespace moduleA {

class TestComponent : public TestComponent_Base {
 public:
  TestComponent() : _name("NOT SET"), _readOnlyString("READONLY") {
    _testEnum = First;
  }

  virtual ~TestComponent() {
    // empty
  }

  // TestInterface: methods:

  IDummy* getDummyInterfaceField() { return _dummyInterface.get(); }

  void setDummyInterfaceField(IDummy* dummyInterfaceField) {
    _dummyInterface = dummyInterfaceField;
  }

  co::TSlice<IDummy*> getDummyInterfaces() { return _dummyInterfaces; }

  void setDummyInterfaces(co::Slice<IDummy*> dummyInterfaces) {
    _dummyInterfaces.clear();
    for (; dummyInterfaces; dummyInterfaces.popFirst())
      _dummyInterfaces.push_back(dummyInterfaces.getFirst());
  }

  std::string getName() { return _name; }

  void setName(const std::string& name) { _name = name; }

  std::string getNameReadonly() { return _readOnlyString; }

  co::TSlice<std::string> getNames() { return _names; }

  void setNames(co::Slice<std::string> names) {
    _names.clear();
    for (; names; names.popFirst()) _names.push_back(names.getFirst());
  }

  TestInterface* getSelfReferenceField() { return this; }

  TestEnum getTestEnum() { return _testEnum; }

  void setTestEnum(TestEnum value) { _testEnum = value; }

  TestStruct getTestStruct() { return _testStruct; }

  void setTestStruct(const TestStruct& value) { _testStruct = value; }

  co::TSlice<TestStruct> getTestStructArray() { return _testStructArray; }

  void setTestStructArray(co::Slice<TestStruct> range) {
    _testStructArray.clear();
    _testStructArray.reserve(range.getSize());
    for (; range; range.popFirst())
      _testStructArray.push_back(range.getFirst());
  }

  void testInParameters(double d, TestEnum e, const std::string& text,
                        const TestStruct& testStruct, IDummy* dummyInterface,
                        co::Slice<co::int32> intList,
                        co::Slice<IDummy*> interfaces) {
    assert(text != "");
    assert(testStruct.anInt16 > 0);
    assert(!intList.isEmpty());
    assert(intList.getFirst() > 0);
    assert(!interfaces.isEmpty());

    CORAL_UNUSED(d);
    CORAL_UNUSED(e);
    CORAL_UNUSED(text);
    CORAL_UNUSED(testStruct);
    CORAL_UNUSED(dummyInterface);
    CORAL_UNUSED(intList);
    CORAL_UNUSED(interfaces);
  }

  void testOutParameters(double& d, TestEnum& e, std::string& text,
                         TestStruct& testStruct, IDummyRef& dummyInterface,
                         std::vector<co::int32>& intList,
                         std::vector<IDummyRef>& interfaces) {
    d = -1;
    e = First;
    assert(text != "");

    assert(testStruct.anInt16 > 0);
    testStruct.anInt16 = -1;
    text = "";

    dummyInterface = NULL;

    assert(!intList.empty());

    co::int32 myFirstInt = intList[0];
    assert(myFirstInt > 0);
    myFirstInt = -1;

    assert(!interfaces.empty());
    CORAL_UNUSED(interfaces);
  }

  co::AnyValue testAnyReturn(const co::Any& param) { return param; }

  // co::ITypeTransaction methods:

  co::TSlice<co::ITypeBuilder*> getTypeBuilders() {
    return co::TSlice<co::ITypeBuilder*>();
  }

  void commit() {
    // empty
  }

  void rollback() {
    // empty
  }

 protected:
  co::IInterface* getItfService() { return _itf.get(); }

  void setItfService(co::IInterface* itf) { _itf = itf; }

  co::IType* getTypeService() { return _type.get(); }

  void setTypeService(co::IType* type) { _type = type; }

 private:
  co::ITypeRef _type;
  co::IInterfaceRef _itf;

  IDummyRef _dummyInterface;
  std::string _name;
  std::string _readOnlyString;

  std::vector<std::string> _names;
  std::vector<IDummyRef> _dummyInterfaces;
  TestEnum _testEnum;
  TestStruct _testStruct;
  std::vector<TestStruct> _testStructArray;
};

CORAL_EXPORT_COMPONENT(TestComponent, TestComponent);

}  // namespace co
