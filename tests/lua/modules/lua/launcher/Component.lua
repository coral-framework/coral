local Component = co.Component "lua.launcher.Component"

function Component:main( args )
	if #args < 1 then
		return -1
	end

	print( "ARGS: " .. table.concat( args, " " ) )

	return 0
end
