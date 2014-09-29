
#include <co/private/ReflectorBase.h>
#include <memory>

namespace co {

// The following two functions are implemented by CORAL_EXPORT_COMPONENT()
co::int32 __TypeTransaction_getSize();
co::IObject* __TypeTransaction_newInstance();

//------ Reflector Component ------//

class TypeTransaction_Reflector : public co::ReflectorBase
{
public:
	TypeTransaction_Reflector()
	{
		// empty
	}

	virtual ~TypeTransaction_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::getType( "co.TypeTransaction" );
	}

	co::int32 getSize()
	{
		return __TypeTransaction_getSize();
	}

	co::IObject* newInstance()
	{
		co::IObject* instance = __TypeTransaction_newInstance();
		assert( instance->getComponent()->getFullName() == "co.TypeTransaction" );
		return instance;
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createTypeTransactionReflector()
{
    return new TypeTransaction_Reflector;
}

} // namespace co
