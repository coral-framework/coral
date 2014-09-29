
#include <lua/IScripted.h>
#include <co/IDynamicServiceProvider.h>
#include <co/IType.h>
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

class IScripted_Proxy : public lua::IScripted
{
public:
	IScripted_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		moduleRetain();
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~IScripted_Proxy()
	{
		moduleRelease();
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<lua::IScripted>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// co.IAnnotation Methods:

	// co.IDynamicTypeProvider Methods:

	void provideReflectorFor( co::IType* type_ )
	{
		co::Any args[] = { type_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IDynamicTypeProvider>( 0 ), args, co::Any() );
	}

	// lua.IScripted Methods:

	std::string getValue()
	{
		std::string res;
		_provider->dynamicGetField( _cookie, getField<lua::IScripted>( 0 ), res );
		return res;
	}

	void setValue( const std::string& value_ )
	{
		_provider->dynamicSetField( _cookie, getField<lua::IScripted>( 0 ), value_ );
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

class IScripted_Reflector : public co::ReflectorBase
{
public:
	IScripted_Reflector()
	{
		moduleRetain();
	}

	virtual ~IScripted_Reflector()
	{
		moduleRelease();
	}

	co::IType* getType()
	{
		return co::typeOf<lua::IScripted>::get();
	}

	co::int32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new lua::IScripted_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		lua::IScripted* p = co::checkInstance<lua::IScripted>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		value.put( p->getValue() ); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		lua::IScripted* p = co::checkInstance<lua::IScripted>( instance, field );
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
		co::checkInstance<lua::IScripted>( instance, method );
		raiseUnexpectedMemberIndex();
		CORAL_UNUSED( args );
		CORAL_UNUSED( res );
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createIScriptedReflector()
{
    return new IScripted_Reflector;
}

} // namespace lua
