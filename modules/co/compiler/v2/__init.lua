-------------------------------------------------------------------------------
-- Coral CSL Compiler Module
-------------------------------------------------------------------------------

local lfs = require "lfs"
local path = require "lua.path"

-- Compiler Class
local Compiler = {
	------ Default Field Values --
	outDir = "./generated",		-- output dir for generated files
	log = function( ... ) end,	-- function called to print misc. info (stats, etc.)
}

Compiler.__index = Compiler

-- Creates a new Compiler instance.
function Compiler.new()
	local self = setmetatable( {}, Compiler )

	-- setting to true disables the use of caches and the writing of files
	self.simulation = false

	-- array of module types
	self.types = {}

	-- map of direct type dependencies for the module
	-- includes all static module types and all explicitly-specified dependencies
	-- these are the types for which we should generate mappings
	self.dependencies = {} -- map[type] = distance

	return self
end

return Compiler
