
#include <co/MissingServiceException.h>
#include <co/reserved/ReflectorBase.h>
#include <memory>

namespace co {

//------ Reflector Component ------//

class MissingServiceException_Reflector : public co::ReflectorBase
{
public:
	MissingServiceException_Reflector()
	{
		// empty
	}

	virtual ~MissingServiceException_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::MissingServiceException>::get();
	}

	co::int32 getSize()
	{
		return sizeof(co::MissingServiceException);
	}

	void raise( const std::string& message )
	{
		throw co::MissingServiceException( message );
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createMissingServiceExceptionReflector()
{
    return new MissingServiceException_Reflector;
}

} // namespace co
