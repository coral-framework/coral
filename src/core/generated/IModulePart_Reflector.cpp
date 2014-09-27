
#include <co/IModulePart.h>
#include <co/IDynamicServiceProvider.h>
#include <co/IModule.h>
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

class IModulePart_Proxy : public co::IModulePart
{
public:
	IModulePart_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~IModulePart_Proxy()
	{
		// empty
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<co::IModulePart>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// co.IModulePart Methods:

	void disintegrate( co::IModule* module_ )
	{
		co::Any args[] = { module_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IModulePart>( 0 ), args, co::Any() );
	}

	void dispose( co::IModule* module_ )
	{
		co::Any args[] = { module_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IModulePart>( 1 ), args, co::Any() );
	}

	void initialize( co::IModule* module_ )
	{
		co::Any args[] = { module_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IModulePart>( 2 ), args, co::Any() );
	}

	void integrate( co::IModule* module_ )
	{
		co::Any args[] = { module_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IModulePart>( 3 ), args, co::Any() );
	}

	void integratePresentation( co::IModule* module_ )
	{
		co::Any args[] = { module_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IModulePart>( 4 ), args, co::Any() );
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

class IModulePart_Reflector : public co::ReflectorBase
{
public:
	IModulePart_Reflector()
	{
		// empty
	}

	virtual ~IModulePart_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::IModulePart>::get();
	}

	co::int32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new co::IModulePart_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::checkInstance<co::IModulePart>( instance, field );
		raiseUnexpectedMemberIndex();
		CORAL_UNUSED( value );
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::checkInstance<co::IModulePart>( instance, field );
		raiseUnexpectedMemberIndex();
		CORAL_UNUSED( value );
	}

	void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& res )
	{
		co::IModulePart* p = co::checkInstance<co::IModulePart>( instance, method );
		checkNumArguments( method, args.getSize() );
		int argIndex = -1;
		try
		{
			switch( method->getIndex() )
			{
			case 0:
				{
					co::IModule* module_ = args[++argIndex].get< co::IModule* >();
					argIndex = -1;
					p->disintegrate( module_ );
				}
				break;
			case 1:
				{
					co::IModule* module_ = args[++argIndex].get< co::IModule* >();
					argIndex = -1;
					p->dispose( module_ );
				}
				break;
			case 2:
				{
					co::IModule* module_ = args[++argIndex].get< co::IModule* >();
					argIndex = -1;
					p->initialize( module_ );
				}
				break;
			case 3:
				{
					co::IModule* module_ = args[++argIndex].get< co::IModule* >();
					argIndex = -1;
					p->integrate( module_ );
				}
				break;
			case 4:
				{
					co::IModule* module_ = args[++argIndex].get< co::IModule* >();
					argIndex = -1;
					p->integratePresentation( module_ );
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

co::IReflector* __createIModulePartReflector()
{
    return new IModulePart_Reflector;
}

} // namespace co
