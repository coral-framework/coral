-------------------------------------------------------------------------------
-- Coral CSL Compiler Module v2 (in pure Lua)
-------------------------------------------------------------------------------

local path = require "co.compiler.path"
local pretty = require "co.compiler.pretty"

local parser = require "co.compiler.v2.parser"

-- Compiler Class
local Compiler = {
  ------ Default Field Values --
  outDir = "./generated",     -- output dir for generated files
  moduleName = nil,           -- module we're operating on
  log = function( ... ) end,  -- function called to print info (stats, etc.)
}

Compiler.__index = Compiler

-- Creates a new Compiler instance.
function Compiler.new()
  local self = setmetatable({}, Compiler)

  -- array of module types
  self.types = {}

  -- map of type dependencies for the module {type => distance}
  -- includes all static module types and all explicitly-specified dependencies
  -- these are the types for which we should generate mappings
  self.dependencies = {}

  return self
end

-- Add a type for which we should generate mappings.
-- If we generate a module, it will be considered a module dependency.
function Compiler:addType(typeName)
  local filename = self:findCSL(typeName)
  if not filename then
    error("type '" .. typeName .. "' was not found in the path")
  end
  local ok, ast = pcall(parser.parseFile, filename)
  if not ok then
    error(filename..':'..ast.line..':'..ast.column..': syntax error')
  end
  pretty.print(typeName, ast)
end

-- Given a local or fully-qualified typeName returns the absolute
-- path to the type's CSL file, or nil if it doesn't exist.
function Compiler:findCSL(typeName)
  local moduleName, baseName = path.splitType(typeName)
  if moduleName == '' then
    moduleName = self.moduleName
  end
  return path.findModuleFile(moduleName, baseName .. '.csl')
end

return Compiler
