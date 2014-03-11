require "lua.test"

local M = {}

function M:initialize( module )
	ASSERT_TRUE( thisNonExistingGlobalIsNilAndThusFalse )
end

return M
