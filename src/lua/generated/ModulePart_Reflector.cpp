#include <co/reserved/ReflectorBase.h>
#include <memory>

namespace lua {

// The following two functions are implemented by CORAL_EXPORT_COMPONENT()
co::uint32 __ModulePart_getSize();
co::IObject* __ModulePart_newInstance();

void moduleRetain();
void moduleRelease();

//------ Reflector Component ------//

class ModulePart_Reflector : public co::ReflectorBase
{
public:
	ModulePart_Reflector()
	{
		moduleRetain();
	}

	virtual ~ModulePart_Reflector()
	{
		moduleRelease();
	}

	co::IType* getType()
	{
		return co::getType( "lua.ModulePart" );
	}

	co::uint32 getSize()
	{
		return __ModulePart_getSize();
	}

	co::IObject* newInstance()
	{
		co::IObject* instance = __ModulePart_newInstance();
		assert( instance->getComponent()->getFullName() == "lua.ModulePart" );
		return instance;
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createModulePartReflector()
{
    return new ModulePart_Reflector;
}

} // namespace lua
