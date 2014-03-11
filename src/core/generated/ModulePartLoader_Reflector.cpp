#include <co/reserved/ReflectorBase.h>
#include <memory>

namespace co {

// The following two functions are implemented by CORAL_EXPORT_COMPONENT()
co::uint32 __ModulePartLoader_getSize();
co::IObject* __ModulePartLoader_newInstance();

//------ Reflector Component ------//

class ModulePartLoader_Reflector : public co::ReflectorBase
{
public:
	ModulePartLoader_Reflector()
	{
		// empty
	}

	virtual ~ModulePartLoader_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::getType( "co.ModulePartLoader" );
	}

	co::uint32 getSize()
	{
		return __ModulePartLoader_getSize();
	}

	co::IObject* newInstance()
	{
		co::IObject* instance = __ModulePartLoader_newInstance();
		assert( instance->getComponent()->getFullName() == "co.ModulePartLoader" );
		return instance;
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createModulePartLoaderReflector()
{
    return new ModulePartLoader_Reflector;
}

} // namespace co
