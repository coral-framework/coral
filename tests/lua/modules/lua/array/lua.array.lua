require "lua.test"

local M = {}

function M:initialize( module )
	-- test the number of types in the lua module
	local luaModuleTypes = co.system.types:findNamespace( "lua" ).types
	ASSERT_EQ( 13, #luaModuleTypes )
	ASSERT_EQ( "Component", luaModuleTypes[1].name )
	ASSERT_EQ( "Universe", luaModuleTypes[12].name )

	-- test a method that returns an array of structs as an output param
	local type, cslErrors = co.system.types:loadType( "TypeLoaderTests.NestedErrors.Struct1" )
	ASSERT_EQ( nil, type )
	ASSERT_EQ( 5, #cslErrors )
end

return M
