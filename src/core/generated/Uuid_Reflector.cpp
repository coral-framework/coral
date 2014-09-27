
#include "Uuid_Adapter.h"
#include <co/IMethod.h>
#include <co/IField.h>
#include <co/IllegalCastException.h>
#include <co/MissingInputException.h>
#include <co/IllegalArgumentException.h>
#include <co/reserved/ReflectorBase.h>
#include <memory>
#include <sstream>

namespace co {

//------ Reflector Component ------//

class Uuid_Reflector : public co::ReflectorBase
{
public:
	Uuid_Reflector()
	{
		// empty
	}

	virtual ~Uuid_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::Uuid>::get();
	}

	co::int32 getSize()
	{
		return sizeof(co::Uuid);
	}

	void createValues( void* ptr, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
			new( reinterpret_cast<co::Uuid*>( ptr ) + i ) co::Uuid;
    }

	void copyValues( const void* fromPtr, void* toPtr, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
			reinterpret_cast<co::Uuid*>( toPtr )[i] = reinterpret_cast<const co::Uuid*>( fromPtr )[i];
    }

	void destroyValues( void* ptr, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
			callDestructor( reinterpret_cast<co::Uuid*>( ptr ) + i );
	}

	bool compareValues( const void* a, const void* b, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
			if( !( reinterpret_cast<const co::Uuid*>( a )[i] == reinterpret_cast<const co::Uuid*>( b )[i] ) )
				return false;
		return true;
    }

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::Uuid* p = co::checkInstance<co::Uuid>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		value.put( co::Uuid_Adapter::getIsNull( *p ) ); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::Uuid* p = co::checkInstance<co::Uuid>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		raiseFieldIsReadOnly( field ); break;
		default:	raiseUnexpectedMemberIndex();
		}
		CORAL_UNUSED( p );
		CORAL_UNUSED( value );
	}

	void invoke( const co::Any& instance, co::IMethod* method, co::Slice<co::Any> args, const co::Any& res )
	{
		co::Uuid* p = co::checkInstance<co::Uuid>( instance, method );
		checkNumArguments( method, args.getSize() );
		int argIndex = -1;
		try
		{
			switch( method->getIndex() )
			{
			case 1:
				{
					co::Uuid_Adapter::clear( *p );
				}
				break;
			case 2:
				{
					co::Uuid_Adapter::createRandom( *p );
				}
				break;
			case 3:
				{
					const std::string& data_ = args[++argIndex].get< const std::string& >();
					argIndex = -1;
					co::Uuid_Adapter::createSha1( *p, data_ );
				}
				break;
			case 4:
				{
					std::string& str_ = args[++argIndex].get< std::string& >();
					argIndex = -1;
					co::Uuid_Adapter::getString( *p, str_ );
				}
				break;
			case 5:
				{
					const std::string& str_ = args[++argIndex].get< const std::string& >();
					argIndex = -1;
					co::Uuid_Adapter::setString( *p, str_ );
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

co::IReflector* __createUuidReflector()
{
    return new Uuid_Reflector;
}

} // namespace co
