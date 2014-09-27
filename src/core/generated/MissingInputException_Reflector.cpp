
#include <co/MissingInputException.h>
#include <co/reserved/ReflectorBase.h>
#include <memory>

namespace co {

//------ Reflector Component ------//

class MissingInputException_Reflector : public co::ReflectorBase
{
public:
	MissingInputException_Reflector()
	{
		// empty
	}

	virtual ~MissingInputException_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::MissingInputException>::get();
	}

	co::int32 getSize()
	{
		return sizeof(co::MissingInputException);
	}

	void raise( const std::string& message )
	{
		throw co::MissingInputException( message );
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createMissingInputExceptionReflector()
{
    return new MissingInputException_Reflector;
}

} // namespace co
