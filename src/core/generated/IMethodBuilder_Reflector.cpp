#include <co/IMethodBuilder.h>
#include <co/IDynamicServiceProvider.h>
#include <co/IException.h>
#include <co/IType.h>
#include <co/ITypeBuilder.h>
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

class IMethodBuilder_Proxy : public co::IMethodBuilder
{
public:
	IMethodBuilder_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~IMethodBuilder_Proxy()
	{
		// empty
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<co::IMethodBuilder>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// co.IMethodBuilder Methods:

	std::string getMethodName()
	{
		std::string res;
		_provider->dynamicGetField( _cookie, getField<co::IMethodBuilder>( 0 ), res );
		return res;
	}

	co::ITypeBuilder* getTypeBuilder()
	{
		co::ITypeBuilderRef res;
		_provider->dynamicGetField( _cookie, getField<co::IMethodBuilder>( 1 ), res );
		return res.get();
	}

	void createMethod()
	{
		co::Slice<co::Any> args;
		_provider->dynamicInvoke( _cookie, getMethod<co::IMethodBuilder>( 0 ), args, co::Any() );
	}

	void defineException( co::IException* exceptionType_ )
	{
		co::Any args[] = { exceptionType_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IMethodBuilder>( 1 ), args, co::Any() );
	}

	void defineParameter( const std::string& name_, co::IType* type_, bool input_, bool output_ )
	{
		co::Any args[] = { name_, type_, input_, output_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IMethodBuilder>( 2 ), args, co::Any() );
	}

	void defineReturnType( co::IType* type_ )
	{
		co::Any args[] = { type_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IMethodBuilder>( 3 ), args, co::Any() );
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

class IMethodBuilder_Reflector : public co::ReflectorBase
{
public:
	IMethodBuilder_Reflector()
	{
		// empty
	}

	virtual ~IMethodBuilder_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::IMethodBuilder>::get();
	}

	co::uint32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new co::IMethodBuilder_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::IMethodBuilder* p = co::checkInstance<co::IMethodBuilder>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		value.put( p->getMethodName() ); break;
		case 1:		value.put( p->getTypeBuilder() ); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::IMethodBuilder* p = co::checkInstance<co::IMethodBuilder>( instance, field );
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
		co::IMethodBuilder* p = co::checkInstance<co::IMethodBuilder>( instance, method );
		checkNumArguments( method, args.getSize() );
		int argIndex = -1;
		try
		{
			switch( method->getIndex() )
			{
			case 2:
				{
					p->createMethod();
				}
				break;
			case 3:
				{
					co::IException* exceptionType_ = args[++argIndex].get< co::IException* >();
					argIndex = -1;
					p->defineException( exceptionType_ );
				}
				break;
			case 4:
				{
					const std::string& name_ = args[++argIndex].get< const std::string& >();
					co::IType* type_ = args[++argIndex].get< co::IType* >();
					bool input_ = args[++argIndex].get< bool >();
					bool output_ = args[++argIndex].get< bool >();
					argIndex = -1;
					p->defineParameter( name_, type_, input_, output_ );
				}
				break;
			case 5:
				{
					co::IType* type_ = args[++argIndex].get< co::IType* >();
					argIndex = -1;
					p->defineReturnType( type_ );
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

co::IReflector* __createIMethodBuilderReflector()
{
    return new IMethodBuilder_Reflector;
}

} // namespace co
