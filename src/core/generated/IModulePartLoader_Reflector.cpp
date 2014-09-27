
#include <co/IModulePartLoader.h>
#include <co/IDynamicServiceProvider.h>
#include <co/IModulePart.h>
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

class IModulePartLoader_Proxy : public co::IModulePartLoader
{
public:
	IModulePartLoader_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~IModulePartLoader_Proxy()
	{
		// empty
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<co::IModulePartLoader>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// co.IModulePartLoader Methods:

	bool canLoadModulePart( const std::string& moduleName_ )
	{
		co::Any args[] = { moduleName_ };
		bool res;
		_provider->dynamicInvoke( _cookie, getMethod<co::IModulePartLoader>( 0 ), args, res );
		return res;
	}

	co::IModulePart* loadModulePart( const std::string& moduleName_ )
	{
		co::Any args[] = { moduleName_ };
		co::IModulePartRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::IModulePartLoader>( 1 ), args, res );
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

class IModulePartLoader_Reflector : public co::ReflectorBase
{
public:
	IModulePartLoader_Reflector()
	{
		// empty
	}

	virtual ~IModulePartLoader_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::IModulePartLoader>::get();
	}

	co::int32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new co::IModulePartLoader_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::checkInstance<co::IModulePartLoader>( instance, field );
		raiseUnexpectedMemberIndex();
		CORAL_UNUSED( value );
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::checkInstance<co::IModulePartLoader>( instance, field );
		raiseUnexpectedMemberIndex();
		CORAL_UNUSED( value );
	}

	void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& res )
	{
		co::IModulePartLoader* p = co::checkInstance<co::IModulePartLoader>( instance, method );
		checkNumArguments( method, args.getSize() );
		int argIndex = -1;
		try
		{
			switch( method->getIndex() )
			{
			case 0:
				{
					const std::string& moduleName_ = args[++argIndex].get< const std::string& >();
					argIndex = -1;
					res.put( p->canLoadModulePart( moduleName_ ) );
				}
				break;
			case 1:
				{
					const std::string& moduleName_ = args[++argIndex].get< const std::string& >();
					argIndex = -1;
					res.put( p->loadModulePart( moduleName_ ) );
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

co::IReflector* __createIModulePartLoaderReflector()
{
    return new IModulePartLoader_Reflector;
}

} // namespace co
