-------------------------------------------------------------------------------
-- Coral CSL Compiler Module
-------------------------------------------------------------------------------

local lfs = require "lfs"
local path = require "co.compiler.path"

local utils = require "co.compiler.v1.utils"
local dependencies = require "co.compiler.v1.dependencies"
local TypeWrapper = require "co.compiler.v1.TypeWrapper"

local json = require "co.compiler.v1.json"
local mapping = require "co.compiler.v1.mapping"

local componentBaseHeader = require "co.compiler.v1.module.componentBaseHeader"
local componentBaseSource = require "co.compiler.v1.module.componentBaseSource"
local componentTemplate = require "co.compiler.v1.module.componentTemplate"
local nativeClassAdapter = require "co.compiler.v1.module.nativeClassAdapter"
local nativeClassTemplate = require "co.compiler.v1.module.nativeClassTemplate"
local reflector = require "co.compiler.v1.module.reflector"

local moduleAllInOne = require "co.compiler.v1.module.allInOne"
local moduleBootstrap = require "co.compiler.v1.module.bootstrap"
local moduleInstallerHeader = require "co.compiler.v1.module.installerHeader"
local moduleInstallerSource = require "co.compiler.v1.module.installerSource"
local moduleDefaultPart = require "co.compiler.v1.module.defaultPart"

