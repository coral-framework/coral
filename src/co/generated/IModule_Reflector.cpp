
#include <co/IModule.h>
#include <co/IDynamicServiceProvider.h>
#include <co/INamespace.h>
#include <co/IModulePart.h>
#include <co/IMethod.h>
#include <co/IField.h>
#include <co/IllegalCastException.h>
#include <co/MissingInputException.h>
#include <co/IllegalArgumentException.h>
#include <co/private/ReflectorBase.h>
#include <memory>
#include <sstream>

namespace co {

//------ Dynamic Service Proxy ------//

class IModule_Proxy : public co::IModule
{
public:
	IModule_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~IModule_Proxy()
	{
		// empty
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<co::IModule>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// co.IModule Methods:

	co::INamespace* getNamespace()
	{
		co::INamespaceRef res;
		_provider->dynamicGetField( _cookie, getField<co::IModule>( 0 ), res );
		return res.get();
	}

	co::TSlice<co::IModulePart*> getParts()
	{
		std::vector<co::IModulePartRef> res;
		_provider->dynamicGetField( _cookie, getField<co::IModule>( 1 ), res );
		return co::moveToSlice<co::IModulePart*>( res );
	}

	co::int32 getRank()
	{
		co::int32 res;
		_provider->dynamicGetField( _cookie, getField<co::IModule>( 2 ), res );
		return res;
	}

	void setRank( co::int32 rank_ )
	{
		_provider->dynamicSetField( _cookie, getField<co::IModule>( 2 ), rank_ );
	}

	co::ModuleState getState()
	{
		co::ModuleState res;
		_provider->dynamicGetField( _cookie, getField<co::IModule>( 3 ), res );
		return res;
	}

	void abort()
	{
		co::Slice<co::Any> args;
		_provider->dynamicInvoke( _cookie, getMethod<co::IModule>( 0 ), args, co::Any() );
	}

	void disintegrate()
	{
		co::Slice<co::Any> args;
		_provider->dynamicInvoke( _cookie, getMethod<co::IModule>( 1 ), args, co::Any() );
	}

	void dispose()
	{
		co::Slice<co::Any> args;
		_provider->dynamicInvoke( _cookie, getMethod<co::IModule>( 2 ), args, co::Any() );
	}

	void initialize()
	{
		co::Slice<co::Any> args;
		_provider->dynamicInvoke( _cookie, getMethod<co::IModule>( 3 ), args, co::Any() );
	}

	void integrate()
	{
		co::Slice<co::Any> args;
		_provider->dynamicInvoke( _cookie, getMethod<co::IModule>( 4 ), args, co::Any() );
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

class IModule_Reflector : public co::ReflectorBase
{
public:
	IModule_Reflector()
	{
		// empty
	}

	virtual ~IModule_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::IModule>::get();
	}

	co::int32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new co::IModule_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::IModule* p = co::checkInstance<co::IModule>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		value.put( p->getNamespace() ); break;
		case 1:		value.put( p->getParts() ); break;
		case 2:		value.put( p->getRank() ); break;
		case 3:		value.put( p->getState() ); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::IModule* p = co::checkInstance<co::IModule>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		raiseFieldIsReadOnly( field ); break;
		case 1:		raiseFieldIsReadOnly( field ); break;
		case 2:		p->setRank( value.get< co::int32 >() ); break;
		case 3:		raiseFieldIsReadOnly( field ); break;
		default:	raiseUnexpectedMemberIndex();
		}
		CORAL_UNUSED( p );
		CORAL_UNUSED( value );
	}

	void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& res )
	{
		co::IModule* p = co::checkInstance<co::IModule>( instance, method );
		checkNumArguments( method, args.getSize() );
		int argIndex = -1;
		try
		{
			switch( method->getIndex() )
			{
			case 4:
				{
					p->abort();
				}
				break;
			case 5:
				{
					p->disintegrate();
				}
				break;
			case 6:
				{
					p->dispose();
				}
				break;
			case 7:
				{
					p->initialize();
				}
				break;
			case 8:
				{
					p->integrate();
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

co::IReflector* __createIModuleReflector()
{
    return new IModule_Reflector;
}

} // namespace co
