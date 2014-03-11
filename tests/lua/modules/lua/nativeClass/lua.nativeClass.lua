require "lua.test"

local M = {}

function M:initialize( module )
	-- create a native class instance
	local vec2d = co.new( "moduleA.Vec2D" )

	-- test getting/setting some fields
	ASSERT_EQ( vec2d.x, 0 )
	ASSERT_EQ( vec2d.y, 0 )
	ASSERT_EQ( vec2d.length, 0 )
	ASSERT_EQ( vec2d.length2, 0 )

	vec2d.x = 1
	ASSERT_EQ( vec2d.x, 1 )
	vec2d.y = 2
	ASSERT_EQ( vec2d.y, 2 )

	ASSERT_EQ( vec2d.length, math.sqrt( 5 ) )
	ASSERT_EQ( vec2d.length2, 5 )

	-- test setting the read-only 'length' field
	ASSERT_ERROR( function() vec2d.length = 2 end, "field 'length' is read-only and cannot be changed" )

	-- test getting a non-existing field
	ASSERT_EQ( vec2d.dummy, nil )

	-- test the get/set methods
	vec2d:set( 3, 7 )
	ASSERT_EQ( vec2d.x, 3 )
	ASSERT_EQ( vec2d.y, 7 )

	local x, y = vec2d:get()
	ASSERT_TRUE( x == 3 and y == 7 )

	-- test dot()
	local anotherVec2d = co.new( "moduleA.Vec2D" )
	anotherVec2d:set( 2, 2 )

	ASSERT_EQ( vec2d:dot( anotherVec2d ), 20 )

	-- test inout/out native class params
	local s, a, b = co.new "moduleA.Vec2D", co.new "moduleA.Vec2D", co.new "moduleA.Vec2D"
	s:set( 1, 1 )
	a:set( 2, 2 )
	b:set( 3, 3 )

	local r1, r2, r3 = s:testInOut( a, b )

	-- returned values should be new values
	ASSERT_TRUE( r1 ~= s and r2 ~= a and r3 ~= b )

	-- instance 'b' should NOT have been modified
	ASSERT_EQ( b.x, 3 ) ASSERT_EQ( b.y, 3 )

	-- expected outcome: r1 == s, r2 == a, r3 == b
	ASSERT_EQ( r1.x, s.x ) ASSERT_EQ( r1.y, s.y )
	ASSERT_EQ( r2.x, a.x ) ASSERT_EQ( r2.y, a.y )
	ASSERT_EQ( r3.x, b.x ) ASSERT_EQ( r3.y, b.y )

	-- check returned values
	ASSERT_EQ( r1.x, 1 ) ASSERT_EQ( r1.y, 1 )
	ASSERT_EQ( r2.x, 2 ) ASSERT_EQ( r2.y, 2 )
	ASSERT_EQ( r3.x, 3 ) ASSERT_EQ( r3.y, 3 )

	-- test passing & retrieving any's
	local _, v = vec2d:dummyMethod( "str" )
	ASSERT_EQ( v, "str" )

	_, v = vec2d:dummyMethod( 3.14 )
	ASSERT_EQ( type( v ), 'number' )
	ASSERT_EQ( v, 3.14 )

	_, v = vec2d:dummyMethod( s )
	ASSERT_EQ( type( v), 'userdata' )
	ASSERT_EQ( v.x, 1 )
	ASSERT_EQ( v.y, 1 )

--[[-- currently we cannot pass an array as an 'any' argument, unfortunately
	_, v = vec2d:dummyMethod( { 4, 5, 6 } )
	ASSERT_EQ( type( v ), 'array' )
	ASSERT_EQ( v[1], 4 )
	ASSERT_EQ( v[2], 5 )
	ASSERT_EQ( v[3], 6 )
--]]

end

return M
