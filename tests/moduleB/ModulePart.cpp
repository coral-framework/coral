#include "moduleB_Base.h"
#include "ModuleInstaller.h"
#include <co/ISystem.h>
#include <co/IModuleManager.h>

namespace moduleB {

class ModulePart : public moduleB_Base
{
public:
    ModulePart()
	{
		// empty
	}

	virtual ~ModulePart()
	{
		// empty
	}

	// co::IModulePart methods:

	void initialize( co::IModule* )
	{
		moduleB::ModuleInstaller::instance().install();

		// require moduleA
		co::getSystem()->getModules()->load( "moduleA" );
	}

	void integrate( co::IModule* )
	{
		// empty
	}

	void integratePresentation( co::IModule* )
	{
		// empty
	}

	void disintegrate( co::IModule* )
	{
		// empty
	}

	void dispose( co::IModule* )
	{
		moduleB::ModuleInstaller::instance().uninstall();
	}
};

CORAL_EXPORT_COMPONENT( ModulePart, moduleB );
CORAL_EXPORT_MODULE_PART( ModulePart );

} // namespace moduleB
