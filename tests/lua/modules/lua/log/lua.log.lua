require "lua.test"

local M = {}

function M:initialize( module )
	co.log( 'INFO', "Installing Lua log handler " )

	-- From now on, capture all log events
	local lastMsg, lastLevel
	co.setLogHandler( function( message, level, isDebug )
		lastMsg = message
		lastLevel = level
	end )

	ASSERT_EQ( lastMsg, nil )
	ASSERT_EQ( lastLevel, nil )

	co.log( "Should default to warning" )
	ASSERT_EQ( lastMsg, "Should default to warning" )
	ASSERT_EQ( lastLevel, co.LogLevel.WARNING )

	co.log( 'INFO', "Some info" )
	ASSERT_EQ( lastMsg, "Some info" )
	ASSERT_EQ( lastLevel, co.LogLevel.INFO )

	ASSERT_ERROR( function() co.log() end, "nothing to log" )
	ASSERT_ERROR( function() co.log( 'a', 'b' ) end, "invalid log level" )
	ASSERT_ERROR( function() co.log( 'INFO', 'a', 'b' ) end, "co.log() takes only 2 arguments" )
	ASSERT_ERROR( function() co.log( 2, 'x' ) end, "number expected, got string" )
	ASSERT_ERROR( function() co.log( 'ERROR', true ) end, "string expected, got boolean" )

	co.LogLevel['FAKE'] = 9
	ASSERT_ERROR( function() co.log( 'FAKE', 'x' ) end, "level 8 is out of range" )
	co.LogLevel['FAKE'] = nil

	co.setLogHandler( nil )

	co.log( 'INFO', "Restored default log handler" )
end

return M
