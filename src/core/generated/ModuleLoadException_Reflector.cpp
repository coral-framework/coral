#include <co/ModuleLoadException.h>
#include <co/reserved/ReflectorBase.h>
#include <memory>

namespace co {

//------ Reflector Component ------//

class ModuleLoadException_Reflector : public co::ReflectorBase
{
public:
	ModuleLoadException_Reflector()
	{
		// empty
	}

	virtual ~ModuleLoadException_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::typeOf<co::ModuleLoadException>::get();
	}

	co::uint32 getSize()
	{
		return sizeof(co::ModuleLoadException);
	}

	void raise( const std::string& message )
	{
		throw co::ModuleLoadException( message );
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createModuleLoadExceptionReflector()
{
    return new ModuleLoadException_Reflector;
}

} // namespace co
