--------------------------------------------------------------------------------
-- Module 'pretty' provides functions for displaying data in human-readable form
--------------------------------------------------------------------------------

local type = type
local pairs = pairs
local assert = assert
local select = select
local tostring = tostring
local tsort = table.sort
local tconcat = table.concat
local rep = string.rep
local gsub = string.gsub
local find = string.find
local format = string.format

--------------------------------------------------------------------------------
-- Internal Functions
--------------------------------------------------------------------------------

local function formatScalar( tp, var )
	if tp == 'string' then
		return gsub( format( '%q', var ), '^"([^"\']*)"$' , "'%1'" )
	else
		return tostring( var )
	end
end

local function asKey( var, separator )
	local tp = type( var )
	if tp == 'string' and find( var, '^[%w_]+$' ) then
		return separator .. var
	end
	return '[' .. formatScalar( tp, var ) .. ']'
end

local function formatHorizontalList( t, maxLength )
	local res, sep = '', ''
	for i = 1, #t do
		local v = t[i]
		local tp = type( v )
		if tp == 'table' then return end
		res = res .. sep .. formatScalar( tp, v )
		if #res > maxLength then return end
		sep = ','
	end
	return '(' .. res .. ')'
end

local function formatHorizontalMap( t, maxLength )
	local res, sep = '', ''
	for k, v in pairs( t ) do
		local tp = type( v )
		if tp == 'table' then return end
		res = res .. sep .. asKey( k, '' ) .. '=' .. formatScalar( tp, v )
		if #res > maxLength then return end
		sep = ','
	end
	return '{' .. res .. '}'
end

local function compareAsString( v1, v2 )
	return tostring( v1 ) < tostring( v2 )
end

local function longDump( buffer, name, key, var, tp )
	local res
	if tp == 'table' then
		res = buffer[var]
		if not res then
			buffer[var] = key
			local listOfKeys = {}
			for k, _ in pairs( var ) do
				listOfKeys[#listOfKeys + 1] = k
			end
			if #listOfKeys == 0 then
				res = '{}'
			else
				local maxLength = 75 - #name
				res = ( #listOfKeys == #var and formatHorizontalList( var, maxLength ) )
				if not res then
					res = formatHorizontalMap( var, maxLength )
					if not res then
						tsort( listOfKeys, compareAsString )
						for i = 1, #listOfKeys do
							local k = assert( listOfKeys[i] )
							local suffix = asKey( k, '.' )
							local v = var[k]
							longDump( buffer, name .. suffix, key .. suffix, v, type( v ) )
							name = rep( ' ', #name )
						end
						return
					end
				end
			end
		end
	else
		res = formatScalar( tp, var )
	end
	local n = #buffer + 1
	if name ~= '' then
		buffer[n] = name
		n = n + 1
		buffer[n] = ' = '
		n = n + 1
	end
	buffer[n] = res
	n = n + 1
	buffer[n] = '\n'
end

--------------------------------------------------------------------------------
-- Module Functions
--------------------------------------------------------------------------------

local M = {}

--[[
 pretty.dump( [name,] var )
 Returns a full human-readable string dump of the passed variable.
 Useful for debugging. Properly deals with tables and cycles.
--]]
local function dump( name, ... )
	local hasName = ( select( '#', ... ) > 0 )
	if hasName then
		var = select( 1, ... )
	else
		name, var = '', name
	end

	local tp = type( var )
	if tp == 'table' then
		local stringBuffer = {}
		longDump( stringBuffer, name, name, var, tp )
		return tconcat( stringBuffer, '', 1, #stringBuffer - 1 )
	else
		local res = formatScalar( tp, var )
		if name ~= '' then
			return name .. ' = ' .. res
		else
			return res
		end
	end
end

M.dump = dump

--[[
 pretty.dumpGlobals()
 Returns a full human-readable string dump of the global environment.
 Useful for debugging. Properly deals with tables and cycles.
--]]
function M.dumpGlobals()
	return dump( '_G', _G )
end

--[[
 pretty.print( [name,] var )
 Prints a full human-readable dump of the passed variable.
 Useful for debugging. Properly deals with tables and cycles.
--]]
function M.print( name, var )
	print( dump( name, var ) )
end

--[[
 pretty.printGlobals()
 Prints a full human-readable dump of the global environment.
 Useful for debugging. Properly deals with tables and cycles.
--]]
function M.printGlobals()
	print( dump( '_G', _G ) )
end

return M
