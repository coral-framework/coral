#include <co/IllegalStateException.h>
#include <co/reserved/ReflectorBase.h>
#include <memory>

namespace co {

//------ Reflector Component ------//

class IllegalStateException_Reflector : public co::ReflectorBase
{
public:
	IllegalStateException_Reflector()
	{
		// empty
	}

	virtual ~IllegalStateException_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::IllegalStateException>::get();
	}

	co::uint32 getSize()
	{
		return sizeof(co::IllegalStateException);
	}

	void raise( const std::string& message )
	{
		throw co::IllegalStateException( message );
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createIllegalStateExceptionReflector()
{
    return new IllegalStateException_Reflector;
}

} // namespace co
