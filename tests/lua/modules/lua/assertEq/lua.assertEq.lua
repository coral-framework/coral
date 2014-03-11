require "lua.test"

local M = {}

function M:initialize( module )
	ASSERT_EQ( 1, 2 )
end

return M
