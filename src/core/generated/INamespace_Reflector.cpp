#include <co/INamespace.h>
#include <co/IDynamicServiceProvider.h>
#include <co/IModule.h>
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

class INamespace_Proxy : public co::INamespace
{
public:
	INamespace_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~INamespace_Proxy()
	{
		// empty
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<co::INamespace>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// co.INamespace Methods:

	co::TSlice<co::INamespace*> getChildNamespaces()
	{
		std::vector<co::INamespaceRef> res;
		_provider->dynamicGetField( _cookie, getField<co::INamespace>( 0 ), res );
		return co::moveToSlice<co::INamespace*>( res );
	}

	std::string getFullName()
	{
		std::string res;
		_provider->dynamicGetField( _cookie, getField<co::INamespace>( 1 ), res );
		return res;
	}

	co::IModule* getModule()
	{
		co::IModuleRef res;
		_provider->dynamicGetField( _cookie, getField<co::INamespace>( 2 ), res );
		return res.get();
	}

	std::string getName()
	{
		std::string res;
		_provider->dynamicGetField( _cookie, getField<co::INamespace>( 3 ), res );
		return res;
	}

	co::INamespace* getParentNamespace()
	{
		co::INamespaceRef res;
		_provider->dynamicGetField( _cookie, getField<co::INamespace>( 4 ), res );
		return res.get();
	}

	co::TSlice<co::IType*> getTypes()
	{
		std::vector<co::ITypeRef> res;
		_provider->dynamicGetField( _cookie, getField<co::INamespace>( 5 ), res );
		return co::moveToSlice<co::IType*>( res );
	}

	co::INamespace* defineChildNamespace( const std::string& name_ )
	{
		co::Any args[] = { name_ };
		co::INamespaceRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::INamespace>( 0 ), args, res );
		return res.get();
	}

	co::ITypeBuilder* defineType( const std::string& name_, co::TypeKind kind_ )
	{
		co::Any args[] = { name_, kind_ };
		co::ITypeBuilderRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::INamespace>( 1 ), args, res );
		return res.get();
	}

	co::INamespace* findChildNamespace( const std::string& name_ )
	{
		co::Any args[] = { name_ };
		co::INamespaceRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::INamespace>( 2 ), args, res );
		return res.get();
	}

	co::IType* findType( const std::string& name_ )
	{
		co::Any args[] = { name_ };
		co::ITypeRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::INamespace>( 3 ), args, res );
		return res.get();
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

class INamespace_Reflector : public co::ReflectorBase
{
public:
	INamespace_Reflector()
	{
		// empty
	}

	virtual ~INamespace_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::INamespace>::get();
	}

	co::uint32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new co::INamespace_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::INamespace* p = co::checkInstance<co::INamespace>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		value.put( p->getChildNamespaces() ); break;
		case 1:		value.put( p->getFullName() ); break;
		case 2:		value.put( p->getModule() ); break;
		case 3:		value.put( p->getName() ); break;
		case 4:		value.put( p->getParentNamespace() ); break;
		case 5:		value.put( p->getTypes() ); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::INamespace* p = co::checkInstance<co::INamespace>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		raiseFieldIsReadOnly( field ); break;
		case 1:		raiseFieldIsReadOnly( field ); break;
		case 2:		raiseFieldIsReadOnly( field ); break;
		case 3:		raiseFieldIsReadOnly( field ); break;
		case 4:		raiseFieldIsReadOnly( field ); break;
		case 5:		raiseFieldIsReadOnly( field ); break;
		default:	raiseUnexpectedMemberIndex();
		}
		CORAL_UNUSED( p );
		CORAL_UNUSED( value );
	}

	void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& res )
	{
		co::INamespace* p = co::checkInstance<co::INamespace>( instance, method );
		checkNumArguments( method, args.getSize() );
		int argIndex = -1;
		try
		{
			switch( method->getIndex() )
			{
			case 6:
				{
					const std::string& name_ = args[++argIndex].get< const std::string& >();
					argIndex = -1;
					res.put( p->defineChildNamespace( name_ ) );
				}
				break;
			case 7:
				{
					const std::string& name_ = args[++argIndex].get< const std::string& >();
					co::TypeKind kind_ = args[++argIndex].get< co::TypeKind >();
					argIndex = -1;
					res.put( p->defineType( name_, kind_ ) );
				}
				break;
			case 8:
				{
					const std::string& name_ = args[++argIndex].get< const std::string& >();
					argIndex = -1;
					res.put( p->findChildNamespace( name_ ) );
				}
				break;
			case 9:
				{
					const std::string& name_ = args[++argIndex].get< const std::string& >();
					argIndex = -1;
					res.put( p->findType( name_ ) );
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

co::IReflector* __createINamespaceReflector()
{
    return new INamespace_Reflector;
}

} // namespace co
