
#include <co/private/ReflectorBase.h>
#include <memory>

namespace co {

// The following two functions are implemented by CORAL_EXPORT_COMPONENT()
co::int32 __DocumentationAnnotation_getSize();
co::IObject* __DocumentationAnnotation_newInstance();

//------ Reflector Component ------//

class DocumentationAnnotation_Reflector : public co::ReflectorBase
{
public:
	DocumentationAnnotation_Reflector()
	{
		// empty
	}

	virtual ~DocumentationAnnotation_Reflector()
	{
		// empty
	}

	co::IType* getType()
	{
		return co::getType( "co.DocumentationAnnotation" );
	}

	co::int32 getSize()
	{
		return __DocumentationAnnotation_getSize();
	}

	co::IObject* newInstance()
	{
		co::IObject* instance = __DocumentationAnnotation_newInstance();
		assert( instance->getComponent()->getFullName() == "co.DocumentationAnnotation" );
		return instance;
	}
};

//------ Reflector Creation Function ------//

co::IReflector* __createDocumentationAnnotationReflector()
{
    return new DocumentationAnnotation_Reflector;
}

} // namespace co
