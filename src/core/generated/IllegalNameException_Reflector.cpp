
#include <co/IllegalNameException.h>
#include <co/reserved/ReflectorBase.h>
#include <memory>

namespace co {

//------ Reflector Component ------//

class IllegalNameException_Reflector : public co::ReflectorBase
{
public:
	IllegalNameException_Reflector()
	{
		// empty
	}

	virtual ~IllegalNameException_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::IllegalNameException>::get();
	}

	co::int32 getSize()
	{
		return sizeof(co::IllegalNameException);
	}

	void raise( const std::string& message )
	{
		throw co::IllegalNameException( message );
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createIllegalNameExceptionReflector()
{
    return new IllegalNameException_Reflector;
}

} // namespace co
