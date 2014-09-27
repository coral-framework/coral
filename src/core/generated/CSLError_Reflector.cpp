
#include <co/CSLError.h>
#include <co/IField.h>
#include <co/MissingInputException.h>
#include <co/IllegalArgumentException.h>
#include <co/reserved/ReflectorBase.h>
#include <memory>
#include <sstream>

namespace co {

//------ Reflector Component ------//

class CSLError_Reflector : public co::ReflectorBase
{
public:
	CSLError_Reflector()
	{
		// empty
	}

	virtual ~CSLError_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::CSLError>::get();
	}

	co::int32 getSize()
	{
		return sizeof(co::CSLError);
	}

	void createValues( void* ptr, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
			new( reinterpret_cast<co::CSLError*>( ptr ) + i ) co::CSLError;
    }

	void copyValues( const void* fromPtr, void* toPtr, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
			reinterpret_cast<co::CSLError*>( toPtr )[i] = reinterpret_cast<const co::CSLError*>( fromPtr )[i];
    }

	void destroyValues( void* ptr, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
			callDestructor( reinterpret_cast<co::CSLError*>( ptr ) + i );
	}

	bool compareValues( const void* a, const void* b, size_t numValues )
	{
		for( size_t i = 0; i < numValues; ++i )
			if( !( reinterpret_cast<const co::CSLError*>( a )[i] == reinterpret_cast<const co::CSLError*>( b )[i] ) )
				return false;
		return true;
    }

	void getField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::CSLError* p = co::checkInstance<co::CSLError>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		value.put( p->filename ); break;
		case 1:		value.put( p->line ); break;
		case 2:		value.put( p->message ); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}

	void setField( const co::Any& instance, co::IField* field, const co::Any& value )
	{
		co::CSLError* p = co::checkInstance<co::CSLError>( instance, field );
		switch( field->getIndex() )
		{
		case 0:		p->filename = value.get< const std::string& >(); break;
		case 1:		p->line = value.get< co::int32 >(); break;
		case 2:		p->message = value.get< const std::string& >(); break;
		default:	raiseUnexpectedMemberIndex();
		}
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createCSLErrorReflector()
{
    return new CSLError_Reflector;
}

} // namespace co
