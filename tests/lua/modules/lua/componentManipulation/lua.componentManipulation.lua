require "lua.test"

local M = {}

function M:initialize( module )
	-- create a test component
	local object = co.new( "moduleA.TestComponent" )

	-- test getting the 'component' port
	ASSERT_EQ( object.component.fullName, "moduleA.TestComponent" )

	-- test setting the 'component' port
	ASSERT_ERROR( function() object.component = nil end,
		"'component' is a facet and cannot be set" )

	-- test getting a non-existing component interface
	ASSERT_EQ( object.nonExistingItf, nil )
	-- Previously: ASSERT_ERROR( function() return object.nonExistingItf end, "non-existing member 'nonExistingItf'" )

	-- test getting existing interfaces
	ASSERT_EQ( object.testInterface.facet.name, "testInterface" )
	ASSERT_EQ( object.testInterface.provider, object )

	ASSERT_EQ( object.transaction.facet.name, "transaction" )
	ASSERT_EQ( object.transaction.provider, object )

	-- test getting/setting a receptacle
	ASSERT_EQ( object.itf, nil )
	object.itf = object.testInterface.interface
	ASSERT_EQ( object.itf, object.testInterface.interface )
	object.itf = nil
	ASSERT_EQ( object.itf, nil )

	-- test setting a facet
	ASSERT_ERROR( function() object.testInterface = nil end, "'testInterface' is a facet and cannot be set" )
end

return M
