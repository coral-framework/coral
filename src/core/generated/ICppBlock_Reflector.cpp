#include <co/ICppBlock.h>
#include <co/IDynamicServiceProvider.h>
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

class ICppBlock_Proxy : public co::ICppBlock
{
public:
	ICppBlock_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~ICppBlock_Proxy()
	{
		// empty
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<co::ICppBlock>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// co.IAnnotation Methods:

	// co.ICppBlock Methods:

	std::string getValue()
	{
		std::string res;
		_provider->dynamicGetField( _cookie, getField<co::ICppBlock>( 0 ), res );
		return res;
	}

	void setValue( const std::string& value_ )
	{
		_provider->dynamicSetField( _cookie, getField<co::ICppBlock>( 0 ), value_ );
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

class ICppBlock_Reflector : public co::ReflectorBase
{
public:
	ICppBlock_Reflector()
	{
		// empty
	}

	virtual ~ICppBlock_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::ICppBlock>::get();
	}

	co::uint32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new co::ICppBlock_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::ICppBlock* p = co::checkInstance<co::ICppBlock>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		value.put( p->getValue() ); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::ICppBlock* p = co::checkInstance<co::ICppBlock>( instance, field );
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
		co::checkInstance<co::ICppBlock>( instance, method );
		raiseUnexpectedMemberIndex();
		CORAL_UNUSED( args );
		CORAL_UNUSED( res );
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createICppBlockReflector()
{
    return new ICppBlock_Reflector;
}

} // namespace co
