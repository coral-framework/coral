
#include <co/reserved/ReflectorBase.h>
#include <memory>

namespace co {

// The following two functions are implemented by CORAL_EXPORT_COMPONENT()
co::int32 __Enum_getSize();
co::IObject* __Enum_newInstance();

//------ Reflector Component ------//

class Enum_Reflector : public co::ReflectorBase
{
public:
	Enum_Reflector()
	{
		// empty
	}

	virtual ~Enum_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::getType( "co.Enum" );
	}

	co::int32 getSize()
	{
		return __Enum_getSize();
	}

	co::IObject* newInstance()
	{
		co::IObject* instance = __Enum_newInstance();
		assert( instance->getComponent()->getFullName() == "co.Enum" );
		return instance;
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createEnumReflector()
{
    return new Enum_Reflector;
}

} // namespace co
