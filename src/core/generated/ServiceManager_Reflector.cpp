
#include <co/reserved/ReflectorBase.h>
#include <memory>

namespace co {

// The following two functions are implemented by CORAL_EXPORT_COMPONENT()
co::int32 __ServiceManager_getSize();
co::IObject* __ServiceManager_newInstance();

//------ Reflector Component ------//

class ServiceManager_Reflector : public co::ReflectorBase
{
public:
	ServiceManager_Reflector()
	{
		// empty
	}

	virtual ~ServiceManager_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::getType( "co.ServiceManager" );
	}

	co::int32 getSize()
	{
		return __ServiceManager_getSize();
	}

	co::IObject* newInstance()
	{
		co::IObject* instance = __ServiceManager_newInstance();
		assert( instance->getComponent()->getFullName() == "co.ServiceManager" );
		return instance;
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createServiceManagerReflector()
{
    return new ServiceManager_Reflector;
}

} // namespace co
