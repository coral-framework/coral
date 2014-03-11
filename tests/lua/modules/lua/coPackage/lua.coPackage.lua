require "lua.test"

local M = {}

function M:initialize( module )
	-- test the co.system interface
	ASSERT_TRUE( co.system )
	ASSERT_EQ( "co.ISystem", co.system.interface.fullName )

	-- test the co.getType() function
	ASSERT_EQ( "bool", co.getType( "bool" ).fullName )
	ASSERT_EQ( "IArray", co.getType( "co.IArray" ).name )
	ASSERT_ERROR( function() co.getType( "nonExistingType" ) end, "could not load type 'nonExistingType'" )

	-- test the co.Type table
	ASSERT_EQ( co.Type "bool", co.getType( "bool" ) )
	ASSERT_EQ( co.Type["co.IArray"], co.getType( "co.IArray" ) )
	ASSERT_ERROR( function() return co.Type.nonExistingType end, "could not load type 'nonExistingType'" )

	-- test the co.new() function
	local testObject = co.new "moduleA.TestComponent"
	ASSERT_EQ( "moduleA.TestComponent", testObject.component.fullName )

	local testStruct = co.new "moduleA.TestStruct"
	ASSERT_EQ( 0, testStruct.anInt8 )

	local testNativeClass = co.new "moduleA.Vec2D"
	ASSERT_EQ( 0, testNativeClass.x )
	ASSERT_EQ( 0, testNativeClass.y )

	ASSERT_ERROR( function() co.new "co.IType" end, "co.IType is not instantiable" )
	ASSERT_ERROR( function() co.new "co.SystemState" end, "co.SystemState is not instantiable" )
	ASSERT_ERROR( function() co.new "int8" end, "int8 is not instantiable" )
	ASSERT_ERROR( function() co.new "nonExistingType" end, "could not load type 'nonExistingType'" )

	-- test the co.typeOf() function
	ASSERT_EQ( "co.ISystem", co.typeOf( co.system ) )
	ASSERT_EQ( "co.IInterface", co.typeOf( co.system.interface ) )
	ASSERT_EQ( "moduleA.TestComponent", co.typeOf( testObject ) )
	ASSERT_EQ( "moduleA.TestStruct", co.typeOf( testStruct ) )
	ASSERT_EQ( "moduleA.Vec2D", co.typeOf( testNativeClass ) )
	ASSERT_TRUE( co.typeOf( 7 ) == nil )
	ASSERT_TRUE( co.typeOf( 'x' ) == nil )
	ASSERT_TRUE( co.typeOf( co.Type ) == nil )
	ASSERT_TRUE( co.typeOf( co.typeOf ) == nil )

	-- test findScript()
	ASSERT_TRUE( co.findScript( "lua.scripts.sum" ) )
	ASSERT_TRUE( co.findScript( "lua.scripts.nonExistingScript" ) == nil )

	-- test findFile
	ASSERT_TRUE( co.findFile( "lua.coPackage", "lua.coPackage.lua" ) )
	ASSERT_TRUE( co.findFile( "lua.coPackage", "nonExisting.lua" ) == nil )
end

return M
