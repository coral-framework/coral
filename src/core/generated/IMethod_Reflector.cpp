
#include <co/IMethod.h>
#include <co/IDynamicServiceProvider.h>
#include <co/IType.h>
#include <co/IParameter.h>
#include <co/IInterface.h>
#include <co/IAnnotation.h>
#include <co/IException.h>
#include <co/ICompositeType.h>
#include <co/IField.h>
#include <co/IllegalCastException.h>
#include <co/MissingInputException.h>
#include <co/IllegalArgumentException.h>
#include <co/reserved/ReflectorBase.h>
#include <memory>
#include <sstream>

namespace co {

//------ Dynamic Service Proxy ------//

class IMethod_Proxy : public co::IMethod
{
public:
	IMethod_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~IMethod_Proxy()
	{
		// empty
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<co::IMethod>::get(); }
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

	// co.IMember Methods:

	co::int16 getIndex()
	{
		co::int16 res;
		_provider->dynamicGetField( _cookie, getField<co::IMember>( 0 ), res );
		return res;
	}

	co::MemberKind getKind()
	{
		co::MemberKind res;
		_provider->dynamicGetField( _cookie, getField<co::IMember>( 1 ), res );
		return res;
	}

	std::string getName()
	{
		std::string res;
		_provider->dynamicGetField( _cookie, getField<co::IMember>( 2 ), res );
		return res;
	}

	co::ICompositeType* getOwner()
	{
		co::ICompositeTypeRef res;
		_provider->dynamicGetField( _cookie, getField<co::IMember>( 3 ), res );
		return res.get();
	}

	// co.IMethod Methods:

	co::TSlice<co::IException*> getExceptions()
	{
		std::vector<co::IExceptionRef> res;
		_provider->dynamicGetField( _cookie, getField<co::IMethod>( 0 ), res );
		return co::moveToSlice<co::IException*>( res );
	}

	co::TSlice<co::IParameter*> getParameters()
	{
		std::vector<co::IParameterRef> res;
		_provider->dynamicGetField( _cookie, getField<co::IMethod>( 1 ), res );
		return co::moveToSlice<co::IParameter*>( res );
	}

	co::IType* getReturnType()
	{
		co::ITypeRef res;
		_provider->dynamicGetField( _cookie, getField<co::IMethod>( 2 ), res );
		return res.get();
	}

protected:
	template<typename T>
	co::IField* getField( co::int32 index )
	{
		return co::typeOf<T>::get()->getFields()[index];
	}

	template<typename T>
	co::IMethod* getMethod( co::int32 index )
	{
		return co::typeOf<T>::get()->getMethods()[index];
	}

private:
	co::IDynamicServiceProvider* _provider;
	co::int32 _cookie;
};

//------ Reflector Component ------//

class IMethod_Reflector : public co::ReflectorBase
{
public:
	IMethod_Reflector()
	{
		// empty
	}

	virtual ~IMethod_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::IMethod>::get();
	}

	co::int32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new co::IMethod_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::IMethod* p = co::checkInstance<co::IMethod>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		value.put( p->getExceptions() ); break;
		case 1:		value.put( p->getParameters() ); break;
		case 2:		value.put( p->getReturnType() ); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::IMethod* p = co::checkInstance<co::IMethod>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		raiseFieldIsReadOnly( field ); break;
		case 1:		raiseFieldIsReadOnly( field ); break;
		case 2:		raiseFieldIsReadOnly( field ); break;
		default:	raiseUnexpectedMemberIndex();
		}
		CORAL_UNUSED( p );
		CORAL_UNUSED( value );
	}

	void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& res )
	{
		co::checkInstance<co::IMethod>( instance, method );
		raiseUnexpectedMemberIndex();
		CORAL_UNUSED( args );
		CORAL_UNUSED( res );
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createIMethodReflector()
{
    return new IMethod_Reflector;
}

} // namespace co
