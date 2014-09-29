
#include <co/NoSuchPortException.h>
#include <co/private/ReflectorBase.h>
#include <memory>

namespace co {

//------ Reflector Component ------//

class NoSuchPortException_Reflector : public co::ReflectorBase
{
public:
	NoSuchPortException_Reflector()
	{
		// empty
	}

	virtual ~NoSuchPortException_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::NoSuchPortException>::get();
	}

	co::int32 getSize()
	{
		return sizeof(co::NoSuchPortException);
	}

	void raise( const std::string& message )
	{
		throw co::NoSuchPortException( message );
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createNoSuchPortExceptionReflector()
{
    return new NoSuchPortException_Reflector;
}

} // namespace co
