#include <co/reserved/ReflectorBase.h>
#include <memory>

namespace co {

// The following two functions are implemented by CORAL_EXPORT_COMPONENT()
co::uint32 __ArrayType_getSize();
co::IObject* __ArrayType_newInstance();

//------ Reflector Component ------//

class ArrayType_Reflector : public co::ReflectorBase
{
public:
	ArrayType_Reflector()
	{
		// empty
	}

	virtual ~ArrayType_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::getType( "co.ArrayType" );
	}

	co::uint32 getSize()
	{
		return __ArrayType_getSize();
	}

	co::IObject* newInstance()
	{
		co::IObject* instance = __ArrayType_newInstance();
		assert( instance->getComponent()->getFullName() == "co.ArrayType" );
		return instance;
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createArrayTypeReflector()
{
    return new ArrayType_Reflector;
}

} // namespace co
