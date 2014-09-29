
#include <co/private/ReflectorBase.h>
#include <memory>

namespace lua {

// The following two functions are implemented by CORAL_EXPORT_COMPONENT()
co::int32 __Launcher_getSize();
co::IObject* __Launcher_newInstance();

void moduleRetain();
void moduleRelease();

//------ Reflector Component ------//

class Launcher_Reflector : public co::ReflectorBase
{
public:
	Launcher_Reflector()
	{
		moduleRetain();
	}

	virtual ~Launcher_Reflector()
	{
		moduleRelease();
	}

	co::IType* getType()
	{
		return co::getType( "lua.Launcher" );
	}

	co::int32 getSize()
	{
		return __Launcher_getSize();
	}

	co::IObject* newInstance()
	{
		co::IObject* instance = __Launcher_newInstance();
		assert( instance->getComponent()->getFullName() == "lua.Launcher" );
		return instance;
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createLauncherReflector()
{
    return new Launcher_Reflector;
}

} // namespace lua
