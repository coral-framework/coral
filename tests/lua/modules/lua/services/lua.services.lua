require "lua.test"

--------------------------------------------------------------------------------
-- Auxiliary Lua component that implements a dummy service
--------------------------------------------------------------------------------

local DummyProvider = co.Component{ name = "lua.test.DummyProvider", provides = { dummy = "moduleA.IDummy" } }

function DummyProvider.dummy:getFoo()
	return self.foo or "foo"
end

function DummyProvider.dummy:setFoo( foo )
	self.foo = foo
end

function bindFooForType( fooValue, typeName )
	local object = DummyProvider{ foo = fooValue }
	co.system.services:addServiceForType( co.Type "moduleA.IDummy", co.Type[typeName], object.dummy )
end

--------------------------------------------------------------------------------
-- Module / Test Code
--------------------------------------------------------------------------------

local M = {}

function M:initialize( module )
	-- make sure we don't have services registered for 'moduleA.IBat' nor 'moduleA.IHuman'.
	ASSERT_ERROR( function() co.getService "moduleA.IBat" end, "unknown service" )
	ASSERT_ERROR( function() co.getService "moduleA.IHuman" end, "unknown service" )

	-- we reuse "lua.bat.Component" from the lua.component test
	local bc = co.new "lua.bat.Component"

	-- register a global moduleA.IBat service:
	co.system.services:addService( co.Type "moduleA.IBat", bc.fruitBat )

	-- now all queries should return the global service
	ASSERT_EQ( co.getService "moduleA.IBat", bc.fruitBat )
	ASSERT_EQ( co.getService( "moduleA.IBat", "moduleA.IHuman" ), bc.fruitBat )

	-- now add a specialized bat for humans
	co.system.services:addServiceForType( co.Type "moduleA.IBat", co.Type "moduleA.IHuman", bc.vampireBat )
	ASSERT_EQ( co.getService "moduleA.IBat", bc.fruitBat )
	ASSERT_EQ( co.getService( "moduleA.IBat", "moduleA.IHuman" ), bc.vampireBat )

	-- a query for 'moduleA.IHuman' on 'bc' should give us batman
	ASSERT_EQ( co.getService( "moduleA.IHuman", bc ), bc.batman )
	ASSERT_EQ( co.getService( "moduleA.IHuman", bc.batman ), bc.batman )
	ASSERT_EQ( co.getService( "moduleA.IHuman", bc.fruitBat ), bc.batman )

	-- test for custom service providers implemented in Lua
	bindFooForType( "Bat", "moduleA.IBat" )
	bindFooForType( "Human", "moduleA.IHuman" )

	ASSERT_ERROR( function() co.getService "moduleA.IDummy" end, "service has no global instance" )
	ASSERT_EQ( co.getService( "moduleA.IDummy", "moduleA.IBat" ).foo, "Bat" )
	ASSERT_EQ( co.getService( "moduleA.IDummy", "moduleA.IHuman" ).foo, "Human" )
end

return M
