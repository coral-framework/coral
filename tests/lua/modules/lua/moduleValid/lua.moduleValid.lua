-- A simple Lua Module that prints when each method is called.
local M = {}

function M:initialize( module )
	print( module.namespace.fullName .. ":initialize()" )
end

function M:integrate( module )
	print( module.namespace.fullName .. ":integrate()" )
end

function M:integratePresentation( module )
	print( module.namespace.fullName .. ":integratePresentation()" )
end

function M:disintegrate( module )
	print( module.namespace.fullName .. ":disintegrate()" )
end

return M
