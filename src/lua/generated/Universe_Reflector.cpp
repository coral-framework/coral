#include <co/reserved/ReflectorBase.h>
#include <memory>

namespace lua {

// The following two functions are implemented by CORAL_EXPORT_COMPONENT()
co::uint32 __Universe_getSize();
co::IObject* __Universe_newInstance();

void moduleRetain();
void moduleRelease();

//------ Reflector Component ------//

class Universe_Reflector : public co::ReflectorBase
{
public:
	Universe_Reflector()
	{
		moduleRetain();
	}

	virtual ~Universe_Reflector()
	{
		moduleRelease();
	}

	co::IType* getType()
	{
		return co::getType( "lua.Universe" );
	}

	co::uint32 getSize()
	{
		return __Universe_getSize();
	}

	co::IObject* newInstance()
	{
		co::IObject* instance = __Universe_newInstance();
		assert( instance->getComponent()->getFullName() == "lua.Universe" );
		return instance;
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createUniverseReflector()
{
    return new Universe_Reflector;
}

} // namespace lua
