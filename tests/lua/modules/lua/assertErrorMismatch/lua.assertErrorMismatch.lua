require "lua.test"

local M = {}

function M:initialize( module )
	ASSERT_ERROR( function() error( "different message" ) end, "expected message" )
end

return M
