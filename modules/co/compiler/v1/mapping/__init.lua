local header = require "co.compiler.v1.mapping.header"
local footer = require "co.compiler.v1.mapping.footer"

local kindSpecificMappings = {
	TK_ENUM			= require "co.compiler.v1.mapping.enum",
	TK_EXCEPTION	= require "co.compiler.v1.mapping.exception",
	TK_STRUCT		= require "co.compiler.v1.mapping.struct",
	TK_NATIVECLASS	= require "co.compiler.v1.mapping.nativeClass",
	TK_INTERFACE	= require "co.compiler.v1.mapping.interface",
}

local function template( writer, c, t )
	header( writer, c, t )
	assert( kindSpecificMappings[t.kind] )( writer, c, t )
	footer( writer, c, t )
end

return template
