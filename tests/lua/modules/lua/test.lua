function ASSERT_TRUE( v )
	if not v then
		error( "ASSERT_TRUE failed (" .. tostring( v ) .. " value)", 2 )
	end
end

function ASSERT_EQ( v1, v2 )
	if v1 ~= v2 then
		error( "ASSERT_EQ failed: " .. tostring( v1 ) .. " != " .. tostring( v2 ), 2 )
	end
end

function ASSERT_ERROR( func, expectedErrorMsg )
	local ok, msg = pcall( func )
	if not ok then
		if expectedErrorMsg and ( msg ~= expectedErrorMsg and not msg:find( expectedErrorMsg, 1, true ) ) then
			error( 'ASSERT_ERROR failed: raised error ("' .. msg .. '") does not contain the expected message ("'
					.. expectedErrorMsg .. '")', 2 )
		end
	else
		local explanation = "one was expected"
		if expectedErrorMsg then
			explanation = "the following was expected: '" .. expectedErrorMsg .. "'"
		end
		error( "ASSERT_ERROR failed: no error was raised, though " .. explanation, 2 )
	end
end
