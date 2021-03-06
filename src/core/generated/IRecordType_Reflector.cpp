#include <co/IRecordType.h>
#include <co/IDynamicServiceProvider.h>
#include <co/IField.h>
#include <co/IInterface.h>
#include <co/IType.h>
#include <co/Uuid.h>
#include <co/IReflector.h>
#include <co/INamespace.h>
#include <co/IAnnotation.h>
#include <co/IMember.h>
#include <co/IMethod.h>
#include <co/IllegalCastException.h>
#include <co/MissingInputException.h>
#include <co/IllegalArgumentException.h>
#include <co/reserved/ReflectorBase.h>
#include <memory>
#include <sstream>

namespace co {

//------ Dynamic Service Proxy ------//

class IRecordType_Proxy : public co::IRecordType
{
public:
	IRecordType_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~IRecordType_Proxy()
	{
		// empty
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<co::IRecordType>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// co.IAnnotated Methods:

	co::TSlice<co::IAnnotation*> getAnnotations()
	{
		std::vector<co::IAnnotationRef> res;
		_provider->dynamicGetField( _cookie, getField<co::IAnnotated>( 0 ), res );
		return co::moveToSlice<co::IAnnotation*>( res );
	}

	void setAnnotations( co::Slice<co::IAnnotation*> annotations_ )
	{
		_provider->dynamicSetField( _cookie, getField<co::IAnnotated>( 0 ), annotations_ );
	}

	void addAnnotation( co::IAnnotation* annotation_ )
	{
		co::Any args[] = { annotation_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IAnnotated>( 0 ), args, co::Any() );
	}

	co::IAnnotation* findAnnotation( co::IInterface* annotationType_ )
	{
		co::Any args[] = { annotationType_ };
		co::IAnnotationRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::IAnnotated>( 1 ), args, res );
		return res.get();
	}

	// co.IType Methods:

	co::Uuid getBinarySignature()
	{
		co::Uuid res;
		_provider->dynamicGetField( _cookie, getField<co::IType>( 0 ), res );
		return res;
	}

	co::IReflector* getCurrentReflector()
	{
		co::IReflectorRef res;
		_provider->dynamicGetField( _cookie, getField<co::IType>( 1 ), res );
		return res.get();
	}

	std::string getFullName()
	{
		std::string res;
		_provider->dynamicGetField( _cookie, getField<co::IType>( 2 ), res );
		return res;
	}

	co::Uuid getFullSignature()
	{
		co::Uuid res;
		_provider->dynamicGetField( _cookie, getField<co::IType>( 3 ), res );
		return res;
	}

	co::TypeKind getKind()
	{
		co::TypeKind res;
		_provider->dynamicGetField( _cookie, getField<co::IType>( 4 ), res );
		return res;
	}

	std::string getName()
	{
		std::string res;
		_provider->dynamicGetField( _cookie, getField<co::IType>( 5 ), res );
		return res;
	}

	co::INamespace* getNamespace()
	{
		co::INamespaceRef res;
		_provider->dynamicGetField( _cookie, getField<co::IType>( 6 ), res );
		return res.get();
	}

	co::IReflector* getReflector()
	{
		co::IReflectorRef res;
		_provider->dynamicGetField( _cookie, getField<co::IType>( 7 ), res );
		return res.get();
	}

	void setReflector( co::IReflector* reflector_ )
	{
		_provider->dynamicSetField( _cookie, getField<co::IType>( 7 ), reflector_ );
	}

	bool isA( co::IType* type_ )
	{
		co::Any args[] = { type_ };
		bool res;
		_provider->dynamicInvoke( _cookie, getMethod<co::IType>( 0 ), args, res );
		return res;
	}

	// co.ICompositeType Methods:

	co::TSlice<co::IMember*> getMembers()
	{
		std::vector<co::IMemberRef> res;
		_provider->dynamicGetField( _cookie, getField<co::ICompositeType>( 0 ), res );
		return co::moveToSlice<co::IMember*>( res );
	}

	co::IMember* getMember( const std::string& name_ )
	{
		co::Any args[] = { name_ };
		co::IMemberRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::ICompositeType>( 0 ), args, res );
		return res.get();
	}

	// co.IRecordType Methods:

	co::TSlice<co::IField*> getFields()
	{
		std::vector<co::IFieldRef> res;
		_provider->dynamicGetField( _cookie, getField<co::IRecordType>( 0 ), res );
		return co::moveToSlice<co::IField*>( res );
	}

protected:
	template<typename T>
	co::IField* getField( co::uint32 index )
	{
		return co::typeOf<T>::get()->getFields()[index];
	}

	template<typename T>
	co::IMethod* getMethod( co::uint32 index )
	{
		return co::typeOf<T>::get()->getMethods()[index];
	}

private:
	co::IDynamicServiceProvider* _provider;
	co::uint32 _cookie;
};

//------ Reflector Component ------//

class IRecordType_Reflector : public co::ReflectorBase
{
public:
	IRecordType_Reflector()
	{
		// empty
	}

	virtual ~IRecordType_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::IRecordType>::get();
	}

	co::uint32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new co::IRecordType_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::IRecordType* p = co::checkInstance<co::IRecordType>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		value.put( p->getFields() ); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::IRecordType* p = co::checkInstance<co::IRecordType>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		raiseFieldIsReadOnly( field ); break;
		default:	raiseUnexpectedMemberIndex();
		}
		CORAL_UNUSED( p );
		CORAL_UNUSED( value );
	}

	void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& res )
	{
		co::checkInstance<co::IRecordType>( instance, method );
		raiseUnexpectedMemberIndex();
		CORAL_UNUSED( args );
		CORAL_UNUSED( res );
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createIRecordTypeReflector()
{
    return new IRecordType_Reflector;
}

} // namespace co
