
#include <co/IComponent.h>
#include <co/IDynamicServiceProvider.h>
#include <co/Uuid.h>
#include <co/IInterface.h>
#include <co/IAnnotation.h>
#include <co/IType.h>
#include <co/IPort.h>
#include <co/IMember.h>
#include <co/IReflector.h>
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

class IComponent_Proxy : public co::IComponent
{
public:
	IComponent_Proxy( co::IDynamicServiceProvider* provider ) : _provider( provider )
	{
		_cookie = _provider->dynamicRegisterService( this );
	}

	virtual ~IComponent_Proxy()
	{
		// empty
	}

	// co::IService Methods:

	co::IInterface* getInterface() { return co::typeOf<co::IComponent>::get(); }
	co::IObject* getProvider() { return _provider->getProvider(); }
	co::IPort* getFacet() { return _provider->dynamicGetFacet( _cookie ); }
	void serviceRetain() { _provider->serviceRetain(); }
	void serviceRelease() { _provider->serviceRelease(); }

	// co.IAnnotated Methods:

	co::TSlice<co::IAnnotation*> getAnnotations()
	{
		std::vector<co::IAnnotationRef> res;
		_provider->dynamicGetField( _cookie, getField<co::IAnnotated>( 0 ), res );
		return co::moveToSlice<co::IAnnotation*>( res );
	}

	void setAnnotations( co::Slice<co::IAnnotation*> annotations_ )
	{
		_provider->dynamicSetField( _cookie, getField<co::IAnnotated>( 0 ), annotations_ );
	}

	void addAnnotation( co::IAnnotation* annotation_ )
	{
		co::Any args[] = { annotation_ };
		_provider->dynamicInvoke( _cookie, getMethod<co::IAnnotated>( 0 ), args, co::Any() );
	}

	co::IAnnotation* findAnnotation( co::IInterface* annotationType_ )
	{
		co::Any args[] = { annotationType_ };
		co::IAnnotationRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::IAnnotated>( 1 ), args, res );
		return res.get();
	}

	// co.IType Methods:

	co::Uuid getBinarySignature()
	{
		co::Uuid res;
		_provider->dynamicGetField( _cookie, getField<co::IType>( 0 ), res );
		return res;
	}

	co::IReflector* getCurrentReflector()
	{
		co::IReflectorRef res;
		_provider->dynamicGetField( _cookie, getField<co::IType>( 1 ), res );
		return res.get();
	}

	std::string getFullName()
	{
		std::string res;
		_provider->dynamicGetField( _cookie, getField<co::IType>( 2 ), res );
		return res;
	}

	co::Uuid getFullSignature()
	{
		co::Uuid res;
		_provider->dynamicGetField( _cookie, getField<co::IType>( 3 ), res );
		return res;
	}

	co::TypeKind getKind()
	{
		co::TypeKind res;
		_provider->dynamicGetField( _cookie, getField<co::IType>( 4 ), res );
		return res;
	}

	std::string getName()
	{
		std::string res;
		_provider->dynamicGetField( _cookie, getField<co::IType>( 5 ), res );
		return res;
	}

	co::INamespace* getNamespace()
	{
		co::INamespaceRef res;
		_provider->dynamicGetField( _cookie, getField<co::IType>( 6 ), res );
		return res.get();
	}

	co::IReflector* getReflector()
	{
		co::IReflectorRef res;
		_provider->dynamicGetField( _cookie, getField<co::IType>( 7 ), res );
		return res.get();
	}

	void setReflector( co::IReflector* reflector_ )
	{
		_provider->dynamicSetField( _cookie, getField<co::IType>( 7 ), reflector_ );
	}

	bool isA( co::IType* super_ )
	{
		co::Any args[] = { super_ };
		bool res;
		_provider->dynamicInvoke( _cookie, getMethod<co::IType>( 0 ), args, res );
		return res;
	}

	// co.ICompositeType Methods:

	co::TSlice<co::IMember*> getMembers()
	{
		std::vector<co::IMemberRef> res;
		_provider->dynamicGetField( _cookie, getField<co::ICompositeType>( 0 ), res );
		return co::moveToSlice<co::IMember*>( res );
	}

	co::IMember* getMember( const std::string& name_ )
	{
		co::Any args[] = { name_ };
		co::IMemberRef res;
		_provider->dynamicInvoke( _cookie, getMethod<co::ICompositeType>( 0 ), args, res );
		return res.get();
	}

	// co.IComponent Methods:

	co::TSlice<co::IPort*> getFacets()
	{
		std::vector<co::IPortRef> res;
		_provider->dynamicGetField( _cookie, getField<co::IComponent>( 0 ), res );
		return co::moveToSlice<co::IPort*>( res );
	}

	co::TSlice<co::IPort*> getPorts()
	{
		std::vector<co::IPortRef> res;
		_provider->dynamicGetField( _cookie, getField<co::IComponent>( 1 ), res );
		return co::moveToSlice<co::IPort*>( res );
	}

	co::TSlice<co::IPort*> getReceptacles()
	{
		std::vector<co::IPortRef> res;
		_provider->dynamicGetField( _cookie, getField<co::IComponent>( 2 ), res );
		return co::moveToSlice<co::IPort*>( res );
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

class IComponent_Reflector : public co::ReflectorBase
{
public:
	IComponent_Reflector()
	{
		// empty
	}

	virtual ~IComponent_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::IComponent>::get();
	}

	co::int32 getSize()
	{
		return sizeof(void*);
	}

	co::IService* newDynamicProxy( co::IDynamicServiceProvider* provider )
	{
		checkValidDynamicProvider( provider );
		return new co::IComponent_Proxy( provider );
	}

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::IComponent* p = co::checkInstance<co::IComponent>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		value.put( p->getFacets() ); break;
		case 1:		value.put( p->getPorts() ); break;
		case 2:		value.put( p->getReceptacles() ); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::IComponent* p = co::checkInstance<co::IComponent>( instance, field );
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
		co::checkInstance<co::IComponent>( instance, method );
		raiseUnexpectedMemberIndex();
		CORAL_UNUSED( args );
		CORAL_UNUSED( res );
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createIComponentReflector()
{
    return new IComponent_Reflector;
}

} // namespace co
