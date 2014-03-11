#include <co/reserved/ReflectorBase.h>
#include <memory>

namespace co {

// The following two functions are implemented by CORAL_EXPORT_COMPONENT()
co::uint32 __Namespace_getSize();
co::IObject* __Namespace_newInstance();

//------ Reflector Component ------//

class Namespace_Reflector : public co::ReflectorBase
{
public:
	Namespace_Reflector()
	{
		// empty
	}

	virtual ~Namespace_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::getType( "co.Namespace" );
	}

	co::uint32 getSize()
	{
		return __Namespace_getSize();
	}

	co::IObject* newInstance()
	{
		co::IObject* instance = __Namespace_newInstance();
		assert( instance->getComponent()->getFullName() == "co.Namespace" );
		return instance;
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createNamespaceReflector()
{
    return new Namespace_Reflector;
}

} // namespace co
