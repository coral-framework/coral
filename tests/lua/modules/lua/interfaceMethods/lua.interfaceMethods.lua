require "lua.test"

local M = {}

function M:initialize( module )
	local systemType = co.system.types:findType( "co.ISystem" )
	ASSERT_EQ( systemType.fullName, "co.ISystem" )

	ASSERT_ERROR( function() co.system.services:getService( systemType ) end, "unknown service" )

	local testComponent = co.new( "moduleA.TestComponent" )
	local testItf = testComponent.testInterface

	-- test passing fewer arguments than expected
	ASSERT_ERROR( function() testItf:testInParameters( 0.1, 'First' ) end, "(7 expected, got 2)" )

	-- TODO more tests
end

return M
