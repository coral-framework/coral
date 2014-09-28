local function template( writer, c, t )
	writer( "enum class ", t.name, " : co::int16 {\n" )
	for i, id in ipairs( t.identifiers ) do
		writer( "  ", id, ",\n" )
	end
  writer( "};\n" )
end

return template
