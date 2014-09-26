local coICppBlock = co.Type "co.ICppBlock"

local function template( writer, c, t )
	local cppBlock = t[coICppBlock]
	if cppBlock then
		writer( cppBlock.value )
	end
end

return template
