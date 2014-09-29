
#include <co/private/ReflectorBase.h>
#include <memory>

namespace lua {

// The following two functions are implemented by CORAL_EXPORT_COMPONENT()
co::int32 __lua_getSize();
co::IObject* __lua_newInstance();

void moduleRetain();
void moduleRelease();

//------ Reflector Component ------//

class lua_Reflector : public co::ReflectorBase
{
public:
	lua_Reflector()
	{
		moduleRetain();
	}

	virtual ~lua_Reflector()
	{
		moduleRelease();
	}

	co::IType* getType()
	{
		return co::getType( "lua.lua" );
	}

	co::int32 getSize()
	{
		return __lua_getSize();
	}

	co::IObject* newInstance()
	{
		co::IObject* instance = __lua_newInstance();
		assert( instance->getComponent()->getFullName() == "lua.lua" );
		return instance;
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createluaReflector()
{
    return new lua_Reflector;
}

} // namespace lua
