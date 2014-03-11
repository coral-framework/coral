local M = {}

function M.passthrough( ... )
	return ...
end

function M.constants()
	return nil, true, false, 3.14, "hello", co.system
end

function M.constants2()
	return "one", "two", "three", "four", "five"
end

function M.raiseError( message )
	error( message or "here is the error" )
end

function M.raise( exceptionType, message )
	co.raise( exceptionType, message )
end

function M.causeIllegalStateException()
	co.system:setupBase( {} )
end

return M
