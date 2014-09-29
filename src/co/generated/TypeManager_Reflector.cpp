
#include <co/private/ReflectorBase.h>
#include <memory>

namespace co {

// The following two functions are implemented by CORAL_EXPORT_COMPONENT()
co::int32 __TypeManager_getSize();
co::IObject* __TypeManager_newInstance();

//------ Reflector Component ------//

class TypeManager_Reflector : public co::ReflectorBase
{
public:
	TypeManager_Reflector()
	{
		// empty
	}

	virtual ~TypeManager_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::getType( "co.TypeManager" );
	}

	co::int32 getSize()
	{
		return __TypeManager_getSize();
	}

	co::IObject* newInstance()
	{
		co::IObject* instance = __TypeManager_newInstance();
		assert( instance->getComponent()->getFullName() == "co.TypeManager" );
		return instance;
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createTypeManagerReflector()
{
    return new TypeManager_Reflector;
}

} // namespace co
