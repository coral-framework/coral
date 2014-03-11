require "lua.test"

local M = {}

function M:initialize( module )
	ASSERT_ERROR( function() return noError end )
end

return M
