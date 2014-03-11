local function template( writer, c, t )
	writer( "enum ", t.name, "\n{\n" )
	for i, id in ipairs( t.identifiers ) do
		writer( "\t", id, ",\n" )
	end
	writer( "\t__", t.name, "__FORCE_SIZEOF_UINT32 = 0xFFFFFFFF\n};\n" )
end

return template
