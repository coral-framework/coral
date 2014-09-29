
#include <lua/IState.h>
#include <co/IDynamicServiceProvider.h>
#include <lua/IInterceptor.h>
#include <co/IMethod.h>
#include <co/IField.h>
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

class IState_Proxy : public lua::IState
{
public:
	IState_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		moduleRetain();
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~IState_Proxy()
	{
		moduleRelease();
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<lua::IState>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// lua.IState Methods:

	co::TSlice<lua::IInterceptor*> getInterceptors()
	{
		std::vector<lua::IInterceptorRef> res;
		_provider->dynamicGetField( _cookie, getField<lua::IState>( 0 ), res );
		return co::moveToSlice<lua::IInterceptor*>( res );
	}

	void addInterceptor( lua::IInterceptor* interceptor_ )
	{
		co::Any args[] = { interceptor_ };
		_provider->dynamicInvoke( _cookie, getMethod<lua::IState>( 0 ), args, co::Any() );
	}

	co::int32 call( const std::string& moduleName_, const std::string& functionName_, co::Slice<co::Any> args_, co::Slice<co::Any> results_ )
	{
		co::Any args[] = { moduleName_, functionName_, args_, results_ };
		co::int32 res;
		_provider->dynamicInvoke( _cookie, getMethod<lua::IState>( 1 ), args, res );
		return res;
	}

	void collectGarbage()
	{
		co::Slice<co::Any> args;
		_provider->dynamicInvoke( _cookie, getMethod<lua::IState>( 2 ), args, co::Any() );
	}

	bool findScript( const std::string& name_, std::string& filename_ )
	{
		co::Any args[] = { name_, filename_ };
		bool res;
		_provider->dynamicInvoke( _cookie, getMethod<lua::IState>( 3 ), args, res );
		return res;
	}

	void removeInterceptor( lua::IInterceptor* interceptor_ )
	{
		co::Any args[] = { interceptor_ };
		_provider->dynamicInvoke( _cookie, getMethod<lua::IState>( 4 ), args, co::Any() );
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

class IState_Reflector : public co::ReflectorBase
{
public:
	IState_Reflector()
	{
		moduleRetain();
	}

	virtual ~IState_Reflector()
	{
		moduleRelease();
	}

	co::IType* getType()
	{
		return co::typeOf<lua::IState>::get();
	}

	co::int32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new lua::IState_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		lua::IState* p = co::checkInstance<lua::IState>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		value.put( p->getInterceptors() ); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		lua::IState* p = co::checkInstance<lua::IState>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		raiseFieldIsReadOnly( field ); break;
		default:	raiseUnexpectedMemberIndex();
		}
		CORAL_UNUSED( p );
		CORAL_UNUSED( value );
	}

	void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& res )
	{
		lua::IState* p = co::checkInstance<lua::IState>( instance, method );
		checkNumArguments( method, args.getSize() );
		int argIndex = -1;
		try
		{
			switch( method->getIndex() )
			{
			case 1:
				{
					lua::IInterceptor* interceptor_ = args[++argIndex].get< lua::IInterceptor* >();
					argIndex = -1;
					p->addInterceptor( interceptor_ );
				}
				break;
			case 2:
				{
					const std::string& moduleName_ = args[++argIndex].get< const std::string& >();
					const std::string& functionName_ = args[++argIndex].get< const std::string& >();
					co::Slice<co::Any> args_ = args[++argIndex].get< co::Slice<co::Any> >();
					co::Slice<co::Any> results_ = args[++argIndex].get< co::Slice<co::Any> >();
					argIndex = -1;
					res.put( p->call( moduleName_, functionName_, args_, results_ ) );
				}
				break;
			case 3:
				{
					p->collectGarbage();
				}
				break;
			case 4:
				{
					const std::string& name_ = args[++argIndex].get< const std::string& >();
					std::string& filename_ = args[++argIndex].get< std::string& >();
					argIndex = -1;
					res.put( p->findScript( name_, filename_ ) );
				}
				break;
			case 5:
				{
					lua::IInterceptor* interceptor_ = args[++argIndex].get< lua::IInterceptor* >();
					argIndex = -1;
					p->removeInterceptor( interceptor_ );
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

co::IReflector* __createIStateReflector()
{
    return new IState_Reflector;
}

} // namespace lua
