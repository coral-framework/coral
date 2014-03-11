#include <co/reserved/ReflectorBase.h>
#include <memory>

namespace co {

// The following two functions are implemented by CORAL_EXPORT_COMPONENT()
co::uint32 __Component_getSize();
co::IObject* __Component_newInstance();

//------ Reflector Component ------//

class Component_Reflector : public co::ReflectorBase
{
public:
	Component_Reflector()
	{
		// empty
	}

	virtual ~Component_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::getType( "co.Component" );
	}

	co::uint32 getSize()
	{
		return __Component_getSize();
	}

	co::IObject* newInstance()
	{
		co::IObject* instance = __Component_newInstance();
		assert( instance->getComponent()->getFullName() == "co.Component" );
		return instance;
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createComponentReflector()
{
    return new Component_Reflector;
}

} // namespace co
