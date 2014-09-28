require "lua.test"

local M = {}

function M:initialize( module )
	-- test getting/setting a boolean
	ASSERT_EQ( co.system.services.isLazy, true )
	co.system.services.isLazy = false
	ASSERT_EQ( co.system.services.isLazy, false )
	co.system.services.isLazy = true
	ASSERT_EQ( co.system.services.isLazy, true )

	-- instantiate some components to help with some tests
	local testComponent = co.new "moduleA.TestComponent"
	local testInterface = testComponent.testInterface

	-- test getting/setting an int8
	-- test getting/setting an uint8
	-- test getting/setting an int16
	-- test getting/setting an uint16
	-- test getting/setting an int32
	-- test getting/setting an uint32
	-- test getting/setting a float
	-- test getting/setting a double

	-- test getting/setting a string
	ASSERT_EQ( testInterface.name, "NOT SET" )
	testInterface.name = "My Name"
	ASSERT_EQ( testInterface.name, "My Name" )

	-- test getting/setting an enum
	ASSERT_EQ( testInterface.testEnum, "First" )
	testInterface.testEnum = "Second"
	ASSERT_EQ( testInterface.testEnum, "Second" )
	testInterface.testEnum = 2
	ASSERT_EQ( testInterface.testEnum, "Third" )

	-- test getting/setting a struct
	-- test getting/setting a native class
	-- test getting/setting an interface
	-- test getting/setting a simple array
	-- test getting/setting a struct array
	-- test getting/setting an interface array

	-- try setting a read-only field
	ASSERT_EQ( co.system.state, "Running" )
	ASSERT_ERROR( function() co.system.state = "Integrating" end,
		"field 'state' is read-only and cannot be changed" )
end

return M
