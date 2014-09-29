
#include <co/IDocumentation.h>
#include <co/IDynamicServiceProvider.h>
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

class IDocumentation_Proxy : public co::IDocumentation
{
public:
	IDocumentation_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~IDocumentation_Proxy()
	{
		// empty
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<co::IDocumentation>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// co.IAnnotation Methods:

	// co.IDocumentation Methods:

	std::string getValue()
	{
		std::string res;
		_provider->dynamicGetField( _cookie, getField<co::IDocumentation>( 0 ), res );
		return res;
	}

	void setValue( const std::string& value_ )
	{
		_provider->dynamicSetField( _cookie, getField<co::IDocumentation>( 0 ), value_ );
	}

	void addDocFor( const std::string& member_, const std::string& doc_ )
	{
		co::Any args[] = { member_, doc_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IDocumentation>( 0 ), args, co::Any() );
	}

	std::string getDocFor( const std::string& member_ )
	{
		co::Any args[] = { member_ };
		std::string res;
		_provider->dynamicInvoke( _cookie, getMethod<co::IDocumentation>( 1 ), args, res );
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

class IDocumentation_Reflector : public co::ReflectorBase
{
public:
	IDocumentation_Reflector()
	{
		// empty
	}

	virtual ~IDocumentation_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::IDocumentation>::get();
	}

	co::int32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new co::IDocumentation_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::IDocumentation* p = co::checkInstance<co::IDocumentation>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		value.put( p->getValue() ); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::IDocumentation* p = co::checkInstance<co::IDocumentation>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		p->setValue( value.get< const std::string& >() ); break;
		default:	raiseUnexpectedMemberIndex();
		}
		CORAL_UNUSED( p );
		CORAL_UNUSED( value );
	}

	void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& res )
	{
		co::IDocumentation* p = co::checkInstance<co::IDocumentation>( instance, method );
		checkNumArguments( method, args.getSize() );
		int argIndex = -1;
		try
		{
			switch( method->getIndex() )
			{
			case 1:
				{
					const std::string& member_ = args[++argIndex].get< const std::string& >();
					const std::string& doc_ = args[++argIndex].get< const std::string& >();
					argIndex = -1;
					p->addDocFor( member_, doc_ );
				}
				break;
			case 2:
				{
					const std::string& member_ = args[++argIndex].get< const std::string& >();
					argIndex = -1;
					res.put( p->getDocFor( member_ ) );
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

co::IReflector* __createIDocumentationReflector()
{
    return new IDocumentation_Reflector;
}

} // namespace co
