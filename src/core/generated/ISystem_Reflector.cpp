
#include <co/ISystem.h>
#include <co/IDynamicServiceProvider.h>
#include <co/IModuleManager.h>
#include <co/IServiceManager.h>
#include <co/ITypeManager.h>
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

class ISystem_Proxy : public co::ISystem
{
public:
	ISystem_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~ISystem_Proxy()
	{
		// empty
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<co::ISystem>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// co.ISystem Methods:

	co::IModuleManager* getModules()
	{
		co::IModuleManagerRef res;
		_provider->dynamicGetField( _cookie, getField<co::ISystem>( 0 ), res );
		return res.get();
	}

	co::IServiceManager* getServices()
	{
		co::IServiceManagerRef res;
		_provider->dynamicGetField( _cookie, getField<co::ISystem>( 1 ), res );
		return res.get();
	}

	co::SystemState getState()
	{
		co::SystemState res;
		_provider->dynamicGetField( _cookie, getField<co::ISystem>( 2 ), res );
		return res;
	}

	co::ITypeManager* getTypes()
	{
		co::ITypeManagerRef res;
		_provider->dynamicGetField( _cookie, getField<co::ISystem>( 3 ), res );
		return res.get();
	}

	void setupBase( co::Slice<std::string> requiredModules_ )
	{
		co::Any args[] = { requiredModules_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::ISystem>( 0 ), args, co::Any() );
	}

	void setupPresentation()
	{
		co::Slice<co::Any> args;
		_provider->dynamicInvoke( _cookie, getMethod<co::ISystem>( 1 ), args, co::Any() );
	}

	void tearDown()
	{
		co::Slice<co::Any> args;
		_provider->dynamicInvoke( _cookie, getMethod<co::ISystem>( 2 ), args, co::Any() );
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

class ISystem_Reflector : public co::ReflectorBase
{
public:
	ISystem_Reflector()
	{
		// empty
	}

	virtual ~ISystem_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::ISystem>::get();
	}

	co::int32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new co::ISystem_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::ISystem* p = co::checkInstance<co::ISystem>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		value.put( p->getModules() ); break;
		case 1:		value.put( p->getServices() ); break;
		case 2:		value.put( p->getState() ); break;
		case 3:		value.put( p->getTypes() ); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::ISystem* p = co::checkInstance<co::ISystem>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		raiseFieldIsReadOnly( field ); break;
		case 1:		raiseFieldIsReadOnly( field ); break;
		case 2:		raiseFieldIsReadOnly( field ); break;
		case 3:		raiseFieldIsReadOnly( field ); break;
		default:	raiseUnexpectedMemberIndex();
		}
		CORAL_UNUSED( p );
		CORAL_UNUSED( value );
	}

	void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& res )
	{
		co::ISystem* p = co::checkInstance<co::ISystem>( instance, method );
		checkNumArguments( method, args.getSize() );
		int argIndex = -1;
		try
		{
			switch( method->getIndex() )
			{
			case 4:
				{
					co::Slice<std::string> requiredModules_ = args[++argIndex].get< co::Slice<std::string> >();
					argIndex = -1;
					p->setupBase( requiredModules_ );
				}
				break;
			case 5:
				{
					p->setupPresentation();
				}
				break;
			case 6:
				{
					p->tearDown();
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

co::IReflector* __createISystemReflector()
{
    return new ISystem_Reflector;
}

} // namespace co
