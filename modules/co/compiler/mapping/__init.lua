local header = require "co.compiler.mapping.header"
local footer = require "co.compiler.mapping.footer"

local kindSpecificMappings = {
	TK_ENUM			= require "co.compiler.mapping.enum",
	TK_EXCEPTION	= require "co.compiler.mapping.exception",
	TK_STRUCT		= require "co.compiler.mapping.struct",
	TK_NATIVECLASS	= require "co.compiler.mapping.nativeClass",
	TK_INTERFACE	= require "co.compiler.mapping.interface",
}

local function template( writer, c, t )
	header( writer, c, t )
	assert( kindSpecificMappings[t.kind] )( writer, c, t )
	footer( writer, c, t )
end

return template
