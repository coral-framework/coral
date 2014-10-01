-------------------------------------------------------------------------------
-- Parses CSL into an AST
-------------------------------------------------------------------------------

local lpeg = require "lpeg"

local P = lpeg.P
local S = lpeg.S
local R = lpeg.R

local C = lpeg.C
local Cmt = lpeg.Cmt

-------------------------------------------------------------------------------
-- Patterns
-------------------------------------------------------------------------------

local eof = P(-1)

local ml_comment = P'/*' * C((1 - P'*/')^0) * P'*/'
local sl_comment = P'//' * C((1 - P'\n')^0)
local comment = (ml_comment + sl_comment)

-- matches the empty string and issues an error with location info
local err = Cmt(true, function(subject, pos, ...)
	local line, column, before = 1, nil, 1
	for i in subject:gmatch("\n()") do
		if i >= pos then break end
		before = i
		line = line + 1
	end
	error{line = line, column = pos - before}
end)

-------------------------------------------------------------------------------
-- Grammar
-------------------------------------------------------------------------------

local CSL = P {
	comment^1 * (eof + err)
}

-------------------------------------------------------------------------------
-- Module Functions
-------------------------------------------------------------------------------

local function parse(subject)
	return CSL:match(subject)
end

local function parseFile(filename)
	local file = assert(io.open(filename))
	local subject = file:read'*a'
	file:close()
	return parse(subject)
end

local M = {
	parse = parse,
	parseFile = parseFile,
}

return M
