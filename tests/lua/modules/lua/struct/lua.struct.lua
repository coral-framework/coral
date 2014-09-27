require "lua.test"

local M = {}

function M:initialize( module )
	-- create a co.CSLError struct instance
	local cslError = co.new( "co.CSLError" )

	-- test getting some fields
	ASSERT_EQ( cslError.message, "" )
	ASSERT_EQ( cslError.filename, "" )
	ASSERT_EQ( cslError.line, 0 )

	-- test getting a non-existing field
	ASSERT_EQ( cslError.dummy, nil )

	-- test setting some fields
	cslError.message = "My Message"
	ASSERT_EQ( cslError.message, "My Message" )
	cslError.filename = "My Filename"
	ASSERT_EQ( cslError.filename, "My Filename" )
	cslError.line = 1337
	ASSERT_EQ( cslError.line, 1337 )

	-- test setting a non-existing field
	ASSERT_ERROR( function() cslError.dummy = 2 end, "non-existing member 'dummy'" )

	-- create a moduleA.TestStruct struct instance
	local testStruct = co.new( "moduleA.TestStruct" )

	-- test getting some fields
	ASSERT_EQ( testStruct.testEnum, "First" )
	ASSERT_EQ( testStruct.aType, nil )

	-- test setting some fields
	testStruct.testEnum = "Third"
	ASSERT_EQ( testStruct.testEnum, "Third" )
	testStruct.aType = co.Type.int16
	ASSERT_EQ( testStruct.aType.fullName, "int16" )

	-- test setting 'testEnum' with an invalid identifier
	ASSERT_ERROR( function() testStruct.testEnum = "Fourth" end,
		"no such identifier 'Fourth' in the enum" )

	-- test getting fields of the innerStruct
	ASSERT_EQ( testStruct.innerStruct.message, "" )
	ASSERT_EQ( testStruct.innerStruct.filename, "" )
	ASSERT_EQ( testStruct.innerStruct.line, 0 )

	-- test setting fields of the innerStruct
	testStruct.innerStruct.message = "Something"
	ASSERT_EQ( testStruct.innerStruct.message, "" ) -- FIX THIS

	-- test setting the innerStruct field itself
	testStruct.innerStruct = cslError
	ASSERT_EQ( testStruct.innerStruct.message, "My Message" )
	ASSERT_EQ( testStruct.innerStruct.filename, "My Filename" )
	ASSERT_EQ( testStruct.innerStruct.line, 1337 )

	-- test gettin/setting several array fields in a modulaA.TestStruct
	local ts = co.new "moduleA.TestStruct"
	ASSERT_EQ( 0, #ts.doubleArray )
	ASSERT_EQ( 0, #ts.enumArray )
	ASSERT_EQ( 0, #ts.typeArray )

	ts.doubleArray = { 1.5, 2.5, 3.5 }
	ASSERT_EQ( 3, #ts.doubleArray )
	ASSERT_EQ( 2.5, ts.doubleArray[2] )

	ts.enumArray = { 'Second', 'Third', 'First' }
	ASSERT_EQ( 3, #ts.enumArray )
	ASSERT_EQ( 'Third', ts.enumArray[2] )

	ts.enumArray = { 2, 1, 0, 1, 2 }
	ASSERT_EQ( 5, #ts.enumArray )
	ASSERT_EQ( 'First', ts.enumArray[3] )

	ASSERT_ERROR( function() ts.enumArray = { 3 } end, "3 is out of range for the enum" )

	ts.typeArray = {}
	ASSERT_EQ( 0, #ts.typeArray )

	ts.typeArray = { co.Type "int32", co.Type "co.IArray", co.Type "any" }
	ASSERT_EQ( 3, #ts.typeArray )
	ASSERT_EQ( "int32", ts.typeArray[1].fullName )
	ASSERT_EQ( "co.IArray", ts.typeArray[2].fullName )
end

return M
