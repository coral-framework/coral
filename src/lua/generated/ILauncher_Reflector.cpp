
#include <lua/ILauncher.h>
#include <co/IDynamicServiceProvider.h>
#include <co/IMethod.h>
#include <co/IField.h>
#include <co/IllegalCastException.h>
#include <co/MissingInputException.h>
#include <co/IllegalArgumentException.h>
#include <co/reserved/ReflectorBase.h>
#include <memory>
#include <sstream>

namespace lua {

void moduleRetain();
void moduleRelease();

//------ Dynamic Service Proxy ------//

class ILauncher_Proxy : public lua::ILauncher
{
public:
	ILauncher_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		moduleRetain();
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~ILauncher_Proxy()
	{
		moduleRelease();
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<lua::ILauncher>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// lua.ILauncher Methods:

	co::int32 main( co::Slice<std::string> args_ )
	{
		co::Any args[] = { args_ };
		co::int32 res;
		_provider->dynamicInvoke( _cookie, getMethod<lua::ILauncher>( 0 ), args, res );
		return res;
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

class ILauncher_Reflector : public co::ReflectorBase
{
public:
	ILauncher_Reflector()
	{
		moduleRetain();
	}

	virtual ~ILauncher_Reflector()
	{
		moduleRelease();
	}

	co::IType* getType()
	{
		return co::typeOf<lua::ILauncher>::get();
	}

	co::int32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new lua::ILauncher_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::checkInstance<lua::ILauncher>( instance, field );
		raiseUnexpectedMemberIndex();
		CORAL_UNUSED( value );
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::checkInstance<lua::ILauncher>( instance, field );
		raiseUnexpectedMemberIndex();
		CORAL_UNUSED( value );
	}

	void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& res )
	{
		lua::ILauncher* p = co::checkInstance<lua::ILauncher>( instance, method );
		checkNumArguments( method, args.getSize() );
		int argIndex = -1;
		try
		{
			switch( method->getIndex() )
			{
			case 0:
				{
					co::Slice<std::string> args_ = args[++argIndex].get< co::Slice<std::string> >();
					argIndex = -1;
					res.put( p->main( args_ ) );
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

co::IReflector* __createILauncherReflector()
{
    return new ILauncher_Reflector;
}

} // namespace lua
