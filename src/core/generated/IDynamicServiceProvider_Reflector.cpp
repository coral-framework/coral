
#include <co/IDynamicServiceProvider.h>
#include <co/IMethod.h>
#include <co/IPort.h>
#include <co/IField.h>
#include <co/IllegalCastException.h>
#include <co/MissingInputException.h>
#include <co/IllegalArgumentException.h>
#include <co/reserved/ReflectorBase.h>
#include <memory>
#include <sstream>

namespace co {

//------ Dynamic Service Proxy ------//

class IDynamicServiceProvider_Proxy : public co::IDynamicServiceProvider
{
public:
	IDynamicServiceProvider_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~IDynamicServiceProvider_Proxy()
	{
		// empty
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<co::IDynamicServiceProvider>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// co.IDynamicServiceProvider Methods:

	co::IPort* dynamicGetFacet( co::int32 proxyId_ )
	{
		co::Any args[] = { proxyId_ };
		co::IPortRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::IDynamicServiceProvider>( 0 ), args, res );
		return res.get();
	}

	void dynamicGetField( co::int32 proxyId_, co::IField* field_, const co::Any& var_ )
	{
		co::Any args[] = { proxyId_, field_, var_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IDynamicServiceProvider>( 1 ), args, co::Any() );
	}

	void dynamicInvoke( co::int32 proxyId_, co::IMethod* method_, co::Slice<co::Any> args_, const co::Any& retVal_ )
	{
		co::Any args[] = { proxyId_, method_, args_, retVal_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IDynamicServiceProvider>( 2 ), args, co::Any() );
	}

	co::int32 dynamicRegisterService( co::IService* proxy_ )
	{
		co::Any args[] = { proxy_ };
		co::int32 res;
		_provider->dynamicInvoke( _cookie, getMethod<co::IDynamicServiceProvider>( 3 ), args, res );
		return res;
	}

	void dynamicSetField( co::int32 proxyId_, co::IField* field_, const co::Any& value_ )
	{
		co::Any args[] = { proxyId_, field_, value_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IDynamicServiceProvider>( 4 ), args, co::Any() );
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

class IDynamicServiceProvider_Reflector : public co::ReflectorBase
{
public:
	IDynamicServiceProvider_Reflector()
	{
		// empty
	}

	virtual ~IDynamicServiceProvider_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::IDynamicServiceProvider>::get();
	}

	co::int32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new co::IDynamicServiceProvider_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::checkInstance<co::IDynamicServiceProvider>( instance, field );
		raiseUnexpectedMemberIndex();
		CORAL_UNUSED( value );
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::checkInstance<co::IDynamicServiceProvider>( instance, field );
		raiseUnexpectedMemberIndex();
		CORAL_UNUSED( value );
	}

	void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& res )
	{
		co::IDynamicServiceProvider* p = co::checkInstance<co::IDynamicServiceProvider>( instance, method );
		checkNumArguments( method, args.getSize() );
		int argIndex = -1;
		try
		{
			switch( method->getIndex() )
			{
			case 0:
				{
					co::int32 proxyId_ = args[++argIndex].get< co::int32 >();
					argIndex = -1;
					res.put( p->dynamicGetFacet( proxyId_ ) );
				}
				break;
			case 1:
				{
					co::int32 proxyId_ = args[++argIndex].get< co::int32 >();
					co::IField* field_ = args[++argIndex].get< co::IField* >();
					const co::Any& var_ = args[++argIndex];
					argIndex = -1;
					p->dynamicGetField( proxyId_, field_, var_ );
				}
				break;
			case 2:
				{
					co::int32 proxyId_ = args[++argIndex].get< co::int32 >();
					co::IMethod* method_ = args[++argIndex].get< co::IMethod* >();
					co::Slice<co::Any> args_ = args[++argIndex].get< co::Slice<co::Any> >();
					const co::Any& retVal_ = args[++argIndex];
					argIndex = -1;
					p->dynamicInvoke( proxyId_, method_, args_, retVal_ );
				}
				break;
			case 3:
				{
					co::IService* proxy_ = args[++argIndex].get< co::IService* >();
					argIndex = -1;
					res.put( p->dynamicRegisterService( proxy_ ) );
				}
				break;
			case 4:
				{
					co::int32 proxyId_ = args[++argIndex].get< co::int32 >();
					co::IField* field_ = args[++argIndex].get< co::IField* >();
					const co::Any& value_ = args[++argIndex];
					argIndex = -1;
					p->dynamicSetField( proxyId_, field_, value_ );
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

co::IReflector* __createIDynamicServiceProviderReflector()
{
    return new IDynamicServiceProvider_Reflector;
}

} // namespace co
