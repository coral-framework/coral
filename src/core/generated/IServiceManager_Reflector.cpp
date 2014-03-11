#include <co/IServiceManager.h>
#include <co/IDynamicServiceProvider.h>
#include <co/IInterface.h>
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

class IServiceManager_Proxy : public co::IServiceManager
{
public:
	IServiceManager_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~IServiceManager_Proxy()
	{
		// empty
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<co::IServiceManager>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// co.IServiceManager Methods:

	bool getIsLazy()
	{
		bool res;
		_provider->dynamicGetField( _cookie, getField<co::IServiceManager>( 0 ), res );
		return res;
	}

	void setIsLazy( bool isLazy_ )
	{
		_provider->dynamicSetField( _cookie, getField<co::IServiceManager>( 0 ), isLazy_ );
	}

	void addService( co::IInterface* serviceType_, co::IService* service_ )
	{
		co::Any args[] = { serviceType_, service_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IServiceManager>( 0 ), args, co::Any() );
	}

	void addServiceForType( co::IInterface* serviceType_, co::IInterface* clientType_, co::IService* service_ )
	{
		co::Any args[] = { serviceType_, clientType_, service_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IServiceManager>( 1 ), args, co::Any() );
	}

	void addServiceProvider( co::IInterface* serviceType_, const std::string& componentName_ )
	{
		co::Any args[] = { serviceType_, componentName_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IServiceManager>( 2 ), args, co::Any() );
	}

	void addServiceProviderForType( co::IInterface* serviceType_, co::IInterface* clientType_, const std::string& componentName_ )
	{
		co::Any args[] = { serviceType_, clientType_, componentName_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IServiceManager>( 3 ), args, co::Any() );
	}

	co::IService* getService( co::IInterface* serviceType_ )
	{
		co::Any args[] = { serviceType_ };
		co::IServiceRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::IServiceManager>( 4 ), args, res );
		return res.get();
	}

	co::IService* getServiceForInstance( co::IInterface* serviceType_, co::IService* client_ )
	{
		co::Any args[] = { serviceType_, client_ };
		co::IServiceRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::IServiceManager>( 5 ), args, res );
		return res.get();
	}

	co::IService* getServiceForType( co::IInterface* serviceType_, co::IInterface* clientType_ )
	{
		co::Any args[] = { serviceType_, clientType_ };
		co::IServiceRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::IServiceManager>( 6 ), args, res );
		return res.get();
	}

	void removeService( co::IInterface* serviceType_ )
	{
		co::Any args[] = { serviceType_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IServiceManager>( 7 ), args, co::Any() );
	}

	void removeServiceForType( co::IInterface* serviceType_, co::IInterface* clientType_ )
	{
		co::Any args[] = { serviceType_, clientType_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IServiceManager>( 8 ), args, co::Any() );
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

class IServiceManager_Reflector : public co::ReflectorBase
{
public:
	IServiceManager_Reflector()
	{
		// empty
	}

	virtual ~IServiceManager_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::IServiceManager>::get();
	}

	co::uint32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new co::IServiceManager_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::IServiceManager* p = co::checkInstance<co::IServiceManager>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		value.put( p->getIsLazy() ); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::IServiceManager* p = co::checkInstance<co::IServiceManager>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		p->setIsLazy( value.get< bool >() ); break;
		default:	raiseUnexpectedMemberIndex();
		}
		CORAL_UNUSED( p );
		CORAL_UNUSED( value );
	}

	void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& res )
	{
		co::IServiceManager* p = co::checkInstance<co::IServiceManager>( instance, method );
		checkNumArguments( method, args.getSize() );
		int argIndex = -1;
		try
		{
			switch( method->getIndex() )
			{
			case 1:
				{
					co::IInterface* serviceType_ = args[++argIndex].get< co::IInterface* >();
					co::IService* service_ = args[++argIndex].get< co::IService* >();
					argIndex = -1;
					p->addService( serviceType_, service_ );
				}
				break;
			case 2:
				{
					co::IInterface* serviceType_ = args[++argIndex].get< co::IInterface* >();
					co::IInterface* clientType_ = args[++argIndex].get< co::IInterface* >();
					co::IService* service_ = args[++argIndex].get< co::IService* >();
					argIndex = -1;
					p->addServiceForType( serviceType_, clientType_, service_ );
				}
				break;
			case 3:
				{
					co::IInterface* serviceType_ = args[++argIndex].get< co::IInterface* >();
					const std::string& componentName_ = args[++argIndex].get< const std::string& >();
					argIndex = -1;
					p->addServiceProvider( serviceType_, componentName_ );
				}
				break;
			case 4:
				{
					co::IInterface* serviceType_ = args[++argIndex].get< co::IInterface* >();
					co::IInterface* clientType_ = args[++argIndex].get< co::IInterface* >();
					const std::string& componentName_ = args[++argIndex].get< const std::string& >();
					argIndex = -1;
					p->addServiceProviderForType( serviceType_, clientType_, componentName_ );
				}
				break;
			case 5:
				{
					co::IInterface* serviceType_ = args[++argIndex].get< co::IInterface* >();
					argIndex = -1;
					res.put( p->getService( serviceType_ ) );
				}
				break;
			case 6:
				{
					co::IInterface* serviceType_ = args[++argIndex].get< co::IInterface* >();
					co::IService* client_ = args[++argIndex].get< co::IService* >();
					argIndex = -1;
					res.put( p->getServiceForInstance( serviceType_, client_ ) );
				}
				break;
			case 7:
				{
					co::IInterface* serviceType_ = args[++argIndex].get< co::IInterface* >();
					co::IInterface* clientType_ = args[++argIndex].get< co::IInterface* >();
					argIndex = -1;
					res.put( p->getServiceForType( serviceType_, clientType_ ) );
				}
				break;
			case 8:
				{
					co::IInterface* serviceType_ = args[++argIndex].get< co::IInterface* >();
					argIndex = -1;
					p->removeService( serviceType_ );
				}
				break;
			case 9:
				{
					co::IInterface* serviceType_ = args[++argIndex].get< co::IInterface* >();
					co::IInterface* clientType_ = args[++argIndex].get< co::IInterface* >();
					argIndex = -1;
					p->removeServiceForType( serviceType_, clientType_ );
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

co::IReflector* __createIServiceManagerReflector()
{
    return new IServiceManager_Reflector;
}

} // namespace co
