-- Used in LuaTests.cpp's interceptor test

local M = {}

function M.get( obj, index )
	return obj[index]
end

function M.serviceCalls()
	-- one getField() and one invoke()
	local arrayType = co.system.types:getType( "int32[]" )

	-- one setField()
	arrayType.reflector = nil
end

function M.objectCalls()
	local object = co.new( "moduleA.TestComponent" )

	-- one getService()
	local service = object.transaction

	-- one getField() and one setService()
	object.itf = service.interface
end

function M.moreCalls()
	co.Type.uint32.reflector = co.Type.uint32.currentReflector
end

return M
