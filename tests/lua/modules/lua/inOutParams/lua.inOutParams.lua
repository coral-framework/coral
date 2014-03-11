require "lua.test"

local M = {}

local Component = co.Component{
	name = "lua.test.InOutParams",
	provides = { shift = "moduleA.InOutTests", noret = "moduleA.InOutTests", const = "moduleA.InOutTests" }
}

function Component.shift:testAny( a, b )
	return a, b
end

Component.shift.testStruct = Component.shift.testAny
Component.shift.testArrayOfInts = Component.shift.testAny
Component.shift.testArrayOfAnys = Component.shift.testAny
Component.shift.testArrayOfStructs = Component.shift.testAny

function Component.noret:testAny( a, b )
	if type( a ) == 'table' then
		ASSERT_EQ( #a, #b )
		for i = 1, #a do
			ASSERT_EQ( a[i], b[i] )
		end
	else
		ASSERT_EQ( a, b )
	end
end

Component.noret.testStruct = Component.noret.testAny
Component.noret.testArrayOfInts = Component.noret.testAny
Component.noret.testArrayOfAnys = Component.noret.testAny
Component.noret.testArrayOfStructs = Component.noret.testAny

function Component:testAny( a, b )
	ASSERT_EQ( a, 5.5 )
	ASSERT_EQ( b.message, "B" )
	return true, "str"
end

function Component:testStruct( a, b )
	ASSERT_EQ( a.testEnum, 'Second' )
	ASSERT_EQ( a.innerStruct.message, "A" )
	ASSERT_EQ( b.testEnum, 'Third' )
	ASSERT_EQ( b.innerStruct.message, "B" )
	local res = co.new "moduleA.TestStruct"
	res.testEnum = 'Second'
	local cslError = res.innerStruct
	cslError.message = "omgitworks"
	res.innerStruct = cslError
	return res, co.new "moduleA.TestStruct"
end

function Component:testArrayOfInts( a, b )
	ASSERT_TRUE( #a == 3 and a[1] == 1 and a[2] == 2 and a[3] == 3 )
	ASSERT_TRUE( #b == 2 and b[1] == 4 and b[2] == 5 )
	return { 6, 7 }, { 8.1, 9.2, 0.3 }
end

function Component:testArrayOfAnys( a, b )
	ASSERT_TRUE( #a == 3 and a[1] == 3.2 and a[2] == nil and a[3] == "str" )
	ASSERT_TRUE( #b == 2 and b[1].message == "A" and b[2] == false )
	return { 3.2, nil }, { "str", false }
end

function Component:testFile( i )
	ASSERT_FALSE( i.isOpen )
	return i, i
end

function M:initialize( module )
	local c = Component()
	local shift = c.shift
	local noret = c.noret
	local const = c.const

	-- re-usable args
	local cslErrorA, cslErrorB = co.new "co.CSLError", co.new "co.CSLError"
	cslErrorA.message = "A"
	cslErrorB.message = "B"

	local tstStructA, tstStructB = co.new "moduleA.TestStruct", co.new "moduleA.TestStruct"
	tstStructA.testEnum = 'Second'
	tstStructA.innerStruct = cslErrorA
	tstStructB.testEnum = 'Third'
	tstStructB.innerStruct = cslErrorB

	-- test any params
	local x, y = const:testAny( 5.5, cslErrorB )
	ASSERT_EQ( x, true )
	ASSERT_EQ( y, "str" )

	x, y = shift:testAny( cslErrorA, cslErrorB )
	ASSERT_EQ( x.message, cslErrorA.message )
	ASSERT_EQ( y.message, cslErrorB.message )

	-- test struct params
	x, y = const:testStruct( tstStructA, tstStructB )
	ASSERT_EQ( x.testEnum, 'Second' )
	ASSERT_EQ( x.innerStruct.message, "omgitworks" )
	ASSERT_EQ( y.testEnum, 'First' )
	ASSERT_EQ( y.innerStruct.message, "" )

	-- the passed structs should not have been modified
	ASSERT_EQ( tstStructA.testEnum, 'Second' )
	ASSERT_EQ( tstStructA.innerStruct.message, "A" )
	ASSERT_EQ( tstStructB.testEnum, 'Third' )
	ASSERT_EQ( tstStructB.innerStruct.message, "B" )

	x, y = shift:testStruct( tstStructB, tstStructA )
	ASSERT_EQ( x.testEnum, 'Third' )
	ASSERT_EQ( x.innerStruct.message, "B" )
	ASSERT_EQ( y.testEnum, 'Second' )
	ASSERT_EQ( y.innerStruct.message, "A" )

	-- test arrays of ints
	local ai1, ai2 = { 1, 2, 3 }, { 4, 5 }

	x, y = const:testArrayOfInts( ai1, ai2 )
	ASSERT_EQ( #x, 2 ) ASSERT_EQ( x[1], 6 ) ASSERT_EQ( x[2], 7 )
	ASSERT_EQ( #y, 3 ) ASSERT_EQ( y[1], 8 ) ASSERT_EQ( y[2], 9 ) ASSERT_EQ( y[3], 0 )

	x, y = shift:testArrayOfInts( ai1, ai2 )
	ASSERT_TRUE( #x, 3 ) ASSERT_EQ( x[1], 1 ) ASSERT_EQ( x[2], 2 ) ASSERT_EQ( x[3], 3 )
	ASSERT_TRUE( #y, 2 ) ASSERT_EQ( y[1], 4 ) ASSERT_EQ( y[2], 5 )

	x, y = shift:testArrayOfInts( ai1, nil )
	ASSERT_EQ( #x, 3 ) ASSERT_EQ( x[1], 1 ) ASSERT_EQ( x[2], 2 ) ASSERT_EQ( x[3], 3 )
	ASSERT_EQ( #y, 0 )

	x, y = noret:testArrayOfInts( ai2, ai2 )
	ASSERT_EQ( #x, 0 )
	ASSERT_EQ( #y, 0 )

	-- test arrays of anys
	local aa1, aa2 = { 3.2, nil, "str" }, { cslErrorA, false }

	x, y = const:testArrayOfAnys( aa1, aa2 )
	ASSERT_TRUE( #x == 1 and x[1] == 3.2 )
	ASSERT_TRUE( #y == 2 and y[1] == "str" and y[2] == false )

	x, y = shift:testArrayOfAnys( aa2, aa1 )
	ASSERT_TRUE( #x == 2 and x[1].message == "A" and x[2] == false )
	ASSERT_TRUE( #y == 3 and y[1] == 3.2 and y[2] == nil and y[3] == "str" )

	-- test arrays of structs
	x, y = shift:testArrayOfStructs( { cslErrorB, cslErrorA, cslErrorB }, nil )
	ASSERT_EQ( #x, 3 )
	ASSERT_TRUE( x[1].message == "B" and x[2].message == "A" and x[3].message == "B" )
	ASSERT_EQ( #y, 0 )
end

return M
