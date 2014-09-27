
#include <co/ITypeBuilder.h>
#include <co/IDynamicServiceProvider.h>
#include <co/IType.h>
#include <co/IMethodBuilder.h>
#include <co/IInterface.h>
#include <co/INamespace.h>
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

class ITypeBuilder_Proxy : public co::ITypeBuilder
{
public:
	ITypeBuilder_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~ITypeBuilder_Proxy()
	{
		// empty
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<co::ITypeBuilder>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// co.ITypeBuilder Methods:

	co::TypeKind getKind()
	{
		co::TypeKind res;
		_provider->dynamicGetField( _cookie, getField<co::ITypeBuilder>( 0 ), res );
		return res;
	}

	co::INamespace* getNamespace()
	{
		co::INamespaceRef res;
		_provider->dynamicGetField( _cookie, getField<co::ITypeBuilder>( 1 ), res );
		return res.get();
	}

	std::string getTypeName()
	{
		std::string res;
		_provider->dynamicGetField( _cookie, getField<co::ITypeBuilder>( 2 ), res );
		return res;
	}

	co::IType* createType()
	{
		co::Slice<co::Any> args;
		co::ITypeRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::ITypeBuilder>( 0 ), args, res );
		return res.get();
	}

	void defineBaseType( co::IType* baseType_ )
	{
		co::Any args[] = { baseType_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::ITypeBuilder>( 1 ), args, co::Any() );
	}

	void defineField( const std::string& name_, co::IType* type_, bool isReadOnly_ )
	{
		co::Any args[] = { name_, type_, isReadOnly_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::ITypeBuilder>( 2 ), args, co::Any() );
	}

	void defineIdentifier( const std::string& name_ )
	{
		co::Any args[] = { name_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::ITypeBuilder>( 3 ), args, co::Any() );
	}

	co::IMethodBuilder* defineMethod( const std::string& name_ )
	{
		co::Any args[] = { name_ };
		co::IMethodBuilderRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::ITypeBuilder>( 4 ), args, res );
		return res.get();
	}

	void definePort( const std::string& name_, co::IInterface* itf_, bool isFacet_ )
	{
		co::Any args[] = { name_, itf_, isFacet_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::ITypeBuilder>( 5 ), args, co::Any() );
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

class ITypeBuilder_Reflector : public co::ReflectorBase
{
public:
	ITypeBuilder_Reflector()
	{
		// empty
	}

	virtual ~ITypeBuilder_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::ITypeBuilder>::get();
	}

	co::int32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new co::ITypeBuilder_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::ITypeBuilder* p = co::checkInstance<co::ITypeBuilder>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		value.put( p->getKind() ); break;
		case 1:		value.put( p->getNamespace() ); break;
		case 2:		value.put( p->getTypeName() ); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::ITypeBuilder* p = co::checkInstance<co::ITypeBuilder>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		raiseFieldIsReadOnly( field ); break;
		case 1:		raiseFieldIsReadOnly( field ); break;
		case 2:		raiseFieldIsReadOnly( field ); break;
		default:	raiseUnexpectedMemberIndex();
		}
		CORAL_UNUSED( p );
		CORAL_UNUSED( value );
	}

	void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& res )
	{
		co::ITypeBuilder* p = co::checkInstance<co::ITypeBuilder>( instance, method );
		checkNumArguments( method, args.getSize() );
		int argIndex = -1;
		try
		{
			switch( method->getIndex() )
			{
			case 3:
				{
					res.put( p->createType() );
				}
				break;
			case 4:
				{
					co::IType* baseType_ = args[++argIndex].get< co::IType* >();
					argIndex = -1;
					p->defineBaseType( baseType_ );
				}
				break;
			case 5:
				{
					const std::string& name_ = args[++argIndex].get< const std::string& >();
					co::IType* type_ = args[++argIndex].get< co::IType* >();
					bool isReadOnly_ = args[++argIndex].get< bool >();
					argIndex = -1;
					p->defineField( name_, type_, isReadOnly_ );
				}
				break;
			case 6:
				{
					const std::string& name_ = args[++argIndex].get< const std::string& >();
					argIndex = -1;
					p->defineIdentifier( name_ );
				}
				break;
			case 7:
				{
					const std::string& name_ = args[++argIndex].get< const std::string& >();
					argIndex = -1;
					res.put( p->defineMethod( name_ ) );
				}
				break;
			case 8:
				{
					const std::string& name_ = args[++argIndex].get< const std::string& >();
					co::IInterface* itf_ = args[++argIndex].get< co::IInterface* >();
					bool isFacet_ = args[++argIndex].get< bool >();
					argIndex = -1;
					p->definePort( name_, itf_, isFacet_ );
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

co::IReflector* __createITypeBuilderReflector()
{
    return new ITypeBuilder_Reflector;
}

} // namespace co
