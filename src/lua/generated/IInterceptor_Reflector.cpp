
#include <lua/IInterceptor.h>
#include <co/IDynamicServiceProvider.h>
#include <co/IMethod.h>
#include <co/IObject.h>
#include <co/IField.h>
#include <co/IPort.h>
#include <co/IllegalCastException.h>
#include <co/MissingInputException.h>
#include <co/IllegalArgumentException.h>
#include <co/private/ReflectorBase.h>
#include <memory>
#include <sstream>

namespace lua {

void moduleRetain();
void moduleRelease();

//------ Dynamic Service Proxy ------//

class IInterceptor_Proxy : public lua::IInterceptor
{
public:
	IInterceptor_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		moduleRetain();
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~IInterceptor_Proxy()
	{
		moduleRelease();
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<lua::IInterceptor>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// lua.IInterceptor Methods:

	void postGetField( co::IService* service_, co::IField* field_, const co::Any& value_ )
	{
		co::Any args[] = { service_, field_, value_ };
		_provider->dynamicInvoke( _cookie, getMethod<lua::IInterceptor>( 0 ), args, co::Any() );
	}

	void postGetService( co::IObject* object_, co::IPort* port_, co::IService* service_ )
	{
		co::Any args[] = { object_, port_, service_ };
		_provider->dynamicInvoke( _cookie, getMethod<lua::IInterceptor>( 1 ), args, co::Any() );
	}

	void postInvoke( co::IService* service_, co::IMethod* method_, co::Slice<co::Any> args_, const co::Any& returnValue_ )
	{
		co::Any args[] = { service_, method_, args_, returnValue_ };
		_provider->dynamicInvoke( _cookie, getMethod<lua::IInterceptor>( 2 ), args, co::Any() );
	}

	void postSetField( co::IService* service_, co::IField* field_, const co::Any& value_ )
	{
		co::Any args[] = { service_, field_, value_ };
		_provider->dynamicInvoke( _cookie, getMethod<lua::IInterceptor>( 3 ), args, co::Any() );
	}

	void postSetService( co::IObject* object_, co::IPort* receptable_, co::IService* service_ )
	{
		co::Any args[] = { object_, receptable_, service_ };
		_provider->dynamicInvoke( _cookie, getMethod<lua::IInterceptor>( 4 ), args, co::Any() );
	}

	void serviceReleased( co::IService* service_ )
	{
		co::Any args[] = { service_ };
		_provider->dynamicInvoke( _cookie, getMethod<lua::IInterceptor>( 5 ), args, co::Any() );
	}

	void serviceRetained( co::IService* service_ )
	{
		co::Any args[] = { service_ };
		_provider->dynamicInvoke( _cookie, getMethod<lua::IInterceptor>( 6 ), args, co::Any() );
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

class IInterceptor_Reflector : public co::ReflectorBase
{
public:
	IInterceptor_Reflector()
	{
		moduleRetain();
	}

	virtual ~IInterceptor_Reflector()
	{
		moduleRelease();
	}

	co::IType* getType()
	{
		return co::typeOf<lua::IInterceptor>::get();
	}

	co::int32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new lua::IInterceptor_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::checkInstance<lua::IInterceptor>( instance, field );
		raiseUnexpectedMemberIndex();
		CORAL_UNUSED( value );
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::checkInstance<lua::IInterceptor>( instance, field );
		raiseUnexpectedMemberIndex();
		CORAL_UNUSED( value );
	}

	void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& res )
	{
		lua::IInterceptor* p = co::checkInstance<lua::IInterceptor>( instance, method );
		checkNumArguments( method, args.getSize() );
		int argIndex = -1;
		try
		{
			switch( method->getIndex() )
			{
			case 0:
				{
					co::IService* service_ = args[++argIndex].get< co::IService* >();
					co::IField* field_ = args[++argIndex].get< co::IField* >();
					const co::Any& value_ = args[++argIndex];
					argIndex = -1;
					p->postGetField( service_, field_, value_ );
				}
				break;
			case 1:
				{
					co::IObject* object_ = args[++argIndex].get< co::IObject* >();
					co::IPort* port_ = args[++argIndex].get< co::IPort* >();
					co::IService* service_ = args[++argIndex].get< co::IService* >();
					argIndex = -1;
					p->postGetService( object_, port_, service_ );
				}
				break;
			case 2:
				{
					co::IService* service_ = args[++argIndex].get< co::IService* >();
					co::IMethod* method_ = args[++argIndex].get< co::IMethod* >();
					co::Slice<co::Any> args_ = args[++argIndex].get< co::Slice<co::Any> >();
					const co::Any& returnValue_ = args[++argIndex];
					argIndex = -1;
					p->postInvoke( service_, method_, args_, returnValue_ );
				}
				break;
			case 3:
				{
					co::IService* service_ = args[++argIndex].get< co::IService* >();
					co::IField* field_ = args[++argIndex].get< co::IField* >();
					const co::Any& value_ = args[++argIndex];
					argIndex = -1;
					p->postSetField( service_, field_, value_ );
				}
				break;
			case 4:
				{
					co::IObject* object_ = args[++argIndex].get< co::IObject* >();
					co::IPort* receptable_ = args[++argIndex].get< co::IPort* >();
					co::IService* service_ = args[++argIndex].get< co::IService* >();
					argIndex = -1;
					p->postSetService( object_, receptable_, service_ );
				}
				break;
			case 5:
				{
					co::IService* service_ = args[++argIndex].get< co::IService* >();
					argIndex = -1;
					p->serviceReleased( service_ );
				}
				break;
			case 6:
				{
					co::IService* service_ = args[++argIndex].get< co::IService* >();
					argIndex = -1;
					p->serviceRetained( service_ );
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

co::IReflector* __createIInterceptorReflector()
{
    return new IInterceptor_Reflector;
}

} // namespace lua
