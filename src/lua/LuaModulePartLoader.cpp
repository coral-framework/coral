#include "LuaState.h"
#include "ModulePartLoader_Base.h"
#include <co/Coral.h>
#include <co/IModulePart.h>
#include <co/ModuleLoadException.h>
#include <co/IllegalArgumentException.h>
#include <co/reserved/OS.h>
#include <lua/Exception.h>

namespace lua {

/*!
	A co.IModulePartLoader that allows Coral modules to be written in Lua.
	If a module 'foo' contains a script named 'foo.lua', this component
	will create a lua.ModulePart to wrap the script as a co.IModulePart.
 */
class LuaModulePartLoader : public ModulePartLoader_Base
{
public:
	LuaModulePartLoader()
	{
		// empty
	}

	~LuaModulePartLoader()
	{
		// empty
	}

	bool canLoadModulePart( const std::string& moduleName )
	{
		return locateModuleLibrary( moduleName );
	}

	co::IModulePart* loadModulePart( const std::string& moduleName )
	{
		std::string libraryFilename;
		if( !locateModuleLibrary( moduleName, &libraryFilename ) )
			throw co::ModuleLoadException( "unexpected missing module library file" );

		lua_State* L = LuaState::getL();
		LuaState::loadFile( L, libraryFilename );
		LuaState::call( L, 0, 1 );

		// we expect the module script to return a table
		if( lua_type( L, -1 ) != LUA_TTABLE )
		{
			lua_pop( L, 1 );
			throw lua::Exception( "the module script must return a table" );
		}

		return co::newInstance( "lua.ModulePart" )->getService<co::IModulePart>();
	}

private:
	bool locateModuleLibrary( const std::string& moduleName, std::string* filename = 0 )
	{
		// 'module.name' => 'module/name/module.name.lua'
		std::string filePath;
		filePath.reserve( moduleName.length() * 2 + 5 );
		filePath.assign( moduleName );
		co::OS::convertDotsToDirSeps( filePath );
		filePath.push_back( CORAL_OS_DIR_SEP );
		filePath.append( moduleName );
		filePath.append( ".lua" );

		return co::OS::searchFile2( co::getPaths(),
									co::Slice<std::string>( &filePath, 1 ),
									filename ? *filename : filePath );
	}
};

CORAL_EXPORT_COMPONENT( LuaModulePartLoader, ModulePartLoader );

} // namespace lua
