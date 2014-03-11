#include <co/IParameter.h>
#include <co/IDynamicServiceProvider.h>
#include <co/IType.h>
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

class IParameter_Proxy : public co::IParameter
{
public:
	IParameter_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~IParameter_Proxy()
	{
		// empty
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<co::IParameter>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// co.IParameter Methods:

	bool getIsIn()
	{
		bool res;
		_provider->dynamicGetField( _cookie, getField<co::IParameter>( 0 ), res );
		return res;
	}

	bool getIsOut()
	{
		bool res;
		_provider->dynamicGetField( _cookie, getField<co::IParameter>( 1 ), res );
		return res;
	}

	std::string getName()
	{
		std::string res;
		_provider->dynamicGetField( _cookie, getField<co::IParameter>( 2 ), res );
		return res;
	}

	co::IType* getType()
	{
		co::ITypeRef res;
		_provider->dynamicGetField( _cookie, getField<co::IParameter>( 3 ), res );
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

class IParameter_Reflector : public co::ReflectorBase
{
public:
	IParameter_Reflector()
	{
		// empty
	}

	virtual ~IParameter_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::IParameter>::get();
	}

	co::uint32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new co::IParameter_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::IParameter* p = co::checkInstance<co::IParameter>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		value.put( p->getIsIn() ); break;
		case 1:		value.put( p->getIsOut() ); break;
		case 2:		value.put( p->getName() ); break;
		case 3:		value.put( p->getType() ); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::IParameter* p = co::checkInstance<co::IParameter>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		raiseFieldIsReadOnly( field ); break;
		case 1:		raiseFieldIsReadOnly( field ); break;
		case 2:		raiseFieldIsReadOnly( field ); break;
		case 3:		raiseFieldIsReadOnly( field ); break;
		default:	raiseUnexpectedMemberIndex();
		}
		CORAL_UNUSED( p );
		CORAL_UNUSED( value );
	}

	void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& res )
	{
		co::checkInstance<co::IParameter>( instance, method );
		raiseUnexpectedMemberIndex();
		CORAL_UNUSED( args );
		CORAL_UNUSED( res );
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createIParameterReflector()
{
    return new IParameter_Reflector;
}

} // namespace co
