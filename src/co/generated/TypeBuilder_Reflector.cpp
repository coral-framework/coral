
#include <co/private/ReflectorBase.h>
#include <memory>

namespace co {

// The following two functions are implemented by CORAL_EXPORT_COMPONENT()
co::int32 __TypeBuilder_getSize();
co::IObject* __TypeBuilder_newInstance();

//------ Reflector Component ------//

class TypeBuilder_Reflector : public co::ReflectorBase
{
public:
	TypeBuilder_Reflector()
	{
		// empty
	}

	virtual ~TypeBuilder_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::getType( "co.TypeBuilder" );
	}

	co::int32 getSize()
	{
		return __TypeBuilder_getSize();
	}

	co::IObject* newInstance()
	{
		co::IObject* instance = __TypeBuilder_newInstance();
		assert( instance->getComponent()->getFullName() == "co.TypeBuilder" );
		return instance;
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createTypeBuilderReflector()
{
    return new TypeBuilder_Reflector;
}

} // namespace co
