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
	
		//------ Manipulation of Values (only available from C++) ------//

		/*!
			Constructs an array of values in the memory area starting at \a ptr.
			\pre The memory area at \a ptr should be at least \a numValues * \a size bytes big.
			\throw NotSupportedException if this \a type does not have a value form.
		 */
		virtual void createValues( void* ptr, size_t numValues ) = 0;

		/*!
			Copies an array of \a numValues values from \a fromPtr to \a toPtr.
			\note Each copy is done using the value's assignment operator.
			\pre Both arrays must contain at least \a numValues properly constructed values of this type.
			\throw NotSupportedException if this \a type does not have a value form.
		 */
		virtual void copyValues( const void* fromPtr, void* toPtr, size_t numValues ) = 0;

		/*!
			Destructs an array of \a numValues values starting at \a ptr.
			\pre The array must contain at least \a numValues values constructed with createValues().
			\throw NotSupportedException if this \a type does not have a value form.
		 */
		virtual void destroyValues( void* ptr, size_t numValues ) = 0;

		/*!
			Returns true if and only if a0 == b0, a1 == b1, ... aN == bN.
			\throw NotSupportedException if this \a type does not have a value form.
		 */
		virtual bool compareValues( const void* a, const void* b, size_t numValues ) = 0;
	
	// End Of c++> Block

	virtual co::uint32 getSize() = 0;

	virtual co::IType* getType() = 0;

	virtual void getField( const co::Any& instance, co::IField* field, const co::Any& value ) = 0;

	virtual void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& returnValue ) = 0;

	virtual co::IService* newDynamicProxy( co::IDynamicServiceProvider* dynamicProvider ) = 0;

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
