
#include <co/IReflector.h>
#include <co/IType.h>
#include <co/IMethod.h>
#include <co/IDynamicServiceProvider.h>
#include <co/IField.h>
#include <co/IObject.h>
#include <co/IllegalCastException.h>
#include <co/MissingInputException.h>
#include <co/IllegalArgumentException.h>
#include <co/NotSupportedException.h>
#include <co/reserved/ReflectorBase.h>
#include <memory>
#include <sstream>

namespace co {

//------ Dynamic Service Proxy ------//

class IReflector_Proxy : public co::IReflector
{
public:
	IReflector_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~IReflector_Proxy()
	{
		// empty
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<co::IReflector>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// co.IReflector Methods:

	co::int32 getSize()
	{
		co::int32 res;
		_provider->dynamicGetField( _cookie, getField<co::IReflector>( 0 ), res );
		return res;
	}

	co::IType* getType()
	{
		co::ITypeRef res;
		_provider->dynamicGetField( _cookie, getField<co::IReflector>( 1 ), res );
		return res.get();
	}

	void getField( const co::Any& instance_, co::IField* field_, const co::Any& var_ )
	{
		co::Any args[] = { instance_, field_, var_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IReflector>( 0 ), args, co::Any() );
	}

	void invoke( const co::Any& instance_, co::IMethod* method_, co::Slice<co::Any> args_, const co::Any& retVal_ )
	{
		co::Any args[] = { instance_, method_, args_, retVal_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IReflector>( 1 ), args, co::Any() );
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider_ )
	{
		co::Any args[] = { provider_ };
		co::IServiceRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::IReflector>( 2 ), args, res );
		return res.get();
	}

	co::IObject* newInstance()
	{
		co::Slice<co::Any> args;
		co::IObjectRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::IReflector>( 3 ), args, res );
		return res.get();
	}

	void raise( const std::string& message_ )
	{
		co::Any args[] = { message_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IReflector>( 4 ), args, co::Any() );
	}

	void setField( const co::Any& instance_, co::IField* field_, const co::Any& value_ )
	{
		co::Any args[] = { instance_, field_, value_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IReflector>( 5 ), args, co::Any() );
	}

	// These co::IReflector methods are not part of the reflection system:

	void createValues( void*, size_t )
	{
		throw co::NotSupportedException( "co::IReflector::createValues() cannot be called through a proxy" );
	}

	void copyValues( const void*, void*, size_t )
	{
		throw co::NotSupportedException( "co::IReflector::copyValues() cannot be called through a proxy" );
	}

	void destroyValues( void*, size_t )
	{
		throw co::NotSupportedException( "co::IReflector::destroyValues() cannot be called through a proxy" );
	}

	bool compareValues( const void*, const void*, size_t )
	{
		throw co::NotSupportedException( "co::IReflector::compareValues() cannot be called through a proxy" );
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

class IReflector_Reflector : public co::ReflectorBase
{
public:
	IReflector_Reflector()
	{
		// empty
	}

	virtual ~IReflector_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::IReflector>::get();
	}

	co::int32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new co::IReflector_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::IReflector* p = co::checkInstance<co::IReflector>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		value.put( p->getSize() ); break;
		case 1:		value.put( p->getType() ); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::IReflector* p = co::checkInstance<co::IReflector>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		raiseFieldIsReadOnly( field ); break;
		case 1:		raiseFieldIsReadOnly( field ); break;
		default:	raiseUnexpectedMemberIndex();
		}
		CORAL_UNUSED( p );
		CORAL_UNUSED( value );
	}

	void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& res )
	{
		co::IReflector* p = co::checkInstance<co::IReflector>( instance, method );
		checkNumArguments( method, args.getSize() );
		int argIndex = -1;
		try
		{
			switch( method->getIndex() )
			{
			case 2:
				{
					const co::Any& instance_ = args[++argIndex];
					co::IField* field_ = args[++argIndex].get< co::IField* >();
					const co::Any& var_ = args[++argIndex];
					argIndex = -1;
					p->getField( instance_, field_, var_ );
				}
				break;
			case 3:
				{
					const co::Any& instance_ = args[++argIndex];
					co::IMethod* method_ = args[++argIndex].get< co::IMethod* >();
					co::Slice<co::Any> args_ = args[++argIndex].get< co::Slice<co::Any> >();
					const co::Any& retVal_ = args[++argIndex];
					argIndex = -1;
					p->invoke( instance_, method_, args_, retVal_ );
				}
				break;
			case 4:
				{
					co::IDynamicServiceProvider* provider_ = args[++argIndex].get< co::IDynamicServiceProvider* >();
					argIndex = -1;
					res.put( p->newDynamicProxy( provider_ ) );
				}
				break;
			case 5:
				{
					res.put( p->newInstance() );
				}
				break;
			case 6:
				{
					const std::string& message_ = args[++argIndex].get< const std::string& >();
					argIndex = -1;
					p->raise( message_ );
				}
				break;
			case 7:
				{
					const co::Any& instance_ = args[++argIndex];
					co::IField* field_ = args[++argIndex].get< co::IField* >();
					const co::Any& value_ = args[++argIndex];
					argIndex = -1;
					p->setField( instance_, field_, value_ );
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

co::IReflector* __createIReflectorReflector()
{
    return new IReflector_Reflector;
}

} // namespace co
