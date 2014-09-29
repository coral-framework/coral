
#include <co/IllegalArgumentException.h>
#include <co/private/ReflectorBase.h>
#include <memory>

namespace co {

//------ Reflector Component ------//

class IllegalArgumentException_Reflector : public co::ReflectorBase
{
public:
	IllegalArgumentException_Reflector()
	{
		// empty
	}

	virtual ~IllegalArgumentException_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::IllegalArgumentException>::get();
	}

	co::int32 getSize()
	{
		return sizeof(co::IllegalArgumentException);
	}

	void raise( const std::string& message )
	{
		throw co::IllegalArgumentException( message );
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createIllegalArgumentExceptionReflector()
{
    return new IllegalArgumentException_Reflector;
}

} // namespace co
