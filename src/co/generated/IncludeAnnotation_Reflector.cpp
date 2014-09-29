
#include <co/private/ReflectorBase.h>
#include <memory>

namespace co {

// The following two functions are implemented by CORAL_EXPORT_COMPONENT()
co::int32 __IncludeAnnotation_getSize();
co::IObject* __IncludeAnnotation_newInstance();

//------ Reflector Component ------//

class IncludeAnnotation_Reflector : public co::ReflectorBase
{
public:
	IncludeAnnotation_Reflector()
	{
		// empty
	}

	virtual ~IncludeAnnotation_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::getType( "co.IncludeAnnotation" );
	}

	co::int32 getSize()
	{
		return __IncludeAnnotation_getSize();
	}

	co::IObject* newInstance()
	{
		co::IObject* instance = __IncludeAnnotation_newInstance();
		assert( instance->getComponent()->getFullName() == "co.IncludeAnnotation" );
		return instance;
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createIncludeAnnotationReflector()
{
    return new IncludeAnnotation_Reflector;
}

} // namespace co
