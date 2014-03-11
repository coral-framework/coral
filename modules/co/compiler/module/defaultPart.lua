local function template( writer, c )
	c.header( writer, c, "Default co::IModulePart component generated for module '", c.moduleName, "'" )

	writer( [[

#include "ModuleInstaller.h"
#include <co/IModule.h>
#include <co/reserved/ModulePartBase.h>

]] )

	c.revisionCheck( writer, c )

	c.utils.openNamespaces( writer, c.moduleName )

	writer( [[

void moduleRetain();
void moduleRelease();

class __ModulePart : public co::ModulePartBase
{
public:
	__ModulePart()
	{
		]], c.moduleNS, [[::moduleRetain();
	}

	virtual ~__ModulePart()
	{
		]], c.moduleNS, [[::moduleRelease();
	}

	void initialize( co::IModule* )
	{
		]], c.moduleNS, [[::ModuleInstaller::instance().install();
	}

	void dispose( co::IModule* )
	{
		]], c.moduleNS, [[::ModuleInstaller::instance().uninstall();
	}
};

CORAL_EXPORT_MODULE_PART( __ModulePart )

]] )

	c.utils.closeNamespaces( writer, c.moduleName )

end

return template
