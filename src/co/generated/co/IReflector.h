
#ifndef _CO_IREFLECTOR_H_
#define _CO_IREFLECTOR_H_

#include <co/Common.h>
#include <co/Any.h>
#include <co/IService.h>

// Forward Declarations:
namespace co {
	class IDynamicServiceProvider; typedef co::RefPtr<IDynamicServiceProvider> IDynamicServiceProviderRef;
	class IField; typedef co::RefPtr<IField> IFieldRef;
	class IMethod; typedef co::RefPtr<IMethod> IMethodRef;
	class IObject; typedef co::RefPtr<IObject> IObjectRef;
	class IType; typedef co::RefPtr<IType> ITypeRef;
} // namespace co
// End Of Forward Declarations

// co.IReflector Mapping:
namespace co {

class IReflector : public co::IService
{
public:
	virtual ~IReflector() {;}

	// Code From <c++ Block:
	
	/*
		Constructs an array of values in memory starting at `ptr`.
		The memory area should be at least `numValues * size` bytes long.
		Raises co.NotSupportedException if #type cannot be cast to a value.
	 */
	virtual void createValues( void* ptr, size_t numValues ) = 0;

	/*
		Copies the array of values at `fromPtr` to `toPtr`.
		Both arrays must contain `numValues` properly constructed values.
		Each copy is done using the value's assignment operator.
		Raises co.NotSupportedException if #type cannot be cast to a value.
	 */
	virtual void copyValues( const void* fromPtr, void* toPtr, size_t numValues ) = 0;

	/*
		Destructs an array of `numValues` values starting at `ptr`.
		All values must have been constructed with createValues().
		Raises co.NotSupportedException if #type cannot be cast to a value.
	 */
	virtual void destroyValues( void* ptr, size_t numValues ) = 0;

	/*
		Returns true if and only if a0 == b0, a1 == b1, ... aN == bN.
		Raises co.NotSupportedException if #type cannot be cast to a value.
	 */
	virtual bool compareValues( const void* a, const void* b, size_t numValues ) = 0;
	
	// End Of c++> Block

	virtual co::int32 getSize() = 0;

	virtual co::IType* getType() = 0;

	virtual void getField( const co::Any& instance, co::IField* field, const co::Any& var ) = 0;

	virtual void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& retVal ) = 0;

	virtual co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider ) = 0;

	virtual co::IObject* newInstance() = 0;

	virtual void raise( const std::string& message ) = 0;

	virtual void setField( const co::Any& instance, co::IField* field, const co::Any& value ) = 0;
};

typedef co::RefPtr<IReflector> IReflectorRef;

} // namespace co

namespace co {
template<> struct kindOf<co::IReflector> : public kindOfBase<TK_INTERFACE> {};
template<> struct nameOf<co::IReflector> { static const char* get() { return "co.IReflector"; } };
} // namespace co

#endif // _CO_IREFLECTOR_H_
