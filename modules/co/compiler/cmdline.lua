-------------------------------------------------------------------------------
-- Module for processing Command-Line Arguments.
-------------------------------------------------------------------------------
--
-- [leftOvers][,errorString] = process( args, handlers )
--
-- Processes flags in the 'args' array by calling functions in the 'handlers'
-- table. Returns an array of leftovers (unprocessed, non-flag arguments), or
-- nil and an error string in case of error.
--
-- The handlers table is a mapping of "flag patterns" to handler functions.
-- A flag pattern is a Lua pattern string containing up to one optional capture.
-- A handler function is called when a flag pattern fully matches a certain flag.
-- Warning: behaviour is undefined if two given patterns can match the same flag.
--
-- Handler functions receive the capture of the flag that triggered their call,
-- plus any subsequent non-flag arguments (potential parameters for the flag).
-- When no captures are specified in a flag pattern, the whole flag is passed.
-- Handler functions must return the number (where nil = 0) of consumed non-flag
-- arguments; or nil and an error string, in case of error.
--
-- Signature of a handler function:
--     [numUsedArgs][,errorString] = function( flagName [, ...] )
--
-- Sample 'handlers' table:
--     local flagHandlers = {
--         v = function() print( "Foo v0.1" ) end,
--         version = 'v',
--         ['enable%-(.+)'] = function( c ) print( "Enabled: " .. c ) end
--     }

local M = {}

-- localize commonly used functions
local smatch = string.match
local sfind = string.find
local unpack = table.unpack
local pairs = pairs
local type = type
local tonumber = tonumber

-- unpackNonFlags( args, startPos )
-- Returns arguments from 'args[startPos]' onwards, until next flag.
local function unpackNonFlags( args, startPos )
	local endPos = 0
	for i = startPos, #args do
		local v = args[i]
		if not v or v:sub( 1, 1 ) == '-' then
			break
		else
			endPos = i
		end
	end
	return unpack( args, startPos, endPos )
end

function M.process( args, handlers )
	-- gather unprocessed, non-flag parameters in 'leftOvers'
	local leftOvers = {}

	-- separate pattern-based flags from plain-string flags
	local patterns = {}
	for k, v in pairs( handlers ) do
		if sfind( k, "^$()%.[]*+-?", 1, true ) then
			patterns["^" .. k .. "$"] = v
		end
	end

	-- process args
	local i = 1
	while args[i] do
		local v = args[i]
		local _, _, flag = sfind( v, "^%-+(.+)$" )

		if not flag then
			leftOvers[#leftOvers + 1] = v
		else
			local handler, capture

			-- first, try to match to a plain string
			handler = handlers[flag]
			if handler then
				capture = flag
			else
				-- search for a matching pattern-based flag
				for p, h in pairs( patterns ) do
					capture = smatch( flag, p )
					if capture then handler = h break end
				end
			end

			if not handler then
				return nil, "Unexpected flag: '" .. v .. "'"
			elseif type( handler ) == 'string' then
				-- flag aliasing
				handler = handlers[handler]
			end

			if type( handler ) ~= 'function' then
				error( "Invalid handler for '" .. v .. "':" .. tostring( handler ), 0 )
			end

			-- Now, find out how many non-flag parameters we can provide the call
			local n, errorString = handler( capture, unpackNonFlags( args, i + 1 ) )
			if errorString then
				return nil, "Error near '" .. v .. "': " .. errorString .. "."
            end

			if n then
				n = tonumber( n )
				if not n then
					error( "Handler for '" .. flag .. "' returned a bad, non-numeric value.", 0 )
				end
				i = i + n  -- skip n
			end
		end
		i= i + 1
	end

	return leftOvers
end

return M
