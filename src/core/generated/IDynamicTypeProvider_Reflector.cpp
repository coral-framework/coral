
#include <co/IDynamicTypeProvider.h>
#include <co/IDynamicServiceProvider.h>
#include <co/IType.h>
#include <co/IMethod.h>
#include <co/IField.h>
#include <co/IllegalCastException.h>
#include <co/MissingInputException.h>
#include <co/IllegalArgumentException.h>
#include <co/reserved/ReflectorBase.h>
#include <memory>
#include <sstream>

namespace co {

//------ Dynamic Service Proxy ------//

class IDynamicTypeProvider_Proxy : public co::IDynamicTypeProvider
{
public:
	IDynamicTypeProvider_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~IDynamicTypeProvider_Proxy()
	{
		// empty
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<co::IDynamicTypeProvider>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// co.IAnnotation Methods:

	// co.IDynamicTypeProvider Methods:

	void provideReflectorFor( co::IType* type_ )
	{
		co::Any args[] = { type_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IDynamicTypeProvider>( 0 ), args, co::Any() );
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

class IDynamicTypeProvider_Reflector : public co::ReflectorBase
{
public:
	IDynamicTypeProvider_Reflector()
	{
		// empty
	}

	virtual ~IDynamicTypeProvider_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::IDynamicTypeProvider>::get();
	}

	co::int32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new co::IDynamicTypeProvider_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::checkInstance<co::IDynamicTypeProvider>( instance, field );
		raiseUnexpectedMemberIndex();
		CORAL_UNUSED( value );
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::checkInstance<co::IDynamicTypeProvider>( instance, field );
		raiseUnexpectedMemberIndex();
		CORAL_UNUSED( value );
	}

	void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& res )
	{
		co::IDynamicTypeProvider* p = co::checkInstance<co::IDynamicTypeProvider>( instance, method );
		checkNumArguments( method, args.getSize() );
		int argIndex = -1;
		try
		{
			switch( method->getIndex() )
			{
			case 0:
				{
					co::IType* type_ = args[++argIndex].get< co::IType* >();
					argIndex = -1;
					p->provideReflectorFor( type_ );
				}
				break;
			default:
				raiseUnexpectedMemberIndex();
			}
		}
		catch( co::IllegalCastException& e )
		{
			if( argIndex == -1 )
				throw; // just re-throw if the exception is not related to 'args'
			raiseArgumentTypeException( method, argIndex, e );
		}
		catch( ... )
		{
			throw;
		}
		CORAL_UNUSED( res );
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createIDynamicTypeProviderReflector()
{
    return new IDynamicTypeProvider_Reflector;
}

} // namespace co
