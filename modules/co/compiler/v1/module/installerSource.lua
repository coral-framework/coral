local function template( writer, c, t )
	c.header( writer, c, "ModuleInstaller class generated for module '", c.moduleName, "'" )

	writer( [[

#include "ModuleInstaller.h"
#include <co/IType.h>

]] )

	c.revisionCheck( writer, c, t )

	writer( c.moduleNS, [[::ModuleInstaller ]], c.moduleNS, [[::ModuleInstaller::sm_instance;

]], c.moduleNS, [[::ModuleInstaller::ModuleInstaller()
{
	// empty
}

]], c.moduleNS, [[::ModuleInstaller::~ModuleInstaller()
{
	// empty
}

void ]], c.moduleNS, [[::ModuleInstaller::initialize()
{
	for( unsigned int i = 0; i < TypeCount; ++i )
	{
		if( !_reflectors[i].isValid() )
			_reflectors[i] = createReflector( static_cast<TypeId>( i ) );
	}
}

void ]], c.moduleNS, [[::ModuleInstaller::install()
{
	initialize();

	for( unsigned int i = 0; i < TypeCount; ++i )
	{
		co::IReflector* reflector = _reflectors[i].get();
		reflector->getType()->setReflector( reflector );
	}
}

void ]], c.moduleNS, [[::ModuleInstaller::uninstall()
{
	for( unsigned int i = 0; i < TypeCount; ++i )
	{
		if( _reflectors[i].isValid() )
		{
			_reflectors[i]->getType()->setReflector( NULL );
			_reflectors[i] = NULL;
		}
	}
}

co::IReflector* ]], c.moduleNS, [[::ModuleInstaller::createReflector( TypeId typeId )
{
	co::IReflector* res = NULL;
	switch( typeId )
	{
]] )

	for i, t in ipairs( c.types ) do
		if t.hasReflector then
			writer( "\tcase TypeId_", t.name, ": res = ", c.moduleNS, "::__create", t.name, "Reflector(); break;\n" )
		end
	end

	writer( [[
	default: assert( false );
	};
	assert( res != NULL );
	return res;
}
]] )

end

return template
