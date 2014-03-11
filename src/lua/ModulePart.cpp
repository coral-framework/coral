#include "lua_Base.h"
#include "LuaState.h"
#include "ModuleInstaller.h"
#include <co/RefPtr.h>
#include <co/ISystem.h>
#include <co/IModule.h>
#include <co/IModuleManager.h>
#include <co/IServiceManager.h>
#include <co/IModulePartLoader.h>
#include <lua/Exception.h>
#include <sstream>

namespace lua {

/*!
	The Lua module's co.IModulePart.
 */
class ModulePart : public lua::lua_Base
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

	void initialize( co::IModule* module )
	{
		// generally update this module AFTER all other modules
		module->setRank( 100000 );

		lua::ModuleInstaller::instance().install();

		LuaState::setup();

		// install our LuaModulePartLoader
		co::RefPtr<co::IObject> loaderObj = co::newInstance( "lua.ModulePartLoader" );
		_luaModulePartLoader = loaderObj->getService<co::IModulePartLoader>();
		co::getSystem()->getModules()->installLoader( _luaModulePartLoader.get() );

		/*
			Manually load our Lua module ("lua/__init.lua"), which will in turn
			install our loader function into Lua's package.loaders table, in order
			to enable 'require' to load Lua scripts in the CORAL_PATH.
		 */
		std::string initScriptFileName;
		lua_State* L = LuaState::getL();
		if( !LuaState::findScript( L, "lua", initScriptFileName ) )
			throw lua::Exception( "could not find the Lua module's init script in the Coral path" );

		// call the 'lua/__init.lua' script passing in its filename as an arg
		LuaState::loadFile( L, initScriptFileName );
		LuaState::push( L, initScriptFileName );
		LuaState::call( L, 1, 0 );

		// create and register the global lua.IState service
		co::getSystem()->getServices()->addServiceProvider( co::typeOf<lua::IState>::get(), "lua.Universe" );
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
		co::getSystem()->getModules()->uninstallLoader( _luaModulePartLoader.get() );

		/*
			Last chance to call anything that needs a reflector, since
			reflectors are no longer available after the disintegration phase.
		*/
		LuaState::tearDown();
	}

	void dispose( co::IModule* )
	{
		lua::ModuleInstaller::instance().uninstall();
	}

private:
	co::RefPtr<co::IModulePartLoader> _luaModulePartLoader;
};
	
CORAL_EXPORT_COMPONENT( ModulePart, lua );
CORAL_EXPORT_MODULE_PART( ModulePart );

} // namespace lua
