
#include <co/private/ReflectorBase.h>
#include <memory>

namespace co {

// The following two functions are implemented by CORAL_EXPORT_COMPONENT()
co::int32 __Method_getSize();
co::IObject* __Method_newInstance();

//------ Reflector Component ------//

class Method_Reflector : public co::ReflectorBase
{
public:
	Method_Reflector()
	{
		// empty
	}

	virtual ~Method_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::getType( "co.Method" );
	}

	co::int32 getSize()
	{
		return __Method_getSize();
	}

	co::IObject* newInstance()
	{
		co::IObject* instance = __Method_newInstance();
		assert( instance->getComponent()->getFullName() == "co.Method" );
		return instance;
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createMethodReflector()
{
    return new Method_Reflector;
}

} // namespace co
