#include <co/reserved/ReflectorBase.h>
#include <memory>

namespace co {

// The following two functions are implemented by CORAL_EXPORT_COMPONENT()
co::uint32 __MethodBuilder_getSize();
co::IObject* __MethodBuilder_newInstance();

//------ Reflector Component ------//

class MethodBuilder_Reflector : public co::ReflectorBase
{
public:
	MethodBuilder_Reflector()
	{
		// empty
	}

	virtual ~MethodBuilder_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::getType( "co.MethodBuilder" );
	}

	co::uint32 getSize()
	{
		return __MethodBuilder_getSize();
	}

	co::IObject* newInstance()
	{
		co::IObject* instance = __MethodBuilder_newInstance();
		assert( instance->getComponent()->getFullName() == "co.MethodBuilder" );
		return instance;
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createMethodBuilderReflector()
{
    return new MethodBuilder_Reflector;
}

} // namespace co