-- Compiler Class
local Compiler = {
	------ Default Field Values --
	outDir = "./generated",		-- output dir for generated files
	log = function( ... ) end,	-- function called to print misc. info (stats, etc.)

	------ Easy access to re-usable modules/templates ------
	utils = require "co.compiler.v1.utils",
	header = require "co.compiler.v1.header",
	revisionCheck = require "co.compiler.v1.module.revisionCheck",
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

-- Add a type for which we should generate mappings.
-- It will be considered a module dependency, if we're generating a module.
function Compiler:addType( typeName )
	local t = TypeWrapper:wrap( co.Type[typeName] )
	if t.kind ~= 'TK_COMPONENT' or not t.isDynamic then
		self.dependencies[t] = 1
	end
end

local numExpandedFiles = 0

-- Creates a dir hierarchy, opens a file and writes the given template, passing it 'data'.
local function expand( dir, filename, template, ... )
	path.makePath( dir )
	local f = io.open( dir .. '/' .. filename, "w" )
	template( function( ... ) f:write( ... ) end, ... )
	f:close()
	numExpandedFiles = numExpandedFiles + 1
end

-- Writes all module data (types, decorators, etc.) to a JSON file.
function Compiler:generateJSON( moduleName )
	assert( not self.updatedTypes and #self.types == 0, "this compiler instance has already been used" )
	self.moduleName = moduleName
	self:loadModuleTypes()
	self.log( "Exporting module '" .. moduleName .. "' to JSON..." )
	expand( self.outDir, moduleName .. '.json', json, self )
	self.log( "All data has been written to " .. self.outDir .. '/' .. moduleName .. ".json" )
end

local function isBuiltInType( t )
	local name = t.fullName
	return name == 'co.IService' or name == 'co.TypeKind'
end

local function reportChanges( action, subject, changed, total )
	if changed == 0 then
		return "All " .. total .. " " .. subject .. "s are up to date."
	else
		local msg = action .. " " .. changed .. " " .. subject .. ( changed > 1 and 's' or '' )
		if changed < total then
			local numChanged = ( total - changed )
			msg = msg .. " (" .. numChanged .. ( numChanged > 1 and " were" or " was" ) .. " up to date)"
		end
		return msg .. "."
	end
end

-- Runs the compiler to generate mappings. Use this when you don't need to generate a module.
function Compiler:generateMappings()
	assert( not self.updatedTypes, "this compiler instance has already been used" )

	-- add implicit dependencies of the Coral API (e.g. Any.h)
	self:addType( "co.IEnum" )
	self:addType( "co.IArray" )
	self:addType( "co.IStruct" )
	self:addType( "co.IException" )
	self:addType( "co.IInterface" )
	self:addType( "co.IComponent" )
	self:addType( "co.INativeClass" )

	-- if not called through generateModule(), we must load the cache ourselves
	if not self.moduleName then
		self:loadCache()
	end

	local cachedTypes = self.cachedTypes
	local updatedTypes = {}
	local numMappings = 0
	numExpandedFiles = 0

	-- generate mappings for all direct and indirect dependencies
	for t, dist in dependencies( self.dependencies ) do
		-- we may find additional direct module dependencies
		if dist <= 1 then self.dependencies[t] = dist end
		-- only create mappings for non-component, non-built-in types
		if t.kind ~= 'TK_COMPONENT' and not isBuiltInType( t ) then
			numMappings = numMappings + 1
			-- only regenerate out-of-date files
			if cachedTypes[t.fullName] ~= t.fullSignature then
				local dir = self.outDir .. '/' .. t.namespace.fullName:gsub( '%.', '/' )
				expand( dir, t.name .. '.h', mapping, self, t )
				updatedTypes[t.fullName] = t.fullSignature
			end
		end
	end

	self.updatedTypes = updatedTypes

	self.log( reportChanges( "Created", "mapping", numExpandedFiles, numMappings ) )

	if not self.moduleName then
		self:saveCache()
	end
end

-- Runs the compiler to generate a module. Also generates the necessary mappings.
function Compiler:generateModule( moduleName )
	assert( self.moduleName == nil, "this compiler instance has already been used" )

	self.moduleName = moduleName
	self.moduleNS = self.utils.toCppName( moduleName )

	if not self.simulation then
		self:loadCache()
	end

	self:loadModuleTypes()

	local doing = ( ( not self.simulation and next( self.cachedTypes ) ) and "Updating" or "Generating" )
	self.log(  doing .. " code for module '" .. self.moduleName .. "' (" .. #self.types .. " types)..." )

	-- add implicit dependencies of the generated module code
	self:addType( "co.ISystem" )
	self:addType( "co.INamespace" )
	self:addType( "co.IReflector" )
	self:addType( "co.IModulePart" )

	if not self.simulation then
		-- generateMappings() also adds entries to self.dependencies
		self:generateMappings()
	end

	local cachedTypes = self.cachedTypes
	local updatedTypes = self.updatedTypes
	local numFiles = 0
	numExpandedFiles = 0

	local function cachedExpand( dir, filename ) numFiles = numFiles + 1 end
	local function updateExpand( dir, filename, template, c, t )
		numFiles = numFiles + 1
		if t and t.fullName then
			updatedTypes[t.fullName] = t.fullSignature
		end
		expand( dir, filename, template, c, t )
	end

	local expand -- set to either cachedExpand or updateExpand

	local outDir = self.outDir
	local templatesDir = outDir .. "/@templates"

	-- list of all .cpp files that must be compiled into the module
	local moduleSources = {}
	local function expandModuleSource( dir, filename, ... )
		expand( dir, filename, ... )
		moduleSources[#moduleSources + 1] = filename
	end

	-- Generate per-type files
	for i, t in ipairs( self.types ) do
		-- only regenerate out-of-date files
		if self.simulation or cachedTypes[t.fullName] == t.fullSignature then
			expand = cachedExpand
		else
			expand = updateExpand
		end

		if not t.isDynamic then
			if t.kind == 'TK_NATIVECLASS' then
				expand( outDir, t.name .. "_Adapter.h", nativeClassAdapter, self, t )
				expand( templatesDir, t.name .. ".cpp", nativeClassTemplate, self, t )
			elseif t.kind == 'TK_COMPONENT' then
				expand( outDir, t.name .. "_Base.h", componentBaseHeader, self, t )
				expandModuleSource( outDir, t.name .. "_Base.cpp", componentBaseSource, self, t )
				expand( templatesDir, t.name .. ".cpp", componentTemplate, self, t )
			end
			if t.kind ~= 'TK_ENUM' then
				expandModuleSource( outDir, t.name .. "_Reflector.cpp", reflector, self, t )
			end
		end
	end

	-- Generate per-module files (only if at least one file was out of date)
	if not self.simulation then
		local shouldUpdateModuleFiles = ( numExpandedFiles > 0 or self.cachedNumModuleFiles > #self.types )
		expand = ( shouldUpdateModuleFiles and updateExpand or cachedExpand )
	end
	expandModuleSource( outDir, "__Bootstrap.cpp", moduleBootstrap, self )
	expand( outDir, "ModuleInstaller.h", moduleInstallerHeader, self )
	expandModuleSource( outDir, "ModuleInstaller.cpp", moduleInstallerSource, self )
	if not self:hasCustomModulePart() then
		expandModuleSource( outDir, "__ModulePart.cpp", moduleDefaultPart, self )
	end

	-- Generate the special "all-in-one" source file
	expand( outDir, "__AllInOne.cpp", moduleAllInOne, self, moduleSources )

	self.log( reportChanges( "Generated", "module file", numExpandedFiles, numFiles ) )

	if not self.simulation then
		self:saveCache()
	end

	return moduleSources
end

-- Loads all module types by locating CSL files in the module's namespace
-- (which may be scattered in several dirs in the Coral path).
function Compiler:loadModuleTypes()
	assert( #self.types == 0 )
	local moduleDirPath = self.moduleName:gsub( '%.', '/' )
	local coralPaths = co.getPaths()
	for i, repositoryDir in ipairs( coralPaths ) do
		local moduleDir = repositoryDir .. '/' .. moduleDirPath
		if path.isDir( moduleDir ) then
			for filename in lfs.dir( moduleDir ) do
				local typeName = filename:match( "(.+)%.csl$" )
				if typeName then
					local moduleType = TypeWrapper:wrap( co.Type[self.moduleName .. '.' .. typeName] )
					self.types[#self.types + 1] = moduleType
					self.dependencies[moduleType] = 0
				end
			end
			if #self.types > 0 then
				break -- already got our module types
			end
		end
	end
end

-- Returns whether the module has a valid custom co.IModulePart.
function Compiler:hasCustomModulePart()
	local localModuleName = self.moduleName:match( '.-([^%.]+)$' )
	local t = co.system.types:findType( self.moduleName .. '.' .. localModuleName )
	local modulePartItf = co.Type "co.IModulePart"
	if t and t.kind == 'TK_COMPONENT' then
		for i, facet in ipairs( t.facets ) do
			if facet.type:isA( modulePartItf ) then
				if facet.name == 'part' then
					return true
				end
				error( "Module '" .. self.moduleName .. "' contains a component named '" ..
					self.moduleName .. "', but its co.IModulePart facet is not named 'part'.", 0  )
			end
		end
		error( "Module '" .. self.moduleName .. "' contains a component named '" ..
			self.moduleName .. "', but it does not provide the co.IModulePart interface.", 0  )
	end
	return false
end

local function loadCacheFile( filename, cachedTypes )
	local f = io.open( filename, 'r' )
	local code = f:read( "*a" )
	f:close()

	local chunk, err = load( code, filename, 't', cachedTypes )
	if not chunk then
		return print( "Error in cache file '" .. filename .. "': " .. tostring( err ) )
	end

	local ok, moreCachedTypes, numModuleFiles, compilerRevision = pcall( chunk )
	if not ok then
		return print( "Error in cache file '" .. filename .. "': " .. tostring( moreCachedTypes ) )
	end

	if type( moreCachedTypes ) ~= 'table' then
		return print( "Cache file '" .. filename .. "' returned an invalid result (" .. tostring( moreCachedTypes ) .. ")." )
	end

	if compilerRevision ~= co.compilerOutputRevision then
		return print( "Code generation rules have changed - forcing full regen... " )
	end

	for k, v in pairs( moreCachedTypes ) do
		local current = cachedTypes[k]
		if current then
			if current ~= v then
				cachedTypes[k] = 'collision' -- handle collisions by forcing an update
			end
		else
			cachedTypes[k] = v
		end
	end
	return numModuleFiles
end

local function saveCacheFile( filename, cachedTypes, numModuleFiles )
	local f = io.open( filename, "w" )
	f:write( [[
-------------------------------------------------------------------------------
-- Coral Compiler v]], co.version, [[ Cache File
-- Generated on ]], utils.currentDateTime(), "\n", [[
-- Delete this file if you want to force all files to be regenerated.
-------------------------------------------------------------------------------

return {
]] )
	for k, v in pairs( cachedTypes ) do
		f:write( '\t["', k, '"] = "', v, '",\n' )
	end
	f:write( "}, ", numModuleFiles, ", ", co.compilerOutputRevision, "\n" )
	f:close()
end

local CACHE_FILE_NAME = '/__coralc_cache.lua'

function Compiler:loadCache()
	local cachedTypes = {}
	local cachedNumModuleFiles

	-- if one of the standard, compiler-generated files is missing, ignore the cache files and
	-- force a full update; this is a workaround for IDEs with inflexible 'clean' routines (e.g. MSVC)
	-- which delete most of the generated source files, but not the cache file.
	local ignoreCache = (not path.isFile(self.outDir .. '/co/ISystem.h') or
			(self.moduleName and not path.isFile(self.outDir .. '/__Bootstrap.cpp')))

	if not ignoreCache then
		local filename = self.outDir .. CACHE_FILE_NAME
		if path.isFile( filename ) then
			cachedNumModuleFiles = loadCacheFile( filename, cachedTypes )
		end
	end

	self.cachedTypes = cachedTypes
	self.cachedNumModuleFiles = cachedNumModuleFiles or 0
end

function Compiler:saveCache()
	local cachedTypes = self.cachedTypes
	local updatedTypes = self.updatedTypes
	assert( type( cachedTypes ) == 'table' )
	-- merge 'updatedTypes' into 'cachedTypes'
	for k, v in pairs( updatedTypes ) do
		cachedTypes[k] = v
	end
	saveCacheFile( self.outDir .. CACHE_FILE_NAME, self.cachedTypes, #self.types )
end

return Compiler
