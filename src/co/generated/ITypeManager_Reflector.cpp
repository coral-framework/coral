
#include <co/ITypeManager.h>
#include <co/IDynamicServiceProvider.h>
#include <co/CSLError.h>
#include <co/IType.h>
#include <co/INamespace.h>
#include <co/IArray.h>
#include <co/ITypeTransaction.h>
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

class ITypeManager_Proxy : public co::ITypeManager
{
public:
	ITypeManager_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~ITypeManager_Proxy()
	{
		// empty
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<co::ITypeManager>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// co.ITypeManager Methods:

	co::INamespace* getRootNS()
	{
		co::INamespaceRef res;
		_provider->dynamicGetField( _cookie, getField<co::ITypeManager>( 0 ), res );
		return res.get();
	}

	co::ITypeTransaction* getTransaction()
	{
		co::ITypeTransactionRef res;
		_provider->dynamicGetField( _cookie, getField<co::ITypeManager>( 1 ), res );
		return res.get();
	}

	co::INamespace* findNamespace( const std::string& fullName_ )
	{
		co::Any args[] = { fullName_ };
		co::INamespaceRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::ITypeManager>( 0 ), args, res );
		return res.get();
	}

	co::IType* findType( const std::string& fullName_ )
	{
		co::Any args[] = { fullName_ };
		co::ITypeRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::ITypeManager>( 1 ), args, res );
		return res.get();
	}

	co::IArray* getArrayOf( co::IType* elementType_ )
	{
		co::Any args[] = { elementType_ };
		co::IArrayRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::ITypeManager>( 2 ), args, res );
		return res.get();
	}

	co::INamespace* getNamespace( const std::string& fullName_ )
	{
		co::Any args[] = { fullName_ };
		co::INamespaceRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::ITypeManager>( 3 ), args, res );
		return res.get();
	}

	co::IType* getType( const std::string& fullName_ )
	{
		co::Any args[] = { fullName_ };
		co::ITypeRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::ITypeManager>( 4 ), args, res );
		return res.get();
	}

	co::IType* loadType( const std::string& typeName_, std::vector<co::CSLError>& errorStack_ )
	{
		co::Any args[] = { typeName_, errorStack_ };
		co::ITypeRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::ITypeManager>( 5 ), args, res );
		return res.get();
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

class ITypeManager_Reflector : public co::ReflectorBase
{
public:
	ITypeManager_Reflector()
	{
		// empty
	}

	virtual ~ITypeManager_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::ITypeManager>::get();
	}

	co::int32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new co::ITypeManager_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::ITypeManager* p = co::checkInstance<co::ITypeManager>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		value.put( p->getRootNS() ); break;
		case 1:		value.put( p->getTransaction() ); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::ITypeManager* p = co::checkInstance<co::ITypeManager>( instance, field );
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
		co::ITypeManager* p = co::checkInstance<co::ITypeManager>( instance, method );
		checkNumArguments( method, args.getSize() );
		int argIndex = -1;
		try
		{
			switch( method->getIndex() )
			{
			case 2:
				{
					const std::string& fullName_ = args[++argIndex].get< const std::string& >();
					argIndex = -1;
					res.put( p->findNamespace( fullName_ ) );
				}
				break;
			case 3:
				{
					const std::string& fullName_ = args[++argIndex].get< const std::string& >();
					argIndex = -1;
					res.put( p->findType( fullName_ ) );
				}
				break;
			case 4:
				{
					co::IType* elementType_ = args[++argIndex].get< co::IType* >();
					argIndex = -1;
					res.put( p->getArrayOf( elementType_ ) );
				}
				break;
			case 5:
				{
					const std::string& fullName_ = args[++argIndex].get< const std::string& >();
					argIndex = -1;
					res.put( p->getNamespace( fullName_ ) );
				}
				break;
			case 6:
				{
					const std::string& fullName_ = args[++argIndex].get< const std::string& >();
					argIndex = -1;
					res.put( p->getType( fullName_ ) );
				}
				break;
			case 7:
				{
					const std::string& typeName_ = args[++argIndex].get< const std::string& >();
					std::vector<co::CSLError>& errorStack_ = args[++argIndex].get< std::vector<co::CSLError>& >();
					argIndex = -1;
					res.put( p->loadType( typeName_, errorStack_ ) );
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

co::IReflector* __createITypeManagerReflector()
{
    return new ITypeManager_Reflector;
}

} // namespace co
