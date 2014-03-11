#include "FaultyAnnotation_Base.h"
#include <co/Coral.h>
#include <co/IType.h>
#include <co/NotSupportedException.h>

namespace moduleA {

class FaultyAnnotation : public FaultyAnnotation_Base
{
public:
	std::string getValue()
	{
		return co::getType( "moduleA.tests.FaultyType" )->getFullName();
	}
	
	void setValue( const std::string& )
	{
		co::getType( "moduleA.tests.FaultyType" );
		throw co::NotSupportedException( "unexpected absence of errors" );
	}
};

CORAL_EXPORT_COMPONENT( FaultyAnnotation, FaultyAnnotation );

} // namespace moduleA
